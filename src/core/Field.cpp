#include "Field.h"
#include <cstdlib>
#include <stdexcept>

Field::Field(int width, int height) : width_(width), height_(height) {
    if (width < MIN_SIZE || width > MAX_SIZE || height < MIN_SIZE || height > MAX_SIZE) {
        throw std::invalid_argument("Field dimensions exceed acceptable bounds.");
    }
    grid_.resize(height_, std::vector<Cell>(width_));
}

int Field::getWidth() const { return width_; }
int Field::getHeight() const { return height_; }

bool Field::isInside(Point pt) const {
    return pt.x >= 0 && pt.x < width_ && pt.y >= 0 && pt.y < height_;
}

const Cell& Field::getCell(const Point pt) const {
    if (!isInside(pt)) throw std::out_of_range("Coordinates out of field bounds");
    return grid_[pt.y][pt.x];
}

bool Field::isWalkable(Point pt) const {
    if (!isInside(pt)) return false;
    if (!grid_[pt.y][pt.x].isWalkable()) return false;
    if (getFactoryAt(pt) != nullptr) return false;
    return true;
}

Robot* Field::getRobotAt(Point pt) const {
    for (const auto& [pos, robot] : robots_) {
        if (pos == pt && robot->isAlive()) {
            return robot.get();
        }
    }
    return nullptr;
}

std::optional<Point> Field::getRobotPosition(const Robot* robot) const {
    for (const auto& [pos, r] : robots_) {
        if (r.get() == robot) {
            return pos;
        }
    }
    return std::nullopt;
}

RobotFactory* Field::getFactoryAt(const Point pt) const {
    for (const auto& factory : factories_) {
        if (factory->occupies(pt)) {
            return factory.get();
        }
    }
    return nullptr;
}

const std::vector<std::pair<Point, std::unique_ptr<Robot>>>& Field::getRobots() const {
    return robots_;
}

const std::vector<std::unique_ptr<RobotFactory>>& Field::getFactories() const {
    return factories_;
}


Cell& Field::getCell(Point pt) {
    if (!isInside(pt)) throw std::out_of_range("Coordinates out of field bounds");
    return grid_[pt.y][pt.x];
}

bool Field::addRobot(std::unique_ptr<Robot> robot, Point pt) {
    if (!isWalkable(pt) || getRobotAt(pt) != nullptr) {
        return false;
    }
    robots_.emplace_back(pt, std::move(robot));
    getCell(pt).applyStatuses(*robots_.back().second);
    return true;
}

bool Field::addFactory(std::unique_ptr<RobotFactory> factory) {
    Point tl = factory->getTopLeft();
    if (!isInside(tl) || !isInside({tl.x + 1, tl.y + 1})) {
        return false;
    }
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            Point p{tl.x + dx, tl.y + dy};
            if (!isWalkable(p) || getRobotAt(p) != nullptr) {
                return false;
            }
        }
    }
    factories_.push_back(std::move(factory));
    return true;
}

ActionResult Field::moveRobot(Robot* robot, Point destination, bool canMove) {
    if (!robot || !robot->isAlive() || !isInside(destination)) {
        return ActionResult::Blocked;
    }

    auto currentPosOpt = getRobotPosition(robot);
    if (!currentPosOpt.has_value()) return ActionResult::Blocked;

    const Point current = currentPosOpt.value();
    if (std::abs(destination.x - current.x) + std::abs(destination.y - current.y) != 1) {
        return ActionResult::Blocked;
    }

    if (!isWalkable(destination)) {
        return ActionResult::Blocked;
    }

    Robot* target = getRobotAt(destination);
    if (target != nullptr) {
        if (!robot->isPlayer() && !target->isPlayer()) return ActionResult::Blocked;
        if (robot->getEnergy() < Robot::InteractionEnergyCost) return ActionResult::NoEnergy;
        return robot->interact(*target) ? ActionResult::Interacted : ActionResult::Blocked;
    }

    if (!canMove) return ActionResult::NoMoves;

    for (auto& [pos, r] : robots_) {
        if (r.get() == robot) {
            const int terrainCost = getCell(destination).getMoveCost();
            const int moveCost = robot->getMoveEnergyCost();
            if (terrainCost > robot->getEnergy() / moveCost) return ActionResult::NoEnergy;
            if (!robot->consumeEnergy(terrainCost * moveCost)) return ActionResult::NoEnergy;
            pos = destination;
            getCell(destination).applyStatuses(*robot);
            return ActionResult::Moved;
        }
    }
    return ActionResult::Blocked;
}

bool Field::teleportRobot(Robot* robot, Point destination) {
    if (!robot || !robot->isAlive() || !isWalkable(destination) || getRobotAt(destination)) {
        return false;
    }

    for (auto& [position, currentRobot] : robots_) {
        if (currentRobot.get() == robot) {
            position = destination;
            getCell(destination).applyStatuses(*robot);
            return true;
        }
    }
    return false;
}

void Field::cleanupDeadRobots() {
    std::erase_if(robots_, [](const auto& pair) {
        return !pair.second->isAlive();
    });
}

void Field::updateFogOfWar(Point playerPos, int viewRadius) {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            Point pt{x, y};
            if (playerPos.distance(pt) <= viewRadius) {
                grid_[y][x].setDiscovered(true);
            }
        }
    }
}

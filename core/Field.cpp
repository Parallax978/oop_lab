#include "../Field.h"
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

const Cell& Field::getCell(Point pt) const {
    if (!isInside(pt)) throw std::out_of_range("Coordinates out of field bounds");
    return grid_[pt.y][pt.x];
}

Cell& Field::getCell(Point pt) {
    if (!isInside(pt)) throw std::out_of_range("Coordinates out of field bounds");
    return grid_[pt.y][pt.x];
}

bool Field::isWalkable(Point pt) const {
    if (!isInside(pt)) return false;
    if (!grid_[pt.y][pt.x].isWalkable()) return false;
    if (getFactoryAt(pt) != nullptr) return false; // Завод 2х2 непроходим
    return true;
}

bool Field::addRobot(std::unique_ptr<Robot> robot, Point pt) {
    if (!isWalkable(pt) || getRobotAt(pt) != nullptr) {
        return false;
    }
    robots_.emplace_back(pt, std::move(robot));
    return true;
}

bool Field::addFactory(std::unique_ptr<RobotFactory> factory) {
    Point tl = factory->getTopLeft();
    // Проверяем, помещается ли 2х2 в поле
    if (!isInside(tl) || !isInside({tl.x + 1, tl.y + 1})) {
        return false;
    }
    // Проверяем, не заняты ли уже эти клетки
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            Point p{tl.x + dx, tl.y + dy};
            if (!grid_[p.y][p.x].isWalkable() || getRobotAt(p) != nullptr || getFactoryAt(p) != nullptr) {
                return false;
            }
        }
    }
    factories_.push_back(std::move(factory));
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

RobotFactory* Field::getFactoryAt(Point pt) const {
    for (const auto& factory : factories_) {
        if (factory->occupies(pt)) {
            return factory.get();
        }
    }
    return nullptr;
}

bool Field::moveRobot(Robot* robot, Point destination) {
    if (!robot || !robot->isAlive() || !isInside(destination)) {
        return false;
    }

    auto currentPosOpt = getRobotPosition(robot);
    if (!currentPosOpt.has_value()) return false;
    Point currentPos = currentPosOpt.value();

    // Нельзя наступать на непроходимую клетку (стены, заводы)
    if (!isWalkable(destination)) {
        return false;
    }

    Robot* target = getRobotAt(destination);
    if (target != nullptr) {
        // Клетка занята другим роботом -> взаимодействие
        robot->interact(*target);
        return true;
    }

    // Клетка свободна -> перемещаем
    for (auto& [pos, r] : robots_) {
        if (r.get() == robot) {
            pos = destination;
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

const std::vector<std::pair<Point, std::unique_ptr<Robot>>>& Field::getRobots() const {
    return robots_;
}

const std::vector<std::unique_ptr<RobotFactory>>& Field::getFactories() const {
    return factories_;
}
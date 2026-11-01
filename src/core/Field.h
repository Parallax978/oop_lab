#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Point.h"
#include "Cell.h"
#include "Robot.h"
#include "RobotFactory.h"

class Field {
public:
    static constexpr int MIN_SIZE = 5;
    static constexpr int MAX_SIZE = 50;

    Field(int width, int height);

    int getWidth() const;
    int getHeight() const;

    bool isInside(Point pt) const;

    const Cell& getCell(Point pt) const;

    bool isWalkable(Point pt) const;

    Robot* getRobotAt(Point pt) const;

    std::optional<Point> getRobotPosition(const Robot* robot) const;

    RobotFactory* getFactoryAt(Point pt) const;

    const std::vector<std::pair<Point, std::unique_ptr<Robot>>>& getRobots() const;

    const std::vector<std::unique_ptr<RobotFactory>>& getFactories() const;


    Cell& getCell(Point pt);

    bool addRobot(std::unique_ptr<Robot> robot, Point pt);

    bool addFactory(std::unique_ptr<RobotFactory> factory);

    bool moveRobot(Robot* robot, Point destination);

    void cleanupDeadRobots();

    void updateFogOfWar(Point playerPos, int viewRadius);

private:
    int width_;
    int height_;
    std::vector<std::vector<Cell>> grid_;
    std::vector<std::pair<Point, std::unique_ptr<Robot>>> robots_;
    std::vector<std::unique_ptr<RobotFactory>> factories_;
};
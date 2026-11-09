#pragma once
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>
#include "IAbility.h"
#include "IStatus.h"

class Robot {
public:
    static constexpr int MoveEnergyCost = 15;
    static constexpr int EnemyMoveEnergyCost = 20;
    static constexpr int InteractionEnergyCost = 20;
    static constexpr double AllyLevelScale = 1.2;
    static constexpr double EnemyLevelScale = 1.1;
    explicit Robot(bool isPlayer, int maxHp = 100, int attack = 20, int maxEnergy = 50, int speed = 3, int viewRadius = 4);


    bool isPlayer() const { return isPlayer_; };
    bool isAlive() const { return hp_ > 0; };

    int getHp() const { return hp_; };
    int getMaxHp() const { return static_cast<int>(std::round(maxHp_)); };

    int getEnergy() const { return energy_; };
    int getMaxEnergy() const { return static_cast<int>(std::round(maxEnergy_)); };

    int getAttack() const { return static_cast<int>(std::round(attack_)); };

    int getRank() const { return rank_; };
    int getExp() const { return exp_; };
    int getExpToNextRank() const { return expToNextRank_; };

    int getSpeed() const { return static_cast<int>(std::round(speed_)); };
    int getMoveEnergyCost() const { return isPlayer_ ? MoveEnergyCost : EnemyMoveEnergyCost; }
    int getViewRadius() const { return static_cast<int>(std::round(viewRadius_)); };


    void takeDamage(int amount);
    void heal(int amount);

    bool consumeEnergy(int amount);
    void restoreEnergy(int amount);

    void addExp(int amount);

    bool interact(Robot& target);

    bool addAbility(std::unique_ptr<IAbility> ability);
    std::size_t getAbilityCount() const;
    const IAbility* getAbility(std::size_t index) const;
    bool upgradeAbility(std::size_t index);
    bool useAbility(std::size_t index, Field& field, const Point& target);

    bool addStatus(std::unique_ptr<IStatus> status);
    std::size_t getStatusCount() const;
    const IStatus* getStatus(std::size_t index) const;
    bool canMove() const;
    void processStatusesOnTurnEnd();

private:
    void levelUp();
    void removeExpiredStatuses();

    bool isPlayer_{false};
    int hp_{100};
    double maxHp_{100.0};
    double attack_{20.0};

    int energy_{50};
    double maxEnergy_{50.0};

    int exp_{0};
    int expToNextRank_{100};
    int rank_{1};

    double speed_{3.0};
    double viewRadius_{4.0};

    std::vector<std::unique_ptr<IAbility>> abilities_;
    std::vector<std::unique_ptr<IStatus>> statuses_;
    bool processingStatusTurn_{false};
};

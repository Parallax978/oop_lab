#pragma once
#include <cmath>

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

private:
    void levelUp();

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
};

#pragma once

class Robot {
public:
    static constexpr int MoveEnergyCost = 15;
    static constexpr int InteractionEnergyCost = 20;
    explicit Robot(bool isPlayer, int maxHp = 100, int attack = 20, int maxEnergy = 50, int speed = 3, int viewRadius = 4);


    bool isPlayer() const { return isPlayer_; };
    bool isAlive() const { return hp_ > 0; };

    int getHp() const { return hp_; };
    int getMaxHp() const { return maxHp_; };

    int getEnergy() const { return energy_; };
    int getMaxEnergy() const { return maxEnergy_; };

    int getAttack() const { return attack_; };

    int getRank() const { return rank_; };
    int getExp() const { return exp_; };
    int getExpToNextRank() const { return expToNextRank_; };

    int getSpeed() const { return speed_; };
    int getViewRadius() const { return viewRadius_; };


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
    int maxHp_{100};
    int attack_{20};

    int energy_{50};
    int maxEnergy_{50};

    int exp_{0};
    int expToNextRank_{100};
    int rank_{1};

    int speed_{3};
    int viewRadius_{4};
};

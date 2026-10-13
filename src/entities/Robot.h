#pragma once

class Robot {
public:
    Robot(bool isPlayer, int maxHp = 100, int attack = 20, int maxEnergy = 50, int speed = 3, int viewRadius = 4);

    bool isPlayer() const;
    bool isAlive() const;

    int getHp() const;
    int getMaxHp() const;
    void takeDamage(int amount);
    void heal(int amount);

    int getEnergy() const;
    int getMaxEnergy() const;
    bool consumeEnergy(int amount);
    void restoreEnergy(int amount);

    int getAttack() const;
    int getRank() const;
    int getExp() const;
    int getExpToNextRank() const;
    void addExp(int amount);

    int getSpeed() const;
    int getViewRadius() const;
    
    void interact(Robot& target);

private:
    void levelUp();

    bool isPlayer_;
    int hp_;
    int maxHp_;
    int attack_;

    int energy_;
    int maxEnergy_;

    int exp_{0};
    int expToNextRank_{100};
    int rank_{1};

    int speed_;
    int viewRadius_;
};
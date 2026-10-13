#include "Robot.h"
#include <algorithm>

Robot::Robot(bool isPlayer, int maxHp, int attack, int maxEnergy, int speed, int viewRadius)
    : isPlayer_(isPlayer),
      hp_(std::max(1, maxHp)),
      maxHp_(std::max(1, maxHp)),
      attack_(std::max(1, attack)),
      energy_(std::max(0, maxEnergy)),
      maxEnergy_(std::max(0, maxEnergy)),
      speed_(std::max(1, speed)),
      viewRadius_(std::max(1, viewRadius)) {}

bool Robot::isPlayer() const { return isPlayer_; }
bool Robot::isAlive() const { return hp_ > 0; }

int Robot::getHp() const { return hp_; }
int Robot::getMaxHp() const { return maxHp_; }

void Robot::takeDamage(int amount) {
    if (amount <= 0) return;
    hp_ = std::max(0, hp_ - amount);
}

void Robot::heal(int amount) {
    if (amount <= 0 || !isAlive()) return;
    hp_ = std::min(maxHp_, hp_ + amount);
}

int Robot::getEnergy() const { return energy_; }
int Robot::getMaxEnergy() const { return maxEnergy_; }

bool Robot::consumeEnergy(int amount) {
    if (amount <= 0) return true;
    if (energy_ >= amount) {
        energy_ -= amount;
        return true;
    }
    return false;
}

void Robot::restoreEnergy(int amount) {
    if (amount <= 0) return;
    energy_ = std::min(maxEnergy_, energy_ + amount);
}

int Robot::getAttack() const { return attack_; }
int Robot::getRank() const { return rank_; }
int Robot::getExp() const { return exp_; }
int Robot::getExpToNextRank() const { return expToNextRank_; }

void Robot::addExp(int amount) {
    if (amount <= 0 || !isAlive()) return;
    exp_ += amount;
    while (exp_ >= expToNextRank_) {
        exp_ -= expToNextRank_;
        levelUp();
    }
}

void Robot::levelUp() {
    rank_++;
    maxHp_ += 20;
    hp_ = maxHp_;
    attack_ += 5;
    maxEnergy_ += 10;
    energy_ = maxEnergy_;
    expToNextRank_ = static_cast<int>(expToNextRank_ * 1.5);
}

int Robot::getSpeed() const { return speed_; }
int Robot::getViewRadius() const { return viewRadius_; }

void Robot::interact(Robot& target) {
    if (!isAlive() || !target.isAlive()) return;

    if (isPlayer_ != target.isPlayer()) {
        target.takeDamage(attack_);
        if (!target.isAlive()) {
            addExp(50 * target.getRank());
        }
    } else {
        target.heal(15 + rank_ * 5);
    }
}
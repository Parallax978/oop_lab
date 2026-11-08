#include "Robot.h"
#include <algorithm>
#include <utility>

Robot::Robot(bool isPlayer, int maxHp, int attack, int maxEnergy, int speed, int viewRadius)
    : isPlayer_(isPlayer),
      hp_(std::max(1, maxHp)),
      maxHp_(std::max(1, maxHp)),
      attack_(std::max(1, attack)),
      energy_(std::max(0, maxEnergy)),
      maxEnergy_(std::max(0, maxEnergy)),
      speed_(std::max(1, speed)),
      viewRadius_(std::max(1, viewRadius)) {}


void Robot::takeDamage(int amount) {
    if (amount <= 0) return;
    hp_ = std::max(0, hp_ - amount);
}

void Robot::heal(int amount) {
    if (amount <= 0 || !isAlive()) return;
    hp_ = std::min(getMaxHp(), hp_ + amount);
}

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
    energy_ = std::min(getMaxEnergy(), energy_ + amount);
}

void Robot::addExp(int amount) {
    if (amount <= 0 || !isAlive()) return;
    exp_ += amount;
    while (exp_ >= expToNextRank_) {
        exp_ -= expToNextRank_;
        levelUp();
    }
}

bool Robot::interact(Robot& target) {
    if (this == &target || !isAlive() || !target.isAlive()) return false;
    if (!isPlayer_ && !target.isPlayer()) return false;
    if (!consumeEnergy(InteractionEnergyCost)) return false;

    if (isPlayer_ != target.isPlayer()) {
        target.takeDamage(getAttack());
        if (!target.isAlive()) {
            addExp(50 * target.getRank());
        }
    } else {
        target.heal(15 + rank_ * 5);
    }
    return true;
}

bool Robot::addAbility(std::unique_ptr<IAbility> ability) {
    if (!ability) return false;
    abilities_.push_back(std::move(ability));
    return true;
}

std::size_t Robot::getAbilityCount() const {
    return abilities_.size();
}

const IAbility* Robot::getAbility(std::size_t index) const {
    if (index >= abilities_.size()) return nullptr;
    return abilities_[index].get();
}

bool Robot::useAbility(std::size_t index, Field& field, const Point& target) {
    if (!isAlive() || index >= abilities_.size()) return false;
    return abilities_[index]->use(*this, field, target);
}

void Robot::levelUp() {
    rank_++;
    const double scale = isPlayer_ ? AllyLevelScale : EnemyLevelScale;
    maxHp_ *= scale;
    attack_ *= scale;
    maxEnergy_ *= scale;
    speed_ *= scale;
    viewRadius_ *= scale;
    hp_ = getMaxHp();
    energy_ = getMaxEnergy();
    expToNextRank_ = static_cast<int>(expToNextRank_ * 1.5);
}

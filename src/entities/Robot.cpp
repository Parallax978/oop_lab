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
    if (amount <= 0 || !isAlive()) return;
    for (const auto& status : statuses_) {
        if (!status->isExpired()) {
            amount = std::max(0, status->processDamage(amount));
            if (amount == 0) break;
        }
    }
    hp_ = std::max(0, hp_ - amount);
    removeExpiredStatuses();
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
    bool allowed = true;
    for (const auto& status : statuses_) {
        if (!status->isExpired() && !status->onAbilityAttempt()) allowed = false;
    }
    removeExpiredStatuses();
    if (!allowed) return false;
    return abilities_[index]->use(*this, field, target);
}

bool Robot::addStatus(std::unique_ptr<IStatus> status) {
    if (!isAlive() || !status || status->isExpired()) return false;

    removeExpiredStatuses();
    for (const auto& current : statuses_) {
        if (!current->isExpired() && current->merge(*status)) return true;
    }
    statuses_.push_back(std::move(status));
    return true;
}

std::size_t Robot::getStatusCount() const {
    return statuses_.size();
}

const IStatus* Robot::getStatus(std::size_t index) const {
    if (index >= statuses_.size()) return nullptr;
    return statuses_[index].get();
}

bool Robot::canMove() const {
    if (!isAlive()) return false;
    for (const auto& status : statuses_) {
        if (!status->isExpired() && !status->canMove()) return false;
    }
    return true;
}

void Robot::processStatusesOnTurnEnd() {
    if (!isAlive()) return;

    processingStatusTurn_ = true;
    for (const auto& status : statuses_) {
        if (!status->isExpired()) status->onTurnEnd(*this);
        if (!isAlive()) break;
    }
    processingStatusTurn_ = false;
    removeExpiredStatuses();
}

void Robot::removeExpiredStatuses() {
    if (processingStatusTurn_) return;
    std::erase_if(statuses_, [](const auto& status) {
        return status->isExpired();
    });
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

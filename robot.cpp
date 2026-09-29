#include "robot.h"

robot::robot(int health, int maxHealth, int attackDamage, bool isFriendly) 
    : health(maxHealth), maxHealth(maxHealth), attackDamage(attackDamage), isFriendly(isFriendly), level(0){}

void takeDamage(int health, int damage) {
    if (health - damage >= 0) {
        health = health - damage;
    } else {
        health = 0;
    }
}
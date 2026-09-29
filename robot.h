#pragma onse

class robot {
private:
    int health;
    int maxHealth;
    int level;
    int attackDamage;
    bool isFriendly;
public:
    robot(int health, int maxHealth, int attackDamage, bool isFriendly);

    void takeDamage(int damage);
    void getHealth(int heal);
};
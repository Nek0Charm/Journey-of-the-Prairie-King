#ifndef PLAYER_H
#define PLAYER_H

class Player {
    private:
        int health;
        int score;
        double speed;
        double damage;

    public:
        explicit Player(int health = 100, int score = 0, double speed = 1.0, double damage = 10.0);
        virtual ~Player();
        int getHealth() const;
        int getScore() const;
        double getSpeed() const;
        double getDamage() const;
        void setHealth(int newHealth){
            if(newHealth < 0) {
                newHealth = 0;
            }
            health = newHealth;
        }
        void setScore(int newScore){
            score = newScore;
        }
        void setSpeed(double newSpeed){
            speed = newSpeed;
        }
        void setDamage(double newDamage){
            damage = newDamage;
        }
};

#endif
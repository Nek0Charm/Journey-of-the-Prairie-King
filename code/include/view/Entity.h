#ifndef ENTITY_H
#define ENTITY_H
#include <QObject> 
#include <QPointF>
#include <QPainter>
#include <QMap>
#include "view/Animation.h"
#include "view/SpriteManager.h"

class Entity : public QObject {
    Q_OBJECT
public:
    explicit Entity(QObject* parent = nullptr);
    virtual ~Entity();

    virtual void update(double deltaTime);
    virtual void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset);

    void setPosition(const QPointF& pos) { m_position = pos; }
    const QPointF& getPosition() const { return m_position; }
    QPointF m_position;
};

enum class PlayerState {
    Idle,
    WalkDown,
    WalkUp,
    WalkLeft,
    WalkRight,
    ShootDown,
    ShootUp,
    ShootLeft,
    ShootRight,
    ShootDownWalk,
    ShootUpWalk,
    ShootLeftWalk,
    ShootRightWalk
};
class PlayerEntity : public Entity {
public:
    PlayerEntity(QObject* parent = nullptr);
    ~PlayerEntity() override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(PlayerState newState);
    void update(double deltaTime);
    bool isVisible() const;
    bool isInvincible() const {return m_isInvincible;}
    void setInvincible(bool res) {m_isInvincible = res;}
    void setInvincibilityTime(int t) {m_invincibilityTimer = t;}
private:
    QMap<PlayerState, Animation*> m_animations;
    PlayerState m_currentState; 
    Animation* m_currentAnimation;
    int m_invincibilityTimer = 0;
    bool m_isInvincible = false;
};


enum class MonsterState {
    Walking,
    Dying, 
    Dead   
};

class MonsterEntity : public Entity {
    Q_OBJECT
public:
    explicit MonsterEntity(const QString& monsterType, QObject* parent = nullptr);
    ~MonsterEntity() override;
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(MonsterState newState) { m_currentState = newState; }
    void setVelocity(const QPointF& velocity);
    QString getType() const {return monsterType;}
private:
    QMap<MonsterState, Animation*> m_animations;
    Animation* m_animation;    
    QPointF m_velocity; 
    QString monsterType;
    MonsterState m_currentState; 
};

enum class DeadMonsterState {
    Dying, 
    Dead   
};

class DeadMonsterEntity : public Entity {
    Q_OBJECT
public:
    explicit DeadMonsterEntity(const QString& monsterType, QObject* parent = nullptr);
    explicit DeadMonsterEntity(const MonsterEntity& monserentity);
    ~DeadMonsterEntity() override;
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(DeadMonsterState newState) { m_currentState = newState; }
    bool ShouldbeRemove() { return m_lingerTimer <= 0; }
private:
    Animation* m_animation;
    QString monsterType;
    DeadMonsterState m_currentState;
    double m_lingerTimer;
};

#endif // ENTITY_H
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
private:
    QMap<PlayerState, Animation*> m_animations;
    PlayerState m_currentState; 
    Animation* m_currentAnimation;
};

class MonsterEntity : public Entity {
    Q_OBJECT
public:
    explicit MonsterEntity(const QString& monsterType, QObject* parent = nullptr);
    ~MonsterEntity() override;
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    
    void setVelocity(const QPointF& velocity);

private:
    Animation* m_animation;    
    QPointF m_velocity; 
    QString monsterType;
};


#endif // ENTITY_H
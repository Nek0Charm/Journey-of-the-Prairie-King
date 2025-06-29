#ifndef ENTITY_H
#define ENTITY_H
#include <QObject> // 可以继承QObject以便未来使用信号槽，或者不继承任何东西
#include <QPointF>
#include "view/Animation.h"
#include "view/SpriteManager.h"

class Entity : public QObject {
    Q_OBJECT
public:
    Entity(const QString& animationName, QObject* parent = nullptr);
    ~Entity();
    void update(double deltaTime, QPointF new_pos, QPointF shoot_direction);
    void update(double deltaTime, QPointF new_pos);
    void paint(QPainter* painter, const QPixmap& spriteSheet);
    void setPosition(const QPointF& pos) { m_position = pos; }
    const QPointF& getPosition() const { return m_position; }
    void setDirection(const QPointF& dir) { direction = dir; }
    const QPointF& getDirection() const { return direction; }
private:
    QPointF m_position;
    Animation* m_animation;
    QPointF direction = {2, 2};
};

#endif // ENTITY_H
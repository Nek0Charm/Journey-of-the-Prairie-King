#include "view/Entity.h"
#include <QPainter>

Entity::Entity(const QString& animationName, QObject* parent) : QObject(parent) {
    QList<QString> frameNames = SpriteManager::instance().getAnimationSequence(animationName);
    if (!frameNames.isEmpty()) {
        m_animation = new Animation(frameNames, 8.0, true);
    } else {
        m_animation = nullptr;
    }
    m_position = QPointF(16*8, 16*8);
}

Entity::~Entity() {
    delete m_animation;
}

void Entity::update(double deltaTime, QPointF new_pos, QPointF shoot_direction) {
    if (shoot_direction != direction) {
        direction = shoot_direction;
        if (shoot_direction == QPointF(0, 1)) {
            QList<QString> frameNames = SpriteManager::instance().getAnimationSequence("player_walk_down");
            m_animation = new Animation(frameNames, 8.0, true);
        } else if (shoot_direction == QPointF(0, -1)) {
            QList<QString> frameNames = SpriteManager::instance().getAnimationSequence("player_walk_up");
            m_animation = new Animation(frameNames, 8.0, true);
        } else if (shoot_direction == QPointF(-1, 0)) {
            QList<QString> frameNames = SpriteManager::instance().getAnimationSequence("player_walk_left");
            m_animation = new Animation(frameNames, 8.0, true);
        } else if (shoot_direction == QPointF(1, 0)) {
            QList<QString> frameNames = SpriteManager::instance().getAnimationSequence("player_walk_right");
            m_animation = new Animation(frameNames, 8.0, true);
        }
    }
    
    if (m_animation) {
        m_animation->update(deltaTime);
    }
    setPosition(new_pos);
}

void Entity::update(double deltaTime, QPointF new_pos) {
    if (m_animation) {
        m_animation->update(deltaTime);
    }
    setPosition(new_pos);
}

void Entity::paint(QPainter* painter, const QPixmap& spriteSheet) {
    if (!m_animation) return;
    const QString& currentFrameName = m_animation->getCurrentFrameName();
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 5.0; 
    QPointF destinationAnchor((m_position.x() + 27)*scale, (m_position.y() + 16)*scale);

    if (!parts.isEmpty()) {
        for (const SpritePart& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destinationRect(finalPos, sourceRect.size() * scale);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    } else {
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    }
}



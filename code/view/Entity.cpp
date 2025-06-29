#include "view/Entity.h"
#include "view/SpriteManager.h"
#include "Entity.h"

Entity::Entity(QObject* parent) : QObject(parent), m_position(0, 0) {};

Entity::~Entity() {}

void Entity::update(double deltaTime) {}
void Entity::paint(QPainter* painter, const QPixmap& spriteSheet) {}

PlayerEntity::PlayerEntity(QObject *parent) : Entity(parent) {
    m_currentState = PlayerState::Idle;
    m_animations[PlayerState::Idle]      = new Animation(SpriteManager::instance().getAnimationSequence("player_idle"), 8.0, true);
    m_animations[PlayerState::WalkDown]  = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_down"), 8.0, true);
    m_animations[PlayerState::WalkUp]    = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_up"), 8.0, true);
    m_animations[PlayerState::WalkLeft]  = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_left"), 8.0, true);
    m_animations[PlayerState::WalkRight] = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_right"), 8.0, true);
    m_animations[PlayerState::ShootDown]  = new Animation(SpriteManager::instance().getAnimationSequence("player_shoot_down"), 8.0, true);
    m_animations[PlayerState::ShootUp]    = new Animation(SpriteManager::instance().getAnimationSequence("player_shoot_up"), 8.0, true);
    m_animations[PlayerState::ShootLeft]  = new Animation(SpriteManager::instance().getAnimationSequence("player_shoot_left"), 8.0, true);
    m_animations[PlayerState::ShootRight] = new Animation(SpriteManager::instance().getAnimationSequence("player_shoot_right"), 8.0, true);
    m_animations[PlayerState::ShootDownWalk]  = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_down"), 8.0, true);
    m_animations[PlayerState::ShootUpWalk]    = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_up"), 8.0, true);
    m_animations[PlayerState::ShootLeftWalk]  = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_left"), 8.0, true);
    m_animations[PlayerState::ShootRightWalk] = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_right"), 8.0, true);
    m_currentAnimation = m_animations.value(m_currentState, nullptr);
    m_position = QPointF(16*8, 16*8);
}

PlayerEntity::~PlayerEntity() {
    qDeleteAll(m_animations);
}

void PlayerEntity::setState(PlayerState newState) {
    if (m_currentState == newState) return; 

    m_currentState = newState;
    m_currentAnimation = m_animations.value(m_currentState, nullptr);
    if (m_currentAnimation) {
        m_currentAnimation->reset(); 
    }
}

void PlayerEntity::update(double deltaTime) {
    if (m_currentAnimation) {
        m_currentAnimation->update(deltaTime);
    }
}

void PlayerEntity::paint(QPainter* painter, const QPixmap& spriteSheet) {
    if (!m_currentAnimation) return;
    const QString& currentFrameName = m_currentAnimation->getCurrentFrameName();
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

MonsterEntity::MonsterEntity(const QString &monsterType, QObject *parent) : Entity(parent), monsterType(monsterType) {
    m_velocity = QPointF(0, 0);
    if (monsterType == "orc") {
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("orc_walk"));
    }
}

MonsterEntity::~MonsterEntity() {
    delete m_animation;
}

void MonsterEntity::setVelocity(const QPointF& velocity) {
    m_velocity = velocity;
}

void MonsterEntity::update(double deltaTime) {
    m_position += m_velocity * deltaTime;
    if (m_animation) {
        m_animation->update(deltaTime);
    }
}

void MonsterEntity::paint(QPainter* painter, const QPixmap& spriteSheet) {
    if (!m_animation) return;

    const QString& currentFrameName = m_animation->getCurrentFrameName();
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 5.0; 
    QPointF destinationAnchor((m_position.x() + 27)*scale, (m_position.y() + 16)*scale);

    if (!parts.isEmpty()) {
        for (const auto& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destRect(finalPos, sourceRect.size() * scale);
            painter->drawPixmap(destRect, spriteSheet, sourceRect);
        }
    } else {
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    }
}
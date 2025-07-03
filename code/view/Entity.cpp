#include "view/Entity.h"

Entity::Entity(QObject* parent) : QObject(parent), m_position(0, 0) {};

Entity::~Entity() {}

void Entity::update(double deltaTime) {}
void Entity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {}

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
    m_animations[PlayerState::Dying] = new Animation(SpriteManager::instance().getAnimationSequence("player_dying"), 8.0, false);
    m_animations[PlayerState::Zombie] = new Animation(SpriteManager::instance().getAnimationSequence("player_zombie"), 8.0, true);
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

bool PlayerEntity::isVisible() const {
    if (!m_isInvincible) {
        return true;
    }
    return static_cast<int>(m_invincibilityTimer / 10) % 2 == 0;
}

void PlayerEntity::update(double deltaTime) {
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime; 
        if (m_invincibilityTimer <= 0) {
            m_isInvincible = false; 
            m_invincibilityTimer = 0;
            // qDebug() << "无敌时间结束。";
        }
    }
    if (m_currentAnimation) {
        m_currentAnimation->update(deltaTime);
    }
}

void PlayerEntity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {
    if (!m_currentAnimation) return;
    if (!isVisible()) return;
    const QString& currentFrameName = m_currentAnimation->getCurrentFrameName();
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 3.0; 
    QPointF destinationAnchor(m_position.x()*scale, m_position.y()*scale);

    if (!parts.isEmpty()) {
        for (const SpritePart& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destinationRect(finalPos, sourceRect.size() * scale);
            destinationRect.translate(viewOffset);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    } else {
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            destinationRect.translate(viewOffset);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    }
}

MonsterEntity::MonsterEntity(const MonsterType &monsterType, QObject *parent) : Entity(parent), monsterType(monsterType) {
    m_currentState = MonsterState::Walking;
    m_velocity = QPointF(0, 0);
    switch (monsterType) {
    case MonsterType::orc:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("orc_walk"), 8.0, true);
        break;
    case MonsterType::spikeball:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_walk"), 8.0, true);
        break;
    case MonsterType::ogre:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("ogre_walk"), 8.0, true);
        break;
    case MonsterType::mushroom:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("mushroom_walk"), 8.0, true);
        break;
    case MonsterType::pixie:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("pixie_walk"), 8.0, true);
        break;
    case MonsterType::mummy:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("mummy_walk"), 8.0, true);
        break;
    case MonsterType::imp:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("imp_walk"), 8.0, true);
        break;
    default:
        break;
    }
}

MonsterEntity::~MonsterEntity() {
    delete m_animation;
}

void MonsterEntity::setVelocity(const QPointF& velocity) {
    m_velocity = velocity;
}

void MonsterEntity::update(double deltaTime) {
    // if (shouldBeRemoved()) return;
    if (m_animation) {
        m_animation->update(deltaTime);
    }
    // 如果被冻结，完全不更新
    if (m_isFrozen) {
        return;
    }
    
    switch (m_currentState) {
        case MonsterState::Walking:
            m_position += m_velocity * deltaTime;
            break;
    }
}

void MonsterEntity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {
    if (!m_animation) return;

    const QString& currentFrameName = m_animation->getCurrentFrameName();
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 3.0; 
    QPointF destinationAnchor(m_position.x()*scale, m_position.y()*scale);

    if (!parts.isEmpty()) {
        for (const auto& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destRect(finalPos, sourceRect.size() * scale);
            destRect.translate(viewOffset);
            painter->drawPixmap(destRect, spriteSheet, sourceRect);
        }
    } else {
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            destinationRect.translate(viewOffset);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    }
}

DeadMonsterEntity::DeadMonsterEntity(const MonsterEntity &monserentity) 
    : m_lingerTimer(20), m_currentState(DeadMonsterState::Dying), monsterType(monserentity.getType()){
    m_position = monserentity.getPosition();
    switch (monsterType) {
    case MonsterType::orc:
    case MonsterType::spikeball:
    case MonsterType::ogre:
    case MonsterType::mushroom:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("orc_die"), 7.0, false);
        break;
    case MonsterType::pixie:
    case MonsterType::imp:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("pixie_die"), 7.0, false);
        break;
    case MonsterType::mummy:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("explode"), 7.0, false);
        break;
    default:
        break;
    }
}

DeadMonsterEntity::~DeadMonsterEntity() {
    delete m_animation;
}

void DeadMonsterEntity::update(double deltaTime) {
    switch (m_currentState) {
        case DeadMonsterState::Dying:
            if (m_animation->isFinished()) {
                setState(DeadMonsterState::Dead);
            }
            break;
        case DeadMonsterState::Dead:
            if (m_lingerTimer >= 0) {
                m_lingerTimer -= deltaTime;
            }
        default:
            break;
    }
    m_animation->update(deltaTime);
}

void DeadMonsterEntity::paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset) {
    if (!m_animation) return;
    const QString& currentFrameName = m_animation->getCurrentFrameName();
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 3.0; 
    QPointF destinationAnchor(m_position.x()*scale, m_position.y()*scale);

    if (!parts.isEmpty()) {
        for (const auto& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destRect(finalPos, sourceRect.size() * scale);
            destRect.translate(viewOffset);
            painter->drawPixmap(destRect, spriteSheet, sourceRect);
        }
    } else {
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            destinationRect.translate(viewOffset);
            painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
        }
    }
}

ItemEntity::ItemEntity(int itemtype, QObject *parent, QPointF pos)
: Entity(parent), m_lingerTimer(15), m_currentState(ItemState::Drop) {   
    setPosition(pos);
    m_itemType = static_cast<ItemType>(itemtype);
    qDebug() << "m_itemType" << itemtype;
}

void ItemEntity::update(double deltaTime) {
    if (m_lingerTimer > 0) {
        m_lingerTimer -= deltaTime;
    }
    if (m_currentState == ItemState::Drop && m_lingerTimer <= 3.0) {
        setState(ItemState::Flash);
    }
}

void ItemEntity::paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset) {
    if (!isVisible()) return;     
    QString framename = typeToString(m_itemType);
    // qDebug() << "paint " << framename;
    double scale = 3.0; 
    QPointF destinationAnchor(m_position.x()*scale, m_position.y()*scale);
    QRect sourceRect = SpriteManager::instance().getSpriteRect(framename);
    if (!sourceRect.isNull()) {
        QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
        // qDebug() << "position " << destinationRect;
        destinationRect.translate(viewOffset);
        painter->drawPixmap(destinationRect, spriteSheet, sourceRect);
    }
}

bool ItemEntity::isVisible() {
    return !(m_currentState == ItemState::Flash) || (static_cast<int>(m_lingerTimer * 6) % 2 == 0);
}

QString ItemEntity::typeToString(ItemType type) {
    QString stringtype;
    switch (type) {
    case ItemType::coin:
        stringtype = "coin";
        break;
    case ItemType::five_coins:
        stringtype = "five_coins";
        break;
    case ItemType::extra_life:
        stringtype = "extra_life";
        break;
    case ItemType::coffee:
        stringtype = "coffee";
        break;
    case ItemType::machine_gun:
        stringtype = "machine_gun";
        break;
    case ItemType::bomb:
        stringtype = "bomb";
        break;
    case ItemType::shotgun:
        stringtype = "shotgun";
        break;
    case ItemType::smoke_bomb:
        stringtype = "smoke_bomb";
        break;
    case ItemType::tombstone:
        stringtype = "tombstone";
        break;
    case ItemType::wheel:
        stringtype = "wheel";
        break;
    case ItemType::badge:
        stringtype = "badge";
        break;
    default:
        break;
    }
    return stringtype;
}


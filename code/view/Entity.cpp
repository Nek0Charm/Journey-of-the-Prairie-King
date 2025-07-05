#include "view/Entity.h"
#include "Entity.h"

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
    m_animations[PlayerState::Lifting] = new Animation(SpriteManager::instance().getAnimationSequence("player_lifting_heart"), 8.0, true);
    m_animations[PlayerState::Lightning] = new Animation(SpriteManager::instance().getAnimationSequence("player_lightning"), 4.0, true);
    m_animations[PlayerState::Kiss] = new Animation(SpriteManager::instance().getAnimationSequence("kiss"), 8.0, true);
    m_animations[PlayerState::WalkLiftingHeart] = new Animation(SpriteManager::instance().getAnimationSequence("player_walk_lifting_heart"), 8.0, true);
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
    return static_cast<int>(m_invincibilityTimer * 6) % 2 == 0;
}

void PlayerEntity::update(double deltaTime) {  
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime; 
        if (m_invincibilityTimer <= 0) {
            m_isInvincible = false; 
            m_invincibilityTimer = 0;
        }
    }
    if (m_isGamewin) {
        m_invincibilityTimer -= deltaTime;
        // qDebug() << "m_time" << m_invincibilityTimer;
        if (m_invincibilityTimer <= 16 && m_invincibilityTimer >= 7) {
            setState(PlayerState::WalkLiftingHeart);
            m_position += QPointF(deltaTime*16, 0);
        } else if (m_invincibilityTimer < 7 && m_invincibilityTimer >= 0) {
            setState(PlayerState::Lifting);
        } else if (m_invincibilityTimer < 0) {
            setState(PlayerState::Kiss);
        }
        
    }
    if (m_currentAnimation) {
        m_currentAnimation->update(deltaTime);
    }
}

void PlayerEntity::onGameWin() {
    setState(PlayerState::Disappearing);
    setPosition(QPointF(0, 7.2*16));
    m_invincibilityTimer = 20;
    m_isGamewin = true;
}

void PlayerEntity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {
    if (!m_currentAnimation || m_currentState == PlayerState::Disappearing) return;
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

void MonsterEntity::setState(MonsterState newState) {
    if (m_currentState != newState) {
        m_currentState = newState;
        // 根据状态切换动画
        if (monsterType == MonsterType::spikeball) {
            if (m_currentState == MonsterState::Deployed) {
                // 删除当前动画
                if (m_animation) {
                    delete m_animation;
                }
                // 播放部署动画（一次）
                m_animation = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_deploy"), 8.0, false);
            } else {
                // 删除当前动画
                if (m_animation) {
                    delete m_animation;
                }
                // 播放行走动画（循环）
                m_animation = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_walk"), 8.0, true);
            }
        }
    }
}

void MonsterEntity::update(double deltaTime) {
    // if (shouldBeRemoved()) return;
    if (m_animation) {
        m_animation->update(deltaTime);
        
        // 检查Spikeball的部署动画状态
        if (monsterType == MonsterType::spikeball && m_currentState == MonsterState::Deployed && m_animation->isFinished()) {
            // 部署动画完成，切换到静态画面
            QStringList staticFrame;
            staticFrame << "spikeball_deploy_4"; // 使用部署动画的最后一帧
            delete m_animation;
            m_animation = new Animation(staticFrame, 0.0, false); // 0.0速度，不循环
        }
    }
    // 如果被冻结，完全不更新
    if (m_isFrozen) {
        return;
    }
    
    switch (m_currentState) {
        case MonsterState::Walking:
            m_position += m_velocity * deltaTime;
            break;
        case MonsterState::Deployed:
            // 部署后停止移动
            break;
        default:
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
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("orc_die"), 8.0, false);
        break;
    case MonsterType::pixie:
    case MonsterType::imp:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("pixie_die"), 8.0, false);
        break;
    case MonsterType::mummy:
        m_animation = new Animation(SpriteManager::instance().getAnimationSequence("explode"), 8.0, false);
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
    // 供应商道具类型映射
    case ItemType::vendor_boots_1:
        stringtype = "boots_1";  // 靴子1
        break;
    case ItemType::vendor_boots_2:
        stringtype = "boots_2";  // 靴子2
        break;
    case ItemType::vendor_extra_life:
        stringtype = "more_live";  // 额外生命
        break;
    case ItemType::vendor_gun_1:
        stringtype = "gun_1";  // 枪1
        break;
    case ItemType::vendor_gun_2:
        stringtype = "gun_2";  // 枪2
        break;
    case ItemType::vendor_gun_3:
        stringtype = "gun_3";  // 枪3
        break;
    case ItemType::vendor_ammo_1:
        stringtype = "bullet_1";  // 弹药1
        break;
    case ItemType::vendor_ammo_2:
        stringtype = "bullet_2";  // 弹药2
        break;
    case ItemType::vendor_ammo_3:
        stringtype = "bullet_3";  // 弹药3
        break;
    case ItemType::vendor_badge:
        stringtype = "badge";  // 治安官徽章
        break;
    default:
        break;
    }
    return stringtype;
}

VendorEntity::VendorEntity(QObject *parent): Entity(parent) {
    m_currentState = VendorState::Disappearing;
    m_animations[VendorState::LookDown] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_look_down"), 8.0, true);
    m_animations[VendorState::Walk] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_walk"), 8.0, true);
    m_animations[VendorState::LookLeft] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_look_left"), 8.0, true);
    m_animations[VendorState::LookRight] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_look_right"), 8.0, true);
    m_animations[VendorState::Singing] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_singing"), 4.0, true);
    m_currentAnimation = m_animations.value(m_currentState, nullptr);
}

VendorEntity::~VendorEntity() {
    qDeleteAll(m_animations);
}

void VendorEntity::update(double deltaTime, const QPointF& playerPosition) {
    if (m_currentState == VendorState::Disappearing) {
        return;// Don't update position or animation if disappearing
    }
    m_currentAnimation = m_animations.value(m_currentState, nullptr);
    if (m_currentState == VendorState::Walk) {
        // qDebug() << "VendorEntity::update: Walking";
        m_lingerTimer -= deltaTime;
        m_position += QPointF(0, 16 * deltaTime);
        double distance = QLineF(m_position, playerPosition).length();
    }
    if (m_lingerTimer <= 0 && m_currentState != VendorState::Singing) {
        if (playerPosition.x() - m_position.x() > 16) {
            setState(VendorState::LookRight);
        } else if (m_position.x() - playerPosition.x() > 16) {
            setState(VendorState::LookLeft);
        } else {
            setState(VendorState::LookDown);
        }
    } else if (m_currentState == VendorState::Singing) {
        m_lingerTimer -= deltaTime;
        if (m_lingerTimer <= 0) {
            setState(VendorState::Disappearing);
        }
    }
    
    if (m_currentAnimation) {
        // qDebug() << "VendorEntity::update: Current animation is not null";
        m_currentAnimation->update(deltaTime);
    }
}

void VendorEntity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {
    if (m_currentState == VendorState::Disappearing || !m_currentAnimation) {
        return;
    }
    const double scale = 3.0;
    const QString& currentFrameName = m_currentAnimation->getCurrentFrameName();
    // qDebug() << "VendorEntity::paint currentFrameName: " << currentFrameName;
    QRect vendorSourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);

    if (vendorSourceRect.isNull()) return;
    QPointF vendorScreenAnchor = m_position * scale + viewOffset;
    QRectF vendorDestRect(vendorScreenAnchor, vendorSourceRect.size() * scale);
    painter->drawPixmap(vendorDestRect, spriteSheet, vendorSourceRect);

    if (m_currentState != VendorState::Walk && m_currentState != VendorState::Singing) {
        QRect tableclothSourceRect = SpriteManager::instance().getSpriteRect("tablecloth");
        if (tableclothSourceRect.isNull()) return;
        QSizeF tableclothScaledSize(tableclothSourceRect.width() * scale, tableclothSourceRect.height() * scale);
        QPointF tableclothTopLeft(
            vendorDestRect.center().x() - tableclothScaledSize.width() / 2.0,
            vendorDestRect.bottom()
        );
        QRectF tableclothDestRect(tableclothTopLeft, tableclothScaledSize);
        painter->drawPixmap(tableclothDestRect, spriteSheet, tableclothSourceRect); 
        double itemSpacing = 10.0; 
        double allItemsWidth = (m_availableItems.size() * 16 * scale) + ((m_availableItems.size() - 1) * itemSpacing);
        double currentItemX = tableclothDestRect.center().x() - allItemsWidth / 2.0;
        for (int itemType : m_availableItems) {
            QString itemName = ItemEntity::typeToString(static_cast<ItemType>(itemType));
            QRect itemSourceRect = SpriteManager::instance().getSpriteRect(itemName);
            
            if (!itemSourceRect.isNull()) {
                QSizeF itemScaledSize(itemSourceRect.width() * scale, itemSourceRect.height() * scale);
                QPointF itemTopLeft(
                    currentItemX,
                    tableclothDestRect.center().y() - itemScaledSize.height() / 2.0
                );
                QRectF itemDestRect(itemTopLeft, itemScaledSize);
                painter->drawPixmap(itemDestRect, spriteSheet, itemSourceRect);
                currentItemX += itemScaledSize.width() + itemSpacing;
            }
        }
    }
}

void VendorEntity::onVendorDisappear() {
    setState(VendorState::Disappearing);
}

void VendorEntity::onVendorAppear() {
    setState(VendorState::Walk);
    m_lingerTimer = 6.0;
    m_position = QPointF(16*8, 0);
}

void VendorEntity::onGameWin() {
    setState(VendorState::Singing);
    m_lingerTimer = 20;
    setPosition(QPointF(16*10,16*8));
}
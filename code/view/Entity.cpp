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
    m_animations[PlayerState::Lifting] = new Animation(SpriteManager::instance().getAnimationSequence("player_lifting"), 8.0, true);
    m_animations[PlayerState::Lightning] = new Animation(SpriteManager::instance().getAnimationSequence("player_lightning"), 4.0, true);
    m_animations[PlayerState::LiftingHeart] = new Animation(SpriteManager::instance().getAnimationSequence("player_lifting_heart"), 8.0, false);
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
            setState(PlayerState::LiftingHeart);
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
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("orc_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("orc_hit"), 8.0, false);
        break;
    case MonsterType::spikeball:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_hit"), 8.0, false);
        break;
    case MonsterType::ogre:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("ogre_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("ogre_hit"), 8.0, false);
        break;
    case MonsterType::mushroom:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("mushroom_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("mushroom_hit"), 8.0, false);
        break;
    case MonsterType::pixie:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("pixie_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("pixie_hit"), 8.0, false);
        break;
    case MonsterType::mummy:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("mummy_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("mummy_hit"), 8.0, false);
        break;
    case MonsterType::imp:
        m_animations[MonsterState::Walking] = new Animation(SpriteManager::instance().getAnimationSequence("imp_walk"), 8.0, true);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("imp_hit"), 8.0, false);
        break;
    default:
        break;
    }
    m_currentAnimation = m_animations.value(m_currentState, nullptr);
}

MonsterEntity::~MonsterEntity() {
    qDeleteAll(m_animations);
}

void MonsterEntity::setVelocity(const QPointF& velocity) {
    m_velocity = velocity;
}

void MonsterEntity::deploy() {
    if (monsterType == MonsterType::spikeball && m_currentState != MonsterState::Deployed && m_currentState != MonsterState::Hit) {
        delete m_animations[MonsterState::Hit];
        m_animations[MonsterState::Deployed] = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_deploy"), 8.0, false);
        m_animations[MonsterState::Hit] = new Animation(SpriteManager::instance().getAnimationSequence("spikeball_deploy_hit"), 8.0, false);
        setState(MonsterState::Deployed);
    }
}
void MonsterEntity::onHit() {
    setState(MonsterState::Hit);
    m_hitTimer = 0.10;
}

void MonsterEntity::setState(MonsterState newState) {
    if (m_currentState != newState) {
            m_currentState = newState;
            m_currentAnimation = m_animations.value(m_currentState, nullptr);
    }
}

void MonsterEntity::update(double deltaTime) {
    // if (shouldBeRemoved()) return;
    if (m_currentAnimation) {
        m_currentAnimation->update(deltaTime);
    }
    switch (m_currentState) {
        case MonsterState::Hit:
            m_hitTimer -= deltaTime;
            if (m_hitTimer <= 0) {
                m_hitTimer = 0;
                if (m_animations[MonsterState::Deployed] == nullptr) {
                    setState(MonsterState::Walking);
                } else {
                    setState(MonsterState::Deployed);
                }
            }
        case MonsterState::Walking:
            m_position += m_velocity * deltaTime;
            break;
        case MonsterState::Deployed:
            break;
        default:
            break;
    }
}

void MonsterEntity::paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) {
    if (!m_currentAnimation) return;
    const QString& currentFrameName = m_currentAnimation->getCurrentFrameName();
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
    : Entity(parent), m_itemType(itemtype), m_currentState(ItemState::Drop), m_lingerTimer(15.0) {
    setPosition(pos);
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

QString ItemEntity::typeToString(int type) {
    QString stringtype;
    switch (type) {
    case 0: // coin
        stringtype = "coin";
        break;
    case 1: // five_coins
        stringtype = "five_coins";
        break;
    case 2: // extra_life
        stringtype = "extra_life";
        break;
    case 3: // coffee
        stringtype = "coffee";
        break;
    case 4: // machine_gun
        stringtype = "machine_gun";
        break;
    case 5: // bomb
        stringtype = "bomb";
        break;
    case 6: // shotgun
        stringtype = "shotgun";
        break;
    case 7: // smoke_bomb
        stringtype = "smoke_bomb";
        break;
    case 8: // tombstone
        stringtype = "tombstone";
        break;
    case 9: // wheel
        stringtype = "wheel";
        break;
    case 10: // badge
        stringtype = "badge";
        break;
    // 供应商道具类型映射
    case 11: // vendor_boots_1
        stringtype = "boots_1";  // 靴子1
        break;
    case 12: // vendor_boots_2
        stringtype = "boots_2";  // 靴子2
        break;
    case 13: // vendor_extra_life
        stringtype = "more_live";  // 额外生命
        break;
    case 14: // vendor_gun_1
        stringtype = "gun_1";  // 枪1
        break;
    case 15: // vendor_gun_2
        stringtype = "gun_2";  // 枪2
        break;
    case 16: // vendor_gun_3
        stringtype = "gun_3";  // 枪3
        break;
    case 17: // vendor_ammo_1
        stringtype = "bullet_1";  // 弹药1
        break;
    case 18: // vendor_ammo_2
        stringtype = "bullet_2";  // 弹药2
        break;
    case 19: // vendor_ammo_3
        stringtype = "bullet_3";  // 弹药3
        break;
    case 20: // vendor_badge
        stringtype = "badge";  // 治安官徽章
        break;
    default:
        stringtype = "unknown";
        break;
    }
    return stringtype;
}

VendorEntity::VendorEntity(QObject *parent): Entity(parent) {
    m_currentState = VendorState::Disappearing;
    m_animations[VendorState::LookDown] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_look_down"), 8.0, true);
    m_animations[VendorState::Come] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_walk"), 8.0, true);
    m_animations[VendorState::Leave] = new Animation(SpriteManager::instance().getAnimationSequence("vendor_walk"), 8.0, true);
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
    if (m_currentState == VendorState::Come) {
        // qDebug() << "VendorEntity::update: Walking";
        m_lingerTimer -= deltaTime;
        m_position += QPointF(0, 16 * deltaTime);
    } else if (m_currentState == VendorState::Leave) {
        // qDebug() << "VendorEntity::update: Leaving";
        m_lingerTimer -= deltaTime;
        m_position += QPointF(0, -16 * deltaTime);
        if (m_lingerTimer <= 0) {
            setState(VendorState::Disappearing);
            return;
        }
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

    if (m_currentState != VendorState::Come && m_currentState != VendorState::Singing && m_currentState != VendorState::Leave) {
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
            QString itemName = ItemEntity::typeToString(itemType);
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
    setState(VendorState::Leave);
    m_lingerTimer = 6.0;
}

void VendorEntity::onVendorAppear() {
    setState(VendorState::Come);
    m_lingerTimer = 6.0;
    m_position = QPointF(16*8, 0);
}

void VendorEntity::onGameWin() {
    setState(VendorState::Singing);
    m_lingerTimer = 20;
    setPosition(QPointF(16*10,16*8));
}
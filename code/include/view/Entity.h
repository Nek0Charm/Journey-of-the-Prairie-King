#ifndef ENTITY_H
#define ENTITY_H
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
    ShootRightWalk,
    Dying,
    Zombie
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

enum class MonsterType {
    orc,
    spikeball,
    ogre,
    mushroom,
    pixie,
    mummy,
    imp
};
class MonsterEntity : public Entity {
    Q_OBJECT
public:
    explicit MonsterEntity(const MonsterType& monsterType, QObject* parent = nullptr);
    ~MonsterEntity() override;
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(MonsterState newState) { m_currentState = newState; }
    void setVelocity(const QPointF& velocity);
    void setFrozen(bool frozen) { m_isFrozen = frozen; }
    bool isFrozen() const { return m_isFrozen; }
    MonsterType getType() const {return monsterType;}
private:
    Animation* m_animation;    
    QPointF m_velocity; 
    MonsterType  monsterType;
    MonsterState m_currentState;
    bool m_isFrozen = false; 
};

enum class DeadMonsterState {
    Dying, 
    Dead   
};

class DeadMonsterEntity : public Entity {
    Q_OBJECT
public:
    explicit DeadMonsterEntity(const MonsterEntity& monserentity);
    ~DeadMonsterEntity() override;
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(DeadMonsterState newState) { m_currentState = newState; }
    bool ShouldbeRemove() { return m_lingerTimer <= 0; }
private:
    Animation* m_animation;
    MonsterType monsterType;
    DeadMonsterState m_currentState;
    double m_lingerTimer;
};

enum class ItemState {
    Drop, 
    Flash,
    Picked   
};

// 使用与ItemEffectManager相同的枚举定义
enum class ItemType {
        coin,
        five_coins,
        extra_life,
        coffee,
        machine_gun,
        bomb,
        shotgun,
        smoke_bomb,
        tombstone,
        wheel,
        badge,
        
        // 供应商升级道具类型
        // 卡槽1：靴子系列
        vendor_boots_1,      // 靴子1 - 8金币
        vendor_boots_2,      // 靴子2 - 20金币
        vendor_extra_life,   // 额外生命 - 10金币
        
        // 卡槽2：枪系列
        vendor_gun_1,        // 枪1 - 10金币
        vendor_gun_2,        // 枪2 - 20金币
        vendor_gun_3,        // 枪3 - 30金币
        
        // 卡槽3：弹药系列
        vendor_ammo_1,       // 弹药1 - 15金币
        vendor_ammo_2,       // 弹药2 - 30金币
        vendor_ammo_3,       // 弹药3 - 45金币
        
        // 通用
        vendor_badge         // 治安官徽章 - 10金币
};
class ItemEntity : public Entity {
    Q_OBJECT
public:
    explicit ItemEntity(int itemtype, QObject* parent = nullptr, QPointF position = QPointF(-25, 0));
    ~ItemEntity() { };
    void update(double deltaTime) override;
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(ItemState newState) { m_currentState = newState; }
    ItemState getState() { return m_currentState; }
    bool ShouldbeRemove() { return m_lingerTimer <= 0 ; }
    bool isVisible();
    static QString typeToString(ItemType type);
private:
    ItemType m_itemType;
    ItemState m_currentState;
    double m_lingerTimer;
};

enum class VendorState {
    Walk,
    LookDown,
    LookLeft,
    LookRight,
    Disappearing,
};
 
class VendorEntity : public Entity {
    Q_OBJECT
public:
    explicit VendorEntity(QObject* parent = nullptr);
    ~VendorEntity() override;
    void update(double deltaTime, const QPointF& playerPosition);
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset) override;
    void setState(VendorState newState) { m_currentState = newState; }
    VendorState getState() const { return m_currentState; }
    
    // 设置可购买的物品列表
    void setAvailableItems(const QList<int>& items) { m_availableItems = items; }
    QList<int> getAvailableItems() const { return m_availableItems; }
    
public slots:
    void onVendorAppear();
    void onVendorDisappear();
private:
    QMap<VendorState, Animation*> m_animations;
    VendorState m_currentState;
    Animation* m_currentAnimation;
    double m_lingerTimer = 6.0;
    QList<int> m_availableItems = {};  // 默认空列表，会被动态更新
};

#endif // ENTITY_H
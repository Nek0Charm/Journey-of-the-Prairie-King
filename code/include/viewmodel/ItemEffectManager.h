#ifndef ITEMEFFECTMANAGER_H
#define ITEMEFFECTMANAGER_H

#include <QVector>

// 前向声明
class PlayerViewModel;
class EnemyManager;

class ItemEffectManager : public QObject {
    Q_OBJECT

public:
    // 效果类型枚举
    enum EffectType {
        MOVE_SPEED_BOOST,    // 移动速度提升
        SHOOT_SPEED_BOOST,   // 射击速度提升
        WHEEL_MODE,          // 8方向射击模式
        SHOTGUN_MODE,        // 霰弹枪模式
        BADGE_MODE,          // 治安官徽章模式（包含霰弹枪效果）
        ZOMBIE_MODE,         // 僵尸模式
        STEALTH_MODE         // 潜行模式
    };
    
    // 效果结构体
    struct ItemEffect {
        EffectType type;
        double endTime;      // 效果结束时间
        double originalValue; // 原始值
        double effectValue;   // 效果值
        bool isActive;        // 是否激活
        
        ItemEffect() : type(MOVE_SPEED_BOOST), endTime(0.0), originalValue(0.0), effectValue(0.0), isActive(false) {}
        ItemEffect(EffectType t, double end, double original, double effect)
            : type(t), endTime(end), originalValue(original), effectValue(effect), isActive(true) {}
    };

    explicit ItemEffectManager(QObject *parent = nullptr);
    
    // 道具类型枚举
    enum ItemType {
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
    
    // 应用道具效果
    void applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager);
    void applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    
    // 获取道具名称
    static QString getItemName(int itemType);
    
    // 获取道具描述
    static QString getItemDescription(int itemType);
    
    // 效果管理方法
    void updateEffects(double deltaTime, PlayerViewModel* player);
    void addEffect(EffectType type, double duration, double originalValue, double effectValue);
    void removeEffect(EffectType type);
    bool hasEffect(EffectType type) const;
    double getEffectRemainingTime(EffectType type) const;
    void clearAllEffects(PlayerViewModel* player = nullptr);
    
    // 获取道具效果持续时间
    static double getItemEffectDuration(int itemType);

signals:
    void itemEffectApplied(int itemType, bool isImmediate);
    void effectExpired(EffectType type);
    void usedTombstone();
    void tombstoneFinished();

private:
    // 具体的道具效果实现
    void applyCoinEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyFiveCoinsEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyExtraLifeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyCoffeeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyMachineGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyShotgunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applySmokeBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyTombstoneEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyWheelEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyBadgeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyVendorAmmoEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate);
    
    // 供应商永久效果函数
    void applyVendorBootsEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate);
    void applyVendorGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate);
    void applyVendorBadgeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    
    // 效果管理私有方法
    void applyEffectToPlayer(EffectType type, double effectValue, PlayerViewModel* player);
    void restorePlayerFromEffect(EffectType type, double originalValue, PlayerViewModel* player);
    void checkAndRemoveExpiredEffects(double currentTime, PlayerViewModel* player);
    
private:
    QMap<EffectType, ItemEffect> m_activeEffects;  // 当前激活的效果
    double m_currentTime = 0.0;  // 当前游戏时间
};

#endif // ITEMEFFECTMANAGER_H 
#ifndef ITEMEFFECTMANAGER_H
#define ITEMEFFECTMANAGER_H

#include <QObject>
#include <QDebug>

// 前向声明
class PlayerViewModel;
class EnemyManager;

class ItemEffectManager : public QObject {
    Q_OBJECT

public:
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
        badge
    };
    
    // 应用道具效果
    void applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager);
    void applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    
    // 获取道具名称
    static QString getItemName(int itemType);
    
    // 获取道具描述
    static QString getItemDescription(int itemType);

signals:
    void itemEffectApplied(int itemType, bool isImmediate);

private:
    // 具体的道具效果实现
    void applyCoinEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyFiveCoinsEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyExtraLifeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyCoffeeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyMachineGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyShotgunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
    void applyWheelEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate);
};

#endif // ITEMEFFECTMANAGER_H 
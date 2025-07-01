#include "viewmodel/ItemEffectManager.h"
#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include <QDebug>

ItemEffectManager::ItemEffectManager(QObject *parent)
    : QObject(parent)
{
}

void ItemEffectManager::applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager) {
    applyItemEffect(itemType, player, enemyManager, false);
}

void ItemEffectManager::applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    QString prefix = isImmediate ? "立即使用" : "使用";
    qDebug() << prefix << getItemName(itemType);
    
    switch(itemType) {
        case coin:
            applyCoinEffect(player, enemyManager, isImmediate);
            break;
        case five_coins:
            applyFiveCoinsEffect(player, enemyManager, isImmediate);
            break;
        case extra_life:
            applyExtraLifeEffect(player, enemyManager, isImmediate);
            break;
        case coffee:
            applyCoffeeEffect(player, enemyManager, isImmediate);
            break;
        case machine_gun:
            applyMachineGunEffect(player, enemyManager, isImmediate);
            break;
        case bomb:
            applyBombEffect(player, enemyManager, isImmediate);
            break;
        case shotgun:
            applyShotgunEffect(player, enemyManager, isImmediate);
            break;
        case wheel:
            applyWheelEffect(player, enemyManager, isImmediate);
            break;
        default:
            qDebug() << prefix << "未知道具:" << itemType;
            break;
    }
    
    emit itemEffectApplied(itemType, isImmediate);
}

QString ItemEffectManager::getItemName(int itemType) {
    switch(itemType) {
        case coin: return "金币";
        case five_coins: return "五个金币";
        case extra_life: return "额外生命";
        case coffee: return "咖啡";
        case machine_gun: return "机枪";
        case bomb: return "炸弹";
        case shotgun: return "霰弹枪";
        case smoke_bomb: return "烟雾弹";
        case tombstone: return "墓碑";
        case wheel: return "轮子";
        case badge: return "徽章";
        default: return "未知道具";
    }
}

QString ItemEffectManager::getItemDescription(int itemType) {
    switch(itemType) {
        case coin: return "增加少量分数";
        case five_coins: return "增加大量分数";
        case extra_life: return "增加一条生命";
        case coffee: return "提升移动速度20%";
        case machine_gun: return "大幅提升射击速度";
        case bomb: return "清除所有敌人";
        case shotgun: return "提升射击速度";
        case smoke_bomb: return "释放烟雾";
        case tombstone: return "墓碑效果";
        case wheel: return "大幅提升移动速度";
        case badge: return "徽章效果";
        default: return "未知效果";
    }
}

void ItemEffectManager::applyCoinEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 金币效果：增加分数或金钱
    // TODO: 实现分数系统
    qDebug() << "获得金币，增加分数";
}

void ItemEffectManager::applyFiveCoinsEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 五个金币效果：增加更多分数或金钱
    // TODO: 实现分数系统
    qDebug() << "获得五个金币，增加大量分数";
}

void ItemEffectManager::applyExtraLifeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 额外生命效果：增加一条生命
    player->addLife();
    qDebug() << "获得额外生命";
}

void ItemEffectManager::applyCoffeeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 咖啡效果：提升移动速度20%
    double currentSpeed = player->getMoveSpeed();
    player->setMoveSpeed(currentSpeed * 1.2);
    qDebug() << "咖啡效果：移动速度提升20%，当前速度:" << currentSpeed * 1.2;
}

void ItemEffectManager::applyMachineGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 机枪效果：大幅提升射击速度
    player->setShootCooldown(0.1);
    qDebug() << "机枪效果：射击冷却时间设为0.1秒";
}

void ItemEffectManager::applyBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 炸弹效果：清除所有敌人
    enemyManager->clearAllEnemies();
    qDebug() << "炸弹效果：清除所有敌人";
}

void ItemEffectManager::applyShotgunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 霰弹枪效果：提升射击速度
    player->setShootCooldown(0.15);
    qDebug() << "霰弹枪效果：射击冷却时间设为0.15秒";
}

void ItemEffectManager::applyWheelEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 轮子效果：大幅提升移动速度
    double currentSpeed = player->getMoveSpeed();
    player->setMoveSpeed(currentSpeed * 1.5);
    qDebug() << "轮子效果：移动速度提升50%，当前速度:" << currentSpeed * 1.5;
} 
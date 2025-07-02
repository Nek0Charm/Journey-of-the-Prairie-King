#include "viewmodel/ItemEffectManager.h"
#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include <QDebug>
#include <QMap>

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
        case smoke_bomb:
            applySmokeBombEffect(player, enemyManager, isImmediate);
            break;
        case tombstone:
            applyTombstoneEffect(player, enemyManager, isImmediate);
            break;
        case wheel:
            applyWheelEffect(player, enemyManager, isImmediate);
            break;
        case badge:
            applyBadgeEffect(player, enemyManager, isImmediate);
            break;
        default:
            qDebug() << prefix << "未知道具:" << itemType;
            break;
    }
    
    emit itemEffectApplied(itemType, isImmediate);
}
//转换成中文这样debug信息清楚一点
QString ItemEffectManager::getItemName(int itemType) {
    switch(itemType) {
        case coin: return "1 Coin";
        case five_coins: return "5 Coin";
        case extra_life: return "额外生命";
        case coffee: return "咖啡";
        case machine_gun: return "重机枪";
        case bomb: return "清屏核弹";
        case shotgun: return "霰弹枪";
        case smoke_bomb: return "烟雾弹";
        case tombstone: return "墓碑";
        case wheel: return "轮子";
        case badge: return "治安官徽章";
        default: return "未知道具";
    }
}

QString ItemEffectManager::getItemDescription(int itemType) {
    switch(itemType) {
        case coin: return "增加1个硬币数量";
        case five_coins: return "增加5个硬币数量";
        case extra_life: return "你的总生命值加一";
        case coffee: return "增加你的移动速率";
        case machine_gun: return "大幅增加开火速率";
        case bomb: return "瞬间摧毁所有屏幕上的敌人";
        case shotgun: return "每次射击会朝开火方向射出三枚方向呈锥状分散的子弹";
        case smoke_bomb: return "将你的角色传送至屏幕上随机地点并赋予潜行";
        case tombstone: return "一道闪电击中你的角色，在一定时间内将你转变成僵尸";
        case wheel: return "你可以一次对8个方向射出子弹";
        case badge: return "提高开火速率和移动速率，并使你能像使用霰弹枪那样锥状射击";
        default: return "未知效果";
    }
}

void ItemEffectManager::applyCoinEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 金币效果：收集金币
    player->addCoins(1);
    qDebug() << "收集到金币，当前金币数量:" << player->getCoins();
}

void ItemEffectManager::applyFiveCoinsEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 五个金币效果：收集五个金币
    player->addCoins(5);
    qDebug() << "收集到五个金币，当前金币数量:" << player->getCoins();
}

void ItemEffectManager::applyExtraLifeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 额外生命效果：增加一条生命
    player->addLife();
    qDebug() << "获得额外生命";
}

void ItemEffectManager::applyCoffeeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 咖啡效果：提升移动速度20%，持续10秒
    double originalSpeed = player->getMoveSpeed();
    double newSpeed = originalSpeed * 1.2;
    double duration = getItemEffectDuration(coffee);
    
    addEffect(MOVE_SPEED_BOOST, duration, originalSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
    qDebug() << "咖啡效果：移动速度提升20%，持续" << duration << "秒";
}

void ItemEffectManager::applyMachineGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 机枪效果：大幅提升射击速度，持续8秒
    double originalCooldown = player->getShootCooldown();
    double newCooldown = 0.1;
    double duration = getItemEffectDuration(machine_gun);
    
    addEffect(SHOOT_SPEED_BOOST, duration, originalCooldown, newCooldown);
    applyEffectToPlayer(SHOOT_SPEED_BOOST, newCooldown, player);
    qDebug() << "机枪效果：射击冷却时间设为0.1秒，持续" << duration << "秒";
}

void ItemEffectManager::applyBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 清屏核弹效果：瞬间摧毁所有屏幕上的敌人
    // 被这种方式杀死的敌人不会掉落物品->clear掉整个敌人列表，而道具生成逻辑是根据destroy信号
    enemyManager->clearAllEnemies();
    qDebug() << "清屏核弹效果：瞬间摧毁所有屏幕上的敌人";
}

void ItemEffectManager::applyShotgunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 霰弹枪效果：锥状射击，每次射出三枚子弹，持续10秒
    double originalCooldown = player->getShootCooldown();
    double newCooldown = 0.15; // 稍微降低开火速率
    double duration = getItemEffectDuration(shotgun);
    
    // 添加射击速度效果
    addEffect(SHOOT_SPEED_BOOST, duration, originalCooldown, newCooldown);
    applyEffectToPlayer(SHOOT_SPEED_BOOST, newCooldown, player);
    
    // 添加霰弹枪模式效果
    addEffect(SHOTGUN_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(SHOTGUN_MODE, 1.0, player);
    
    qDebug() << "霰弹枪效果：激活锥状射击模式，每次射出三枚子弹，持续" << duration << "秒";
}

void ItemEffectManager::applySmokeBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 烟雾弹效果：传送至随机地点并赋予潜行，持续5秒
    double duration = getItemEffectDuration(smoke_bomb);
    
    // 传送至随机位置
    player->teleportToRandomPosition();
    
    // 添加潜行模式效果
    addEffect(STEALTH_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(STEALTH_MODE, 1.0, player);
    
    qDebug() << "烟雾弹效果：传送至随机地点并赋予潜行，持续" << duration << "秒";
}

void ItemEffectManager::applyTombstoneEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 墓碑效果：转变成僵尸，获得接触击杀能力，持续8秒
    double originalSpeed = player->getMoveSpeed();
    double newSpeed = originalSpeed * 1.3; // 僵尸模式移动速度提升30%
    double duration = getItemEffectDuration(tombstone);
    
    // 添加移动速度效果
    addEffect(MOVE_SPEED_BOOST, duration, originalSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
    
    // 添加僵尸模式效果
    addEffect(ZOMBIE_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(ZOMBIE_MODE, 1.0, player);
    
    qDebug() << "墓碑效果：转变成僵尸，移动速度提升30%，获得接触击杀能力，持续" << duration << "秒";
}

void ItemEffectManager::applyWheelEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 轮子效果：8方向射击，持续12秒
    double duration = getItemEffectDuration(wheel);
    
    addEffect(WHEEL_MODE, duration, 0.0, 1.0);  // 使用0.0和1.0作为占位符
    applyEffectToPlayer(WHEEL_MODE, 1.0, player);
    qDebug() << "轮子效果：激活8方向射击模式，持续" << duration << "秒";
}

void ItemEffectManager::applyBadgeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 治安官徽章效果：提高开火速率和移动速率，持续15秒
    double originalSpeed = player->getMoveSpeed();
    double originalCooldown = player->getShootCooldown();
    double newSpeed = originalSpeed * 1.15;
    double newCooldown = 0.12;
    double duration = getItemEffectDuration(badge);
    
    // 添加移动速度效果
    addEffect(MOVE_SPEED_BOOST, duration, originalSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
    
    // 添加射击速度效果
    addEffect(SHOOT_SPEED_BOOST, duration, originalCooldown, newCooldown);
    applyEffectToPlayer(SHOOT_SPEED_BOOST, newCooldown, player);
    
    qDebug() << "治安官徽章效果：提高开火速率和移动速率，持续" << duration << "秒";
}

// 效果管理方法实现
void ItemEffectManager::updateEffects(double deltaTime, PlayerViewModel* player) {
    m_currentTime += deltaTime;
    checkAndRemoveExpiredEffects(m_currentTime, player);
}

void ItemEffectManager::addEffect(EffectType type, double duration, double originalValue, double effectValue) {
    double endTime = m_currentTime + duration;
    ItemEffect effect(type, endTime, originalValue, effectValue);
    m_activeEffects[type] = effect;
    qDebug() << "添加效果:" << type << "持续时间:" << duration << "秒";
}

void ItemEffectManager::removeEffect(EffectType type) {
    if (m_activeEffects.contains(type)) {
        m_activeEffects.remove(type);
        qDebug() << "移除效果:" << type;
    }
}

bool ItemEffectManager::hasEffect(EffectType type) const {
    return m_activeEffects.contains(type) && m_activeEffects[type].isActive;
}

double ItemEffectManager::getEffectRemainingTime(EffectType type) const {
    if (hasEffect(type)) {
        return m_activeEffects[type].endTime - m_currentTime;
    }
    return 0.0;
}

double ItemEffectManager::getItemEffectDuration(int itemType) {
    switch(itemType) {
        case coffee: return 10.0;        // 咖啡效果持续10秒
        case machine_gun: return 8.0;    // 机枪效果持续8秒
        case wheel: return 12.0;         // 轮子效果持续12秒
        case shotgun: return 10.0;       // 霰弹枪效果持续10秒
        case badge: return 15.0;         // 治安官徽章效果持续15秒
        case smoke_bomb: return 5.0;     // 烟雾弹效果持续5秒
        case tombstone: return 8.0;      // 墓碑效果持续8秒
        default: return 0.0;             // 其他道具无持续时间
    }
}

void ItemEffectManager::applyEffectToPlayer(EffectType type, double effectValue, PlayerViewModel* player) {
    switch(type) {
        case MOVE_SPEED_BOOST:
            player->setMoveSpeed(effectValue);
            break;
        case SHOOT_SPEED_BOOST:
            player->setShootCooldown(effectValue);
            break;
        case WHEEL_MODE:
            player->setWheelMode(true);
            break;
        case SHOTGUN_MODE:
            player->setShotgunMode(true);
            break;
        case ZOMBIE_MODE:
            player->setZombieMode(true);
            break;
        case STEALTH_MODE:
            player->setStealthMode(true);
            break;
    }
}

void ItemEffectManager::restorePlayerFromEffect(EffectType type, double originalValue, PlayerViewModel* player) {
    switch(type) {
        case MOVE_SPEED_BOOST:
            player->setMoveSpeed(originalValue);
            qDebug() << "移动速度恢复到:" << originalValue;
            break;
        case SHOOT_SPEED_BOOST:
            player->setShootCooldown(originalValue);
            qDebug() << "射击冷却恢复到:" << originalValue;
            break;
        case WHEEL_MODE:
            player->setWheelMode(false);
            qDebug() << "8方向射击模式关闭";
            break;
        case SHOTGUN_MODE:
            player->setShotgunMode(false);
            qDebug() << "霰弹枪模式关闭";
            break;
        case ZOMBIE_MODE:
            player->setZombieMode(false);
            qDebug() << "僵尸模式关闭";
            break;
        case STEALTH_MODE:
            player->setStealthMode(false);
            qDebug() << "潜行模式关闭";
            break;
    }
}

void ItemEffectManager::checkAndRemoveExpiredEffects(double currentTime, PlayerViewModel* player) {
    QVector<EffectType> expiredEffects;
    
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end(); ++it) {
        if (it.value().isActive && currentTime >= it.value().endTime) {
            expiredEffects.append(it.key());
        }
    }
    
    for (EffectType type : expiredEffects) {
        ItemEffect& effect = m_activeEffects[type];
        effect.isActive = false;
        restorePlayerFromEffect(type, effect.originalValue, player);
        emit effectExpired(type);
        qDebug() << "效果过期:" << type;
    }
    
    // 清理过期的效果
    for (EffectType type : expiredEffects) {
        m_activeEffects.remove(type);
    }
}

void ItemEffectManager::clearAllEffects(PlayerViewModel* player) {
    // 恢复所有效果到原始状态
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end(); ++it) {
        if (player) {
            restorePlayerFromEffect(it.key(), it.value().originalValue, player);
        }
    }
    
    // 清空所有效果
    m_activeEffects.clear();
    m_currentTime = 0.0;
    
    qDebug() << "清除所有道具效果";
}
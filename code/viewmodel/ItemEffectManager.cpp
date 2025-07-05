#include "viewmodel/ItemEffectManager.h"
#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include <algorithm>

ItemEffectManager::ItemEffectManager(QObject *parent)
    : QObject(parent)
{
}

void ItemEffectManager::applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager) {
    applyItemEffect(itemType, player, enemyManager, false);
}

void ItemEffectManager::applyItemEffect(int itemType, PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    QString prefix = isImmediate ? "立即使用" : "使用";
    
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
            
        // 供应商道具类型 - 永久效果
        case vendor_boots_1:
        case vendor_boots_2:
            applyVendorBootsEffect(player, enemyManager, itemType, isImmediate);
            break;
        case vendor_extra_life:
            applyExtraLifeEffect(player, enemyManager, isImmediate);
            break;
        case vendor_gun_1:
        case vendor_gun_2:
        case vendor_gun_3:
            applyVendorGunEffect(player, enemyManager, itemType, isImmediate);
            break;
        case vendor_ammo_1:
        case vendor_ammo_2:
        case vendor_ammo_3:
            applyVendorAmmoEffect(player, enemyManager, itemType, isImmediate);
            break;
        case vendor_badge:
            applyVendorBadgeEffect(player, enemyManager, isImmediate);
            break;
            
        default:
            break;
    }
    
    emit itemEffectApplied(itemType, isImmediate);
}

void ItemEffectManager::applyCoinEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 金币效果：收集金币
    player->addCoins(1);
}

void ItemEffectManager::applyFiveCoinsEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 五个金币效果：收集五个金币
    player->addCoins(5);
}

void ItemEffectManager::applyExtraLifeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 额外生命效果：增加一条生命
    player->addLife();
}

void ItemEffectManager::applyCoffeeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 咖啡效果：提升移动速度20%，持续10秒
    // 基于基础速度计算，避免无限叠加
    double baseSpeed = 100.0; // 基础移动速度
    double currentSpeed = player->getMoveSpeed();
    double newSpeed = std::max(currentSpeed, baseSpeed * 1.2); // 取当前速度和基础速度120%的较大值
    double duration = getItemEffectDuration(coffee);
    
    addEffect(MOVE_SPEED_BOOST, duration, currentSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
}

void ItemEffectManager::applyMachineGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 机枪效果：大幅提升射击速度，持续8秒
    double originalCooldown = player->getShootCooldown();
    double newCooldown = 0.1;
    double duration = getItemEffectDuration(machine_gun);
    
    addEffect(SHOOT_SPEED_BOOST, duration, originalCooldown, newCooldown);
    applyEffectToPlayer(SHOOT_SPEED_BOOST, newCooldown, player);
}

void ItemEffectManager::applyBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 清屏核弹效果：瞬间摧毁所有屏幕上的敌人
    // 被这种方式杀死的敌人不会掉落物品->clear掉整个敌人列表，而道具生成逻辑是根据destroy信号
    enemyManager->clearAllEnemies();
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
}

void ItemEffectManager::applySmokeBombEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 烟雾弹效果：传送至随机地点并赋予潜行，持续5秒
    double duration = getItemEffectDuration(smoke_bomb);
    
    // 传送至随机位置
    player->teleportToRandomPosition();
    
    // 添加潜行模式效果
    addEffect(STEALTH_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(STEALTH_MODE, 1.0, player);
}

void ItemEffectManager::applyTombstoneEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 墓碑效果：转变成僵尸，获得接触击杀能力，持续8秒
    // 基于基础速度计算，避免无限叠加
    double baseSpeed = 100.0; // 基础移动速度
    double currentSpeed = player->getMoveSpeed();
    double newSpeed = std::max(currentSpeed, baseSpeed * 1.3); // 取当前速度和基础速度130%的较大值
    double duration = getItemEffectDuration(tombstone);
    
    // 添加移动速度效果
    addEffect(MOVE_SPEED_BOOST, duration, currentSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
    
    // 添加僵尸模式效果
    addEffect(ZOMBIE_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(ZOMBIE_MODE, 1.0, player);
    emit usedTombstone(); // 发出墓碑使用信号
}

void ItemEffectManager::applyWheelEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 轮子效果：8方向射击，持续12秒
    double duration = getItemEffectDuration(wheel);
    
    addEffect(WHEEL_MODE, duration, 0.0, 1.0);  // 使用0.0和1.0作为占位符
    applyEffectToPlayer(WHEEL_MODE, 1.0, player);
}

void ItemEffectManager::applyBadgeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 治安官徽章效果：提高开火速率和移动速率，并使你能像使用霰弹枪那样锥状射击，持续15秒
    // 基于基础速度计算，避免无限叠加
    double baseSpeed = 100.0; // 基础移动速度
    double currentSpeed = player->getMoveSpeed();
    double originalCooldown = player->getShootCooldown();
    double newSpeed = std::max(currentSpeed, baseSpeed * 1.15); // 取当前速度和基础速度115%的较大值
    double newCooldown = 0.12;
    double duration = getItemEffectDuration(badge);
    
    // 添加移动速度效果
    addEffect(MOVE_SPEED_BOOST, duration, currentSpeed, newSpeed);
    applyEffectToPlayer(MOVE_SPEED_BOOST, newSpeed, player);
    
    // 添加射击速度效果
    addEffect(SHOOT_SPEED_BOOST, duration, originalCooldown, newCooldown);
    applyEffectToPlayer(SHOOT_SPEED_BOOST, newCooldown, player);

    addEffect(SHOTGUN_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(SHOTGUN_MODE, 1.0, player);
    
    // 添加治安官徽章模式效果（包含霰弹枪功能）
    addEffect(BADGE_MODE, duration, 0.0, 1.0);
    applyEffectToPlayer(BADGE_MODE, 1.0, player);
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
}

void ItemEffectManager::removeEffect(EffectType type) {
    if (m_activeEffects.contains(type)) {
        m_activeEffects.remove(type);
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
        case BADGE_MODE:
            player->setBadgeMode(true);
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
            break;
        case SHOOT_SPEED_BOOST:
            player->setShootCooldown(originalValue);
            break;
        case WHEEL_MODE:
            player->setWheelMode(false);
            break;
        case SHOTGUN_MODE:
            player->setShotgunMode(false);
            break;
        case BADGE_MODE:
            player->setBadgeMode(false);
            break;
        case ZOMBIE_MODE:
            player->setZombieMode(false);
            break;
        case STEALTH_MODE:
            player->setStealthMode(false);
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
        if (type) {
            emit tombstoneFinished(); // 如果是墓碑效果，发出墓碑完成信号
        }
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

void ItemEffectManager::applyVendorAmmoEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate) {
    // 供应商弹药效果：永久提升子弹伤害值
    int damageValue = 1; // 默认伤害值
    
    switch(itemType) {
        case vendor_ammo_1:
            damageValue = 2;
            break;
        case vendor_ammo_2:
            damageValue = 3;
            break;
        case vendor_ammo_3:
            damageValue = 4;
            break;
    }
    
    // 直接设置玩家的子弹伤害值，这是永久效果
    player->setBulletDamage(damageValue);
    qDebug() << "供应商弹药效果：子弹伤害值永久提升至" << damageValue;
}

void ItemEffectManager::applyVendorBootsEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate) {
    // 供应商靴子效果：永久提升移动速度
    double baseSpeed = 100.0; // 基础移动速度
    double speedMultiplier = 1.0;
    
    switch(itemType) {
        case vendor_boots_1:
            speedMultiplier = 1.2; // 提升20%
            break;
        case vendor_boots_2:
            speedMultiplier = 1.4; // 提升40%
            break;
    }
    
    double newSpeed = baseSpeed * speedMultiplier;
    player->setMoveSpeed(newSpeed);
    qDebug() << "供应商靴子效果：移动速度永久提升至" << newSpeed;
}

void ItemEffectManager::applyVendorGunEffect(PlayerViewModel* player, EnemyManager* enemyManager, int itemType, bool isImmediate) {
    // 供应商枪效果：永久提升射击速度
    double newCooldown = 0.5; // 默认射击冷却时间
    
    switch(itemType) {
        case vendor_gun_1:
            newCooldown = 0.3; // 提升射击速度
            break;
        case vendor_gun_2:
            newCooldown = 0.2; // 进一步提升射击速度
            break;
        case vendor_gun_3:
            newCooldown = 0.1; // 最高射击速度
            break;
    }
    
    player->setShootCooldown(newCooldown);
    qDebug() << "供应商枪效果：射击冷却时间永久设为" << newCooldown << "秒";
}

void ItemEffectManager::applyVendorBadgeEffect(PlayerViewModel* player, EnemyManager* enemyManager, bool isImmediate) {
    // 供应商治安官徽章效果：永久获得治安官徽章效果
    // 这个效果包含多个方面的提升
    double baseSpeed = 100.0;
    double baseCooldown = 0.5;
    
    // 永久提升移动速度
    player->setMoveSpeed(baseSpeed * 1.3); // 提升30%
    
    // 永久提升射击速度
    player->setShootCooldown(baseCooldown * 0.7); // 提升30%
    
    // 启用霰弹枪模式（永久）
    player->setShotgunMode(true);
    
    qDebug() << "供应商治安官徽章效果：永久获得移动速度+30%、射击速度+30%、霰弹枪模式";
}
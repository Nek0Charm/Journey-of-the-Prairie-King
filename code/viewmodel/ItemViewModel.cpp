#include "viewmodel/ItemViewModel.h"
#include "viewmodel/ItemEffectManager.h"
#include <QDebug>
#include <algorithm>
#include <QRandomGenerator>
#include <QLineF>

ItemViewModel::ItemViewModel(QObject *parent)
    : QObject(parent) {
}

void ItemViewModel::createItem(const QPointF& position, QMap<int, double> itemPossibilities) {
    double randomValue = static_cast<double>(rand()) / RAND_MAX; 
    double cumulativeProbability = 0.0;
    
    for (auto it = itemPossibilities.constBegin(); it != itemPossibilities.constEnd(); ++it) {
        cumulativeProbability += it.value();
        if (randomValue <= cumulativeProbability) {
            createItem(position, it.key());
            return; 
        }
    }

    createItem(position, ItemEffectManager::coin); 
}

void ItemViewModel::createItem(const QPointF& position, int type) {
    QPair<int, int> positionPair(static_cast<int>(position.x()), static_cast<int>(position.y()));
    if(m_itemPositions.find(positionPair) != m_itemPositions.end()) {
        return; 
    }
    ItemData newItem;
    newItem.type = type; 
    newItem.id = m_nextItemId++;
    int px = std::clamp(static_cast<int>(position.x()), 16, 226);
    int py = std::clamp(static_cast<int>(position.y()), 16, 226);
    newItem.position = {px, py};
    newItem.isPossessed = false;
    newItem.isActive = true;
    newItem.remainTime = 15.0; 
    // 道具生成完成
    m_items.append(newItem);
    m_itemPositions[positionPair] = newItem.id;
}

void ItemViewModel::updateItems(double deltaTime, const QPointF& playerPosition) {
    for (auto& item : m_items) {
        if (item.isActive) {
            item.remainTime -= deltaTime;
            if (item.remainTime <= 0.0) {
                item.isActive = false; 
            }
        }
    }
    // 使用更精确的碰撞检测，检查玩家是否在道具附近
    for (auto& item : m_items) {
        if (!item.isActive) continue;
        
        // 计算玩家与道具的距离
        double distance = QLineF(playerPosition, QPointF(item.position.x(), item.position.y())).length();
        
        // 如果距离小于等于8像素（考虑SCALE=5，实际是1.6个游戏单位），则认为拾取
        if (distance <= 8.0) {
            ItemData pickedItem = item;
            item.isActive = false; // 标记道具为已拾取
            
            // 检查是否为需要立即使用的道具类型
            bool shouldUseImmediately = (pickedItem.type == ItemEffectManager::coin || 
                                        pickedItem.type == ItemEffectManager::five_coins ||
                                        pickedItem.type == ItemEffectManager::extra_life ||
                                        pickedItem.type == ItemEffectManager::bomb);
            
            if (shouldUseImmediately) {
                // 需要立即使用的道具，不论道具栏状态都立即使用
                useItemImmediately(pickedItem);
                qDebug() << "道具立即使用:" << pickedItem.type;
            } else if (!m_possessingItem) {
                // 非金币道具且道具栏为空，拾取道具到道具栏
                m_possessingItem = true;
                m_possessedItem = pickedItem;
                m_possessedItem.isPossessed = true;
                emit itemPickedUp(pickedItem.type);
                qDebug() << "道具被拾取到道具栏:" << pickedItem.type;
            } else {
                // 非金币道具且道具栏已有道具，立即使用新道具
                useItemImmediately(pickedItem);
                qDebug() << "道具栏已有道具，立即使用新道具:" << pickedItem.type;
            }
            
            // 从位置映射中移除
            QPair<int, int> positionPair(static_cast<int>(pickedItem.position.x()), static_cast<int>(pickedItem.position.y()));
            m_itemPositions.remove(positionPair);
            break; // 一次只拾取一个道具
        }
    } 
    
    // 清理过期的道具
    m_items.erase(std::remove_if(m_items.begin(), m_items.end(),
                                  [](const ItemData& item) { return !item.isActive; }),
                  m_items.end());

    emit itemsChanged(m_items); // 发出道具列表变化信号
    emit possessedItemChanged(m_possessedItem.type, m_possessingItem); // 发出道具栏变化信号
}

QList<ItemData> ItemViewModel::getActiveItems() const {
    QList<ItemData> activeItems;
    for (const auto& item : m_items) {
        if (item.isActive) {
            activeItems.append(item);
        }
    }
    return activeItems;
}

void ItemViewModel::clearAllItems() {
    m_items.clear();
    m_nextItemId = 0; 
    m_itemPositions.clear();
    m_possessingItem = false;
}

void ItemViewModel::usePossessedItem() {
    if (m_possessingItem) {
        int itemType = m_possessedItem.type;
        m_possessingItem = false;
        emit itemUsed(itemType);
        qDebug() << "使用道具栏中的道具:" << itemType;
    }
}

void ItemViewModel::useItemImmediately(const ItemData& item) {
    // 发出立即使用信号
    emit itemUsedImmediately(item.type);
    qDebug() << "道具立即使用:" << item.type;
}

void ItemViewModel::spawnItemAtPosition(const QPointF& position) {
    // 检查是否应该生成道具
    double randomValue = QRandomGenerator::global()->bounded(1.0);
    if (randomValue > m_spawnProbability) {
        return; // 不生成道具
    }
    
    // 选择随机道具类型
    int itemType = selectRandomItemType();
    
    // 生成道具
    createItem(position, itemType);
    
    // 道具生成完成
    emit itemSpawned(itemType, position);
}

int ItemViewModel::selectRandomItemType() const {
    // 平衡的道具生成概率分布
    int randomValue = QRandomGenerator::global()->bounded(100);
    
    if (randomValue < 25) return ItemEffectManager::coin;          // 25% 金币
    if (randomValue < 40) return ItemEffectManager::five_coins;    // 15% 五金币
    if (randomValue < 50) return ItemEffectManager::extra_life;    // 10% 额外生命
    if (randomValue < 60) return ItemEffectManager::coffee;        // 10% 咖啡
    if (randomValue < 68) return ItemEffectManager::machine_gun;   // 8% 机枪
    if (randomValue < 75) return ItemEffectManager::bomb;          // 7% 清屏核弹
    if (randomValue < 82) return ItemEffectManager::shotgun;       // 7% 霰弹枪
    if (randomValue < 88) return ItemEffectManager::smoke_bomb;    // 6% 烟雾弹
    if (randomValue < 93) return ItemEffectManager::tombstone;     // 5% 墓碑
    if (randomValue < 97) return ItemEffectManager::wheel;         // 4% 轮子
    if (randomValue < 100) return ItemEffectManager::badge;        // 3% 治安官徽章
    
    return ItemEffectManager::coin; // 默认返回金币
}
#include "viewmodel/ItemViewModel.h"
#include <QDebug>

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

    createItem(position, coin); 
}

void ItemViewModel::createItem(const QPointF& position, int type) {
    QPair<int, int> positionPair(static_cast<int>(position.x()), static_cast<int>(position.y()));
    if(m_itemPositions.find(positionPair) != m_itemPositions.end()) {
        return; 
    }
    ItemData newItem;
    newItem.type = type; 
    newItem.id = m_nextItemId++;
    newItem.position = {static_cast<int>(position.x()), static_cast<int>(position.y())};
    newItem.isPossessed = false;
    newItem.isActive = true;
    newItem.remainTime = 15.0; 
    qDebug() << "Item created at position:" << newItem.position << "with type:" << newItem.type;
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
    QPair<int, int> positionPair(static_cast<int>(playerPosition.x()), static_cast<int>(playerPosition.y()));

    // 检查玩家是否踩到道具
    if (m_itemPositions.find(positionPair) != m_itemPositions.end()) {
        int itemIndex = -1;
        for (int i = 0; i < m_items.size(); ++i) {
            if (m_items[i].id == m_itemPositions[positionPair] && m_items[i].isActive) {
                itemIndex = i;
                break;
            }
        }
        
        if (itemIndex != -1) {
            ItemData pickedItem = m_items[itemIndex];
            pickedItem.isActive = false;
            
            if (!m_possessingItem) {
                // 道具栏为空，拾取道具到道具栏
                m_possessingItem = true;
                m_possessedItem = pickedItem;
                m_possessedItem.isPossessed = true;
                emit itemPickedUp(pickedItem.type);
                qDebug() << "道具被拾取到道具栏:" << pickedItem.type;
            } else {
                // 道具栏已有道具，立即使用新道具
                useItemImmediately(pickedItem);
                qDebug() << "道具栏已有道具，立即使用新道具:" << pickedItem.type;
            }
        }
    } 
    
    // 清理过期的道具
    m_items.erase(std::remove_if(m_items.begin(), m_items.end(),
                                  [](const ItemData& item) { return !item.isActive; }),
                  m_items.end());
}

QList<ItemViewModel::ItemData> ItemViewModel::getActiveItems() const {
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

void ItemViewModel::useItemImmediately(const ItemData& item) {
    // 发出立即使用信号
    emit itemUsedImmediately(item.type);
    qDebug() << "道具立即使用:" << item.type;
}
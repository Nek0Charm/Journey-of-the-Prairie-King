#include "viewmodel/ItemViewModel.h"

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

    if(!m_possessingItem && m_itemPositions.find(positionPair) != m_itemPositions.end()) {
        if(m_items[m_itemPositions[positionPair]].isActive) {
            m_possessingItem = true;
            m_possessedItem = m_items[m_itemPositions[positionPair]];
            m_possessedItem.isPossessed = true;
            m_items[m_itemPositions[positionPair]].isActive = false;
        }
    } 
    
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
}
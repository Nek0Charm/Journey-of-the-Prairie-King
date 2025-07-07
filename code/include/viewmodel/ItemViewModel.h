#ifndef __ITEM_VIEW_MODEL_H__
#define __ITEM_VIEW_MODEL_H__

#include <QPoint>

class ItemViewModel : public QObject {

    Q_OBJECT
public:
    

    ItemViewModel(QObject *parent = nullptr);
    
    // 道具数据管理
    void createItem(const QPointF& position, int type);
    void createItem(const QPointF& position, QMap<int, double> itemPossibilities);
    void updateItems(double deltaTime, const QPointF& playerPosition);
    void clearAllItems();
    
    // 道具栏管理
    bool hasPossessedItem() const { return m_possessingItem; }
    int getPossessedItemType() const { return m_possessedItem.type; }
    QList<ItemData> getActiveItems() const;
    
    // 道具使用
    void usePossessedItem();
    
    // 道具生成
    void spawnItemAtPosition(const QPointF& position);
    void setSpawnProbability(double probability) { m_spawnProbability = probability; }
    double getSpawnProbability() const { return m_spawnProbability; }
    
signals:
    void itemPickedUp(int itemType); // 道具拾取信号
    void itemUsedImmediately(int itemType); // 道具立即使用信号
    void itemUsed(int itemType); // 道具使用信号
    void itemSpawned(int itemType, const QPointF& position); // 道具生成信号
    void itemsChanged(const QList<ItemData>& items); // 道具列表变化信号
    void possessedItemChanged(int itemType, bool isPossessed); // 道具栏变化信号

private:
    QList<ItemData> m_items;
    ItemData m_possessedItem;
    int m_nextItemId = 0; 
    bool m_possessingItem = false;
    QMap<QPair<int, int>, int> m_itemPositions;
    double m_spawnProbability = 0.3; // 默认30%概率生成道具
    
    void useItemImmediately(const ItemData& item); // 立即使用道具
    int selectRandomItemType() const; // 选择随机道具类型
};

#endif
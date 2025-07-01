#ifndef __ITEM_VIEW_MODEL_H__
#define __ITEM_VIEW_MODEL_H__

class ItemViewModel : public QObject {

    Q_OBJECT
public:
    enum {
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
    struct ItemData {
        int type;
        int id;
        QPoint position;
        bool isPossessed;
        bool isActive;
        double remainTime;
    };

    ItemViewModel(QObject *parent = nullptr);
    void createItem(const QPointF& position, const QMap<int, double> itemPossibilities);
    void createItem(const QPointF& position, int type);
    void updateItems(double deltaTime, const QPointF& playerPosition);
    void clearAllItems();
    bool hasPossessedItem() const { return m_possessingItem; }
    int getPossessedItemType() const {
        return m_possessedItem.type;
    }

    QList<ItemData> getActiveItems() const ;
    const QMap<int, double> m_itemPossibilities = {
        {coin, 0.3},
        {five_coins, 0.2},
        {extra_life, 0.2},
        {coffee, 0.3}
    };
    private:
    QList<ItemData> m_items;
    ItemData m_possessedItem;
    int m_nextItemId = 0; 
    bool m_possessingItem = false;
    QMap<QPair<int, int>, int> m_itemPositions;
    
};

#endif
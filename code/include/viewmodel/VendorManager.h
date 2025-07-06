#ifndef VENDORMANAGER_H
#define VENDORMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include "viewmodel/ItemEffectManager.h"

// 前向声明
class PlayerViewModel;

class VendorManager : public QObject {
    Q_OBJECT
    
public:
    explicit VendorManager(QObject *parent = nullptr);
    
    // 供应商管理
    void checkVendorAppearance(int currentArea);
    void showVendor();
    void hideVendor();
    bool isVendorActive() const { return m_isActive; }
    
    // 升级物品管理
    QList<int> getAvailableUpgradeItems() const;
    bool canPurchaseItem(int itemType, int playerCoins) const;
    bool purchaseItem(int itemType, PlayerViewModel* player);
    int getItemPrice(int itemType) const;
    
    // 进度管理
    int getSlotProgress(int slotIndex) const;
    void setHardMode(bool isHard) { m_isHardMode = isHard; }
    bool isHardMode() const { return m_isHardMode; }
    
signals:
    void vendorAppeared();
    void vendorDisappeared();
    void itemPurchased(int itemType);
    void vendorItemsChanged(const QList<int>& items);
    
private:
    bool m_isActive = false;
    int m_slotProgress[4] = {0, 0, 0, 0};  // 每个卡槽的购买进度（0-3）
    bool m_isHardMode = false;
    
    // 供应商物品配置
    struct VendorItemConfig {
        int itemType;
        int price;
        int slotIndex;
        int itemIndex;
        bool isHardModeOnly;
        bool isInfinitePurchase;  // 是否可以无限购买（如额外生命、治安官徽章）
        
        VendorItemConfig(int type, int p, int slot, int item, bool hardOnly = false, bool infinite = false)
            : itemType(type), price(p), slotIndex(slot), itemIndex(item), isHardModeOnly(hardOnly), isInfinitePurchase(infinite) {}
    };
    
    QList<VendorItemConfig> m_vendorItems;
    
    void initializeVendorItems();
    void unlockNextItem(int slotIndex);
    int getSlotIndex(int itemType) const;
    bool isItemAvailable(int itemType) const;
    bool isItemAvailableForSlot(const VendorItemConfig& item) const;
};

#endif // VENDORMANAGER_H 
#include "viewmodel/VendorManager.h"
#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/ItemEffectManager.h"
#include <QDebug>

VendorManager::VendorManager(QObject *parent)
    : QObject(parent)
{
    initializeVendorItems();
}

void VendorManager::checkVendorAppearance(int currentArea) {
//获取当前关卡编号
}

void VendorManager::showVendor() {
    if (!m_isActive) {
        m_isActive = true;
        emit vendorAppeared();
        qDebug() << "供应商出现";
    }
}
//出现之后应该有一个动画效果
void VendorManager::hideVendor() {
    if (m_isActive) {
        m_isActive = false;
        emit vendorDisappeared();
        qDebug() << "供应商消失";
    }
}

QList<int> VendorManager::getAvailableUpgradeItems() const {
    QList<int> availableItems;
    QStringList slotInfo;
    
    // 槽位0
    for (const auto& item : m_vendorItems) {
        if (item.slotIndex == 0 && isItemAvailableForSlot(item)) {
            availableItems.append(item.itemType);
            slotInfo << QString("[0]%1(ID:%2,进度:%3)").arg(ItemEffectManager::getItemName(item.itemType)).arg(item.itemType).arg(m_slotProgress[0]);
            break;
        }
    }
    // 槽位1
    for (const auto& item : m_vendorItems) {
        if (item.slotIndex == 1 && isItemAvailableForSlot(item)) {
            availableItems.append(item.itemType);
            slotInfo << QString("[1]%1(ID:%2,进度:%3)").arg(ItemEffectManager::getItemName(item.itemType)).arg(item.itemType).arg(m_slotProgress[1]);
            break;
        }
    }
    // 槽位2
    for (const auto& item : m_vendorItems) {
        if (item.slotIndex == 2 && isItemAvailableForSlot(item)) {
            availableItems.append(item.itemType);
            slotInfo << QString("[2]%1(ID:%2,进度:%3)").arg(ItemEffectManager::getItemName(item.itemType)).arg(item.itemType).arg(m_slotProgress[2]);
            break;
        }
    }
    qDebug() << "[供应商可购买物品]" << slotInfo.join(" | ");
    return availableItems;
}

bool VendorManager::canPurchaseItem(int itemType, int playerCoins) const {
    if (!isItemAvailable(itemType)) {
        return false;
    }
    
    int price = getItemPrice(itemType);
    return playerCoins >= price;
}

bool VendorManager::purchaseItem(int itemType, PlayerViewModel* player) {
    if (!player) return false;
    int price = getItemPrice(itemType);
    if (player->getCoins() >= price) {
        player->addCoins(-price);
        int slotIndex = getSlotIndex(itemType);
        bool isInfiniteItem = false;
        for (const auto& item : m_vendorItems) {
            if (item.itemType == itemType && item.isInfinitePurchase) {
                isInfiniteItem = true;
                break;
            }
        }
        if (!isInfiniteItem && slotIndex >= 0 && slotIndex < 4) {
            m_slotProgress[slotIndex]++;
            unlockNextItem(slotIndex);
        }
        emit itemPurchased(itemType);
        qDebug() << QString("[购买成功] 槽位%1 物品:%2(ID:%3) 价格:%4 进度:%5").arg(slotIndex).arg(ItemEffectManager::getItemName(itemType)).arg(itemType).arg(price).arg(m_slotProgress[slotIndex]);
        // 购买成功后，更新物品列表，但不隐藏供应商
        emit vendorItemsChanged(getAvailableUpgradeItems());
        return true;
    }
    qDebug() << QString("[购买失败] 金币不足 物品:%1(ID:%2) 价格:%3 当前金币:%4").arg(ItemEffectManager::getItemName(itemType)).arg(itemType).arg(price).arg(player->getCoins());
    return false;
}

int VendorManager::getSlotProgress(int slotIndex) const {
    if (slotIndex >= 0 && slotIndex < 4) {
        return m_slotProgress[slotIndex];
    }
    return 0;
}

void VendorManager::initializeVendorItems() {
    m_vendorItems.clear();
    // 重置所有槽位进度
    for (int i = 0; i < 4; i++) m_slotProgress[i] = 0;
    // 槽位0：靴子系列 + 额外生命
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_boots_1, 2, 0, 0));   // 靴子1 - 2金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_boots_2, 5, 0, 1));   // 靴子2 - 5金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_extra_life, 3, 0, 2, false, true)); // 额外生命 - 3金币，无限购买
    // 槽位1：枪系列 + 徽章
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_1, 3, 1, 0));    // 枪1 - 3金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_2, 6, 1, 1));    // 枪2 - 6金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_3, 10, 1, 2));   // 枪3 - 10金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_badge, 3, 1, 3, false, true)); // 治安官徽章 - 3金币，无限购买
    // 槽位2：弹药系列 + 徽章
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_1, 5, 2, 0));   // 弹药1 - 5金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_2, 8, 2, 1));   // 弹药2 - 8金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_3, 12, 2, 2));  // 弹药3 - 12金币
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_badge, 3, 2, 3, false, true)); // 治安官徽章 - 3金币，无限购买
    qDebug() << "[供应商初始化] 槽位进度: [0]" << m_slotProgress[0] << "[1]" << m_slotProgress[1] << "[2]" << m_slotProgress[2];
}

void VendorManager::unlockNextItem(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= 4) {
        return;
    }
    
    int currentProgress = m_slotProgress[slotIndex];
    
    // 根据进度解锁下一个物品
    for (const auto& item : m_vendorItems) {
        if (item.slotIndex == slotIndex && item.itemIndex == currentProgress) {
            // 发出物品列表变化信号，通知UI更新
            emit vendorItemsChanged(getAvailableUpgradeItems());
            break;
        }
    }
}

int VendorManager::getItemPrice(int itemType) const {
    for (const auto& item : m_vendorItems) {
        if (item.itemType == itemType) {
            return item.price;
        }
    }
    return 0;
}

int VendorManager::getSlotIndex(int itemType) const {
    for (const auto& item : m_vendorItems) {
        if (item.itemType == itemType) {
            return item.slotIndex;
        }
    }
    return -1;
}

bool VendorManager::isItemAvailable(int itemType) const {
    for (const auto& item : m_vendorItems) {
        if (item.itemType == itemType) {
            // 检查是否为困难模式专用物品
            if (item.isHardModeOnly && !m_isHardMode) {
                return false;
            }
            
            // 检查购买进度
            int slotIndex = item.slotIndex;
            int itemIndex = item.itemIndex;
            
            // 特殊处理：无限购买物品（额外生命、治安官徽章）
            if (item.isInfinitePurchase) {
                // 如果已经到达该槽位的最后一个物品，则始终可用
                int currentProgress = m_slotProgress[slotIndex];
                // 对于无限购买物品，只要进度达到或超过其itemIndex，就始终可用
                return currentProgress >= itemIndex;
            }
            
            // 对于其他槽位，检查当前进度
            if (slotIndex >= 0 && slotIndex < 4) {
                int currentProgress = m_slotProgress[slotIndex];
                return itemIndex == currentProgress;
            }
        }
    }
    return false;
}

bool VendorManager::isItemAvailableForSlot(const VendorItemConfig& item) const {
    // 检查是否为困难模式专用物品
    if (item.isHardModeOnly && !m_isHardMode) {
        return false;
    }
    
    // 检查购买进度
    int slotIndex = item.slotIndex;
    int itemIndex = item.itemIndex;
    
    // 特殊处理：无限购买物品（额外生命、治安官徽章）
    if (item.isInfinitePurchase) {
        // 如果已经到达该槽位的最后一个物品，则始终可用
        int currentProgress = m_slotProgress[slotIndex];
        // 对于无限购买物品，只要进度达到或超过其itemIndex，就始终可用
        return currentProgress >= itemIndex;
    }
    
    // 对于其他槽位，检查当前进度
    if (slotIndex >= 0 && slotIndex < 4) {
        int currentProgress = m_slotProgress[slotIndex];
        return itemIndex == currentProgress;
    }
    
    return false;
} 
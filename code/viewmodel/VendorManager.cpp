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

bool VendorManager::shouldShowVendor(int area1, int area2) const {
    // 供应商出现的区域: 1-2、1-4、2-1、2-3、3-1和3-3
    QList<QPair<int, int>> vendorAreas = {
        {1, 2}, {1, 4}, {2, 1}, {2, 3}, {3, 1}, {3, 3}
    };
    
    return vendorAreas.contains({area1, area2});
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
    
    for (const auto& item : m_vendorItems) {
        if (isItemAvailable(item.itemType)) {
            availableItems.append(item.itemType);
        }
    }
    
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
    if (!player) {
        return false;
    }
    
    int price = getItemPrice(itemType);
    
    if (player->getCoins() >= price) {
        // 扣除金币
        player->addCoins(-price);
        
        
        // 更新进度
        int slotIndex = getSlotIndex(itemType);
        m_slotProgress[slotIndex]++;
        unlockNextItem(slotIndex);
        
        emit itemPurchased(itemType);
        qDebug() << "购买供应商道具:" << ItemEffectManager::getItemName(itemType) << "价格:" << price;
        return true;
    }
    
    qDebug() << "金币不足，无法购买道具:" << ItemEffectManager::getItemName(itemType);
    return false;
}

int VendorManager::getSlotProgress(int slotIndex) const {
    if (slotIndex >= 0 && slotIndex < 3) {
        return m_slotProgress[slotIndex];
    }
    return 0;
}

void VendorManager::initializeVendorItems() {
    m_vendorItems.clear();
    
    // 卡槽1：靴子系列
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_boots_1, 8, 0, 0));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_boots_2, 20, 0, 1));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_extra_life, 10, 0, 2));
    
    // 卡槽2：枪系列
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_1, 10, 1, 0));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_2, 20, 1, 1));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_3, 30, 1, 2));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_gun_4, 99, 1, 3, true)); // 困难模式
    
    // 卡槽3：弹药系列
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_1, 15, 2, 0));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_2, 30, 2, 1));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_ammo_3, 45, 2, 2));
    
    // 通用：治安官徽章
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_badge, 10, 1, 4));
    m_vendorItems.append(VendorItemConfig(ItemEffectManager::vendor_badge, 10, 2, 3));
    
    qDebug() << "供应商物品初始化完成，共" << m_vendorItems.size() << "个物品";
}

void VendorManager::unlockNextItem(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= 3) {
        return;
    }
    
    int currentProgress = m_slotProgress[slotIndex];
    
    // 根据进度解锁下一个物品
    for (const auto& item : m_vendorItems) {
        if (item.slotIndex == slotIndex && item.itemIndex == currentProgress) {
            qDebug() << "解锁卡槽" << slotIndex << "的下一个物品:" << ItemEffectManager::getItemName(item.itemType);
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
            int currentProgress = m_slotProgress[slotIndex];
            
            return itemIndex == currentProgress;
        }
    }
    return false;
} 
#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <memory>

#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
#include "viewmodel/ItemViewModel.h"
#include "viewmodel/ItemEffectManager.h"
#include "viewmodel/VendorManager.h"

class GameViewModel : public QObject {
    Q_OBJECT

public:
    
    explicit GameViewModel(QObject *parent = nullptr);
    ~GameViewModel() = default;
    
    /*
    GameViewModel的接口
    */
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame();
    void nextGame();
    void manualNextGame();  // 手动切换到下一个布局
    void updateGame(double deltaTime);

    ItemEffectManager* getItemEffectManager() const { return m_itemEffectManager.get(); }
    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    ItemViewModel* getItemViewModel() const { return m_item.get(); }

    void playerAttack(const QPointF& direction);
    void setPlayerMoveDirection(const QPointF& direction, bool isMoving);
    void useItem() { if(m_item) m_item->usePossessedItem(); }
    QList<ItemData> getActiveItems() const { 
        return m_item ? m_item->getActiveItems() : QList<ItemData>(); 
    }
    int getPossessedItemType() const {return m_item->getPossessedItemType();}
        
    GameState getGameState() const { return m_gameState; }
    bool isGameActive() const { return m_gameState == GameState::PLAYING; }

    // 供应商相关接口
    VendorManager* getVendorManager() const { return m_vendorManager.get(); }
    void purchaseVendorItem(int itemType);
    QList<int> getAvailableVendorItems() const;
    bool canPurchaseVendorItem(int itemType) const;
    int getVendorItemPrice(int itemType) const;

signals:
    void gameStateChanged(GameState state);
    void playerDied();
    void playerLivesDown();
    void playerPositonChanged(const QPointF& position);
    void itemUsed(int itemType); 
    void gameTimeChanged(double gameTime);
    void enemiesChanged(QList<EnemyData> enemies);
    void itemsChanged(QList<ItemData> items);
    void mapChanged();
    void gameWin();  // 游戏胜利信号
    void vendorAppeared();
    void vendorDisappeared();
    void vendorItemPurchased(int itemType);
    void vendorItemsChanged(const QList<int>& items);  // 供应商物品列表变化信号

    
private:
    GameState m_gameState = GameState::MENU;
    std::unique_ptr<PlayerViewModel> m_player;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<ItemViewModel> m_item;
    std::unique_ptr<ItemEffectManager> m_itemEffectManager;
    std::unique_ptr<VendorManager> m_vendorManager;
    double m_gameTime = 0.0;   
    int m_currentArea = 11;  // 当前区域，从1-1开始
    bool m_vendorActivated = false;  // 供应商是否已激活
    
    void checkGameState();
    void handlePlayerDeath();
    void setupConnections();
    void initializeComponents();
    void resetGame();
    void handlePlayerHitByEnemy(int enemyId);
    void handleEnemyHitByBullet(int bulletId, int enemyId);
    void handleEnemyHitByZombie(int enemyId);
    void handleCreateItem(int enemyId, const QPointF& position);
    void handleItemUsed(int itemType);
    void handleItemUsedImmediately(int itemType);
    void clearAllGameElements();  // 清除所有游戏元素
};

#endif // GAMEVIEWMODEL_H

#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <memory>

#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
#include "viewmodel/ItemViewModel.h"
#include "viewmodel/ItemEffectManager.h"

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
    void updateGame(double deltaTime);

    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    ItemViewModel* getItemViewModel() const { return m_item.get(); }

    void playerAttack(const QPointF& direction);
    void setPlayerMoveDirection(const QPointF& direction, bool isMoving);
    void useItem() { if(m_item) m_item->usePossessedItem(); }
        
signals:
    void gameStateChanged(GameState state);
    void playerDied();
    void playerLivesDown();
    void playerPositonChanged(const QPointF& position);
    void itemUsed(int itemType); 
    void gameTimeChanged(double gameTime);
    void enemiesChanged(QList<EnemyData> enemies);
    void itemsChanged(QList<ItemData> items);

    
private:
    GameState m_gameState = GameState::MENU;
    std::unique_ptr<PlayerViewModel> m_player;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<ItemViewModel> m_item;
    std::unique_ptr<ItemEffectManager> m_itemEffectManager;
    double m_gameTime = 0.0;   
    
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
};

#endif // GAMEVIEWMODEL_H

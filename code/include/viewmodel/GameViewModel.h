#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <QObject>
#include <memory>

#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
#include "viewmodel/ItemViewModel.h"

class GameViewModel : public QObject {
    Q_OBJECT

public:
    enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };
    
    explicit GameViewModel(QObject *parent = nullptr);
    ~GameViewModel() = default;
    
    /*
    GameViewModel的接口
    */
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame();
    void updateGame(double deltaTime);

    ItemViewModel* getItemViewModel() const { return m_item.get(); }
    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    void playerAttack(const QPointF& direction);
    void setPlayerMoveDirection(const QPointF& direction, bool isMoving);
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    QPointF getPlayerPosition() const { return m_player->getPosition();}
    int getPlayerLives() const { return m_player->getLives();}

    double getGameTime() const { return m_gameTime;}

    QList<ItemViewModel::ItemData> getActiveItems() const {return m_item->getActiveItems();}
    int getPossessedItemType() const {return m_item->getPossessedItemType();}
    void useItem();
    void pickItem(int itemType);
        
    GameState getGameState() const { return m_gameState; }
    bool isGameActive() const { return m_gameState == PLAYING; }
    
signals:
    void gameStateChanged(GameState state);
    void playerDied();
    void playerLivesChanged();
    void playerPositonChanged(const QPointF& position);
    void itemUsed(int itemType); // 道具使用信号
    void itemPicked(int itemType);
    
private:
    GameState m_gameState = MENU;
    std::unique_ptr<PlayerViewModel> m_player;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<ItemViewModel> m_item;
    double m_gameTime = 0.0;   
    
    void checkGameState();
    void handlePlayerDeath();
    void setupConnections();
    void initializeComponents();
    void resetGame();
    void handlePlayerHitByEnemy(int enemyId);
    void handleEnemyHitByBullet(int bulletId, int enemyId);
    void handleCreateItem(const int id);
    void handleItemUsedImmediately(int itemType);
};

#endif // GAMEVIEWMODEL_H

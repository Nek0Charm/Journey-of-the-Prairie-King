#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <QObject>
#include <memory>

#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
// 前向声明

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
    void playerAttack(const QPointF& direction);
    void setPlayerMoveDirection(const QPointF& direction, bool isMoving);
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    QPointF getPlayerPosition() const { return m_player->getPosition();}
    int getPlayerLives() const { return m_player->getLives();}
    
    
    void updateGame(double deltaTime);
        
    // 状态查询
    GameState getGameState() const { return m_gameState; }
    bool isGameActive() const { return m_gameState == PLAYING; }
    
signals:
    void gameStateChanged(GameState state);
    void playerDied();
    void playerLivesChanged();
    void playerPositonChanged(const QPointF& position);
    
private:
    GameState m_gameState = MENU;
    std::unique_ptr<PlayerViewModel> m_player;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    
    void checkGameState();
    void handlePlayerDeath();
    void setupConnections();
    void initializeComponents();
    void resetGame();
    void handlePlayerHitByEnemy(int enemyId);
    void handleEnemyHitByBullet(int bulletId, int enemyId);
};

#endif // GAMEVIEWMODEL_H

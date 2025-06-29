#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <QObject>
#include <memory>


// 前向声明
class PlayerViewModel;
class EnemyManager;
class InputManager;
class CollisionSystem;
class HUDViewModel;

class GameViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(GameState gameState READ getGameState NOTIFY gameStateChanged)
    Q_PROPERTY(bool isGameActive READ isGameActive NOTIFY gameStateChanged)
    
public:
    enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };
    
    explicit GameViewModel(QObject *parent = nullptr);
    ~GameViewModel() = default;
    
    /*
    GameViewModel的接口
    */
    Q_INVOKABLE void startGame();
    Q_INVOKABLE void pauseGame();
    Q_INVOKABLE void resumeGame();
    Q_INVOKABLE void endGame();
    Q_INVOKABLE void playerAttack();
    Q_INVOKABLE void playerMove(const QPointF& direction);

    // 游戏循环
    void updateGame(double deltaTime);
        
    // 状态查询
    GameState getGameState() const { return m_gameState; }
    bool isGameActive() const { return m_gameState == PLAYING; }
    
    // 获取组件
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    HUDViewModel* getHUDViewModel() const { return m_hudViewModel.get(); }
    
signals:
    void gameStateChanged(GameState state);
    void gameOver();
    void gameStarted();
    void gamePaused();
    void gameResumed();
    
private:
    GameState m_gameState = MENU;
    std::unique_ptr<PlayerViewModel> m_player;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<HUDViewModel> m_hudViewModel;
    
    void checkGameState();
    void handlePlayerDeath();
    void setupConnections();
    void initializeComponents();
    void resetGame();
};

#endif // GAMEVIEWMODEL_H

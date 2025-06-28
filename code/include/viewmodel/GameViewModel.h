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
    
public:
    enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };
    
    explicit GameViewModel(QObject *parent = nullptr);
    ~GameViewModel() = default;
    
    // 游戏控制
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame();
    
    // 游戏循环
    void updateGame(double deltaTime);
    
    // 输入处理
    void handleKeyPress(int key);
    void handleKeyRelease(int key);
    
    // 状态查询
    GameState getGameState() const { return m_gameState; }
    bool isGameActive() const { return m_gameState == PLAYING; }
    
    // 获取组件
    PlayerViewModel* getPlayer() const { return m_player.get(); }
    EnemyManager* getEnemyManager() const { return m_enemyManager.get(); }
    InputManager* getInputManager() const { return m_inputManager.get(); }
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
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<HUDViewModel> m_hudViewModel;
    
    void checkGameState();
    void handlePlayerDeath();
    void setupConnections();
    void initializeComponents();
    void resetGame();
};

#endif // GAMEVIEWMODEL_H

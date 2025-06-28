#include "viewmodel/GameViewModel.h"
#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
#include "viewmodel/HUDViewModel.h"
#include <QDebug>

GameViewModel::GameViewModel(QObject *parent)
    : QObject(parent)
    , m_gameState(MENU)
{
    initializeComponents();
    setupConnections();
}

void GameViewModel::startGame()
{
    if (m_gameState != PLAYING) {
        resetGame();
        m_gameState = PLAYING;
        emit gameStateChanged(m_gameState);
        emit gameStarted();
        qDebug() << "Game started";
    }
}

void GameViewModel::pauseGame()
{
    if (m_gameState == PLAYING) {
        m_gameState = PAUSED;
        emit gameStateChanged(m_gameState);
        emit gamePaused();
        qDebug() << "Game paused";
    }
}

void GameViewModel::resumeGame()
{
    if (m_gameState == PAUSED) {
        m_gameState = PLAYING;
        emit gameStateChanged(m_gameState);
        emit gameResumed();
        qDebug() << "Game resumed";
    }
}

void GameViewModel::endGame()
{
    if (m_gameState != GAME_OVER) {
        m_gameState = GAME_OVER;
        emit gameStateChanged(m_gameState);
        emit gameOver();
        qDebug() << "Game over";
    }
}

void GameViewModel::updateGame(double deltaTime)
{
    if (m_gameState != PLAYING) {
        return;
    }
    
    // 更新玩家
    m_player->update(deltaTime);
    
    // 更新敌人
    m_enemyManager->updateEnemies(deltaTime, m_player->getPosition());
    // TODO
    // 碰撞检测
    // m_collisionSystem->checkCollisions(*m_player, 
    //                                   m_enemyManager->getEnemies(),
    //                                   m_player->getBullets());
    
    // 更新HUD
    m_hudViewModel->updateLives(m_player->getLives());
    m_hudViewModel->updateEnemyCount(m_enemyManager->getActiveEnemyCount());
    
    // 检查游戏状态
    checkGameState();
}

void GameViewModel::checkGameState()
{
    // 检查玩家是否死亡
    if (m_player->getLives() <= 0) {
        handlePlayerDeath();
    }
}

void GameViewModel::handlePlayerDeath()
{
    endGame();
}

void GameViewModel::setupConnections()
{
    if (!m_player || !m_enemyManager || !m_collisionSystem) {
        return;
    }

    
    // 连接碰撞检测
    connect(m_collisionSystem.get(), &CollisionSystem::playerHitByEnemy,
            m_player.get(), &PlayerViewModel::takeDamage);
    
    connect(m_collisionSystem.get(), &CollisionSystem::enemyHitByBullet,
            m_enemyManager.get(), &EnemyManager::damageEnemy);
    
    // 连接玩家状态变化
    connect(m_player.get(), &PlayerViewModel::playerDied,
            this, &GameViewModel::handlePlayerDeath);
    
    connect(m_player.get(), &PlayerViewModel::livesChanged,
            m_hudViewModel.get(), &HUDViewModel::updateLives);
    
    // 连接敌人状态变化
    connect(m_enemyManager.get(), &EnemyManager::enemyCountChanged,
            m_hudViewModel.get(), &HUDViewModel::updateEnemyCount);
    
    // 连接游戏状态变化
    connect(this, &GameViewModel::gameStateChanged,
            [this](GameState state) {
                switch (state) {
                case PLAYING:
                    m_hudViewModel->setGameState("Playing");
                    break;
                case PAUSED:
                    m_hudViewModel->setGameState("Paused");
                    break;
                case GAME_OVER:
                    m_hudViewModel->setGameState("Game Over");
                    break;
                default:
                    m_hudViewModel->setGameState("Menu");
                    break;
                }
            });
}

void GameViewModel::initializeComponents()
{
    m_player = std::make_unique<PlayerViewModel>(this);
    m_enemyManager = std::make_unique<EnemyManager>(this);
    m_collisionSystem = std::make_unique<CollisionSystem>(this);
    m_hudViewModel = std::make_unique<HUDViewModel>(this);
}

void GameViewModel::resetGame()
{
    if (m_player) {
        // TODO
        // m_player->reset();
    }
    if (m_enemyManager) {
        m_enemyManager->clearAllEnemies();
    }
    if (m_hudViewModel) {
        m_hudViewModel->reset();
    }
}

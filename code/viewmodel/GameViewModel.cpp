#include "viewmodel/GameViewModel.h"

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
        qDebug() << "Game started";
    }
}

void GameViewModel::pauseGame()
{
    if (m_gameState == PLAYING) {
        m_gameState = PAUSED;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game paused";
    }
}

void GameViewModel::resumeGame()
{
    if (m_gameState == PAUSED) {
        m_gameState = PLAYING;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game resumed";
    }
}

void GameViewModel::endGame()
{
    if (m_gameState != GAME_OVER) {
        m_gameState = GAME_OVER;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game over";
    }
}

void GameViewModel::playerAttack(const QPointF& direction) {
    if (m_gameState == PLAYING && m_player) {
        m_player->shoot(direction);
        qDebug() << "Player attacked in direction:" << direction;
    }
}

void GameViewModel::setPlayerMoveDirection(const QPointF& direciton, bool isMoving) {
    if(m_gameState == PLAYING && m_player) {
        m_player->setMovingDirection(direciton, isMoving);
    }
}

void GameViewModel::updateGame(double deltaTime)
{
    // qDebug() << "Updating game state, deltaTime:" << deltaTime;
    
    if (m_gameState != PLAYING) {
        return;
    }

    m_gameTime += deltaTime;
    if(m_gameTime > MAX_GAMETIME) {
        m_gameTime = 0.0; 
        endGame();
        return;
    }
    
    // 更新玩家
    m_player->update(deltaTime);
    
    // 更新敌人
    m_enemyManager->updateEnemies(deltaTime, m_player->getPosition());

    m_collisionSystem->checkCollisions(*m_player, 
                                      m_enemyManager->getEnemies(),
                                      m_player->getActiveBullets());
    
    
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
    emit playerDied();
    endGame();
}

void GameViewModel::setupConnections()
{
    if (!m_player || !m_enemyManager || !m_collisionSystem) {
        return;
    }

    
    // 连接碰撞检测
    connect(m_collisionSystem.get(), &CollisionSystem::playerHitByEnemy,
            this, &GameViewModel::handlePlayerHitByEnemy);
    
    connect(m_collisionSystem.get(), &CollisionSystem::enemyHitByBullet,
            this, &GameViewModel::handleEnemyHitByBullet);

    
    // 连接玩家状态变化
    connect(m_player.get(), &PlayerViewModel::playerDied,
            this, &GameViewModel::handlePlayerDeath);

    connect(m_player.get(), &PlayerViewModel::livesChanged,
            this, &GameViewModel::playerLivesChanged);

    connect(m_player.get(), &PlayerViewModel::positionChanged,
            this, &GameViewModel::playerPositonChanged);
    
    // 连接游戏状态变化
}

void GameViewModel::initializeComponents()
{

    m_player = std::make_unique<PlayerViewModel>(this);

    m_enemyManager = std::make_unique<EnemyManager>(this);
    m_collisionSystem = std::make_unique<CollisionSystem>(this);
}

void GameViewModel::resetGame()
{
    m_gameTime = 0.0;
    if (m_player) {
        m_player->reset();
    }
    if (m_enemyManager) {
        m_enemyManager->clearAllEnemies();
    }
}

void GameViewModel::handlePlayerHitByEnemy(int enemyId)
{
    m_player->takeDamage();
    m_enemyManager->removeEnemy(enemyId);
    m_player->setPositon({MAP_WIDTH / 2.0, MAP_HEIGHT / 2.0});
    m_enemyManager->clearAllEnemies();
    m_gameTime = std::max(m_gameTime - 5.0, 0.0);
}

void GameViewModel::handleEnemyHitByBullet(int bulletId, int enemyId)
{
    m_enemyManager->damageEnemy(bulletId, enemyId);
    m_player->removeBullet(bulletId);
    
}

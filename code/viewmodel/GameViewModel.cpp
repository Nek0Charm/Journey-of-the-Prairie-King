#include "viewmodel/GameViewModel.h"
#include <QRandomGenerator>
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
    
    m_item->updateItems(deltaTime, m_player->getPosition());
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
    
    connect(m_enemyManager.get(), &EnemyManager::enemyDestroyed,
            this, &GameViewModel::handleCreateItem);
    
    // 连接道具立即使用信号
    connect(m_item.get(), &ItemViewModel::itemUsedImmediately,
            this, &GameViewModel::handleItemUsedImmediately);
}

void GameViewModel::initializeComponents()
{

    m_player = std::make_unique<PlayerViewModel>(this);
    m_item = std::make_unique<ItemViewModel>(this);
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

void GameViewModel::handleCreateItem(const int id)
{
    int rand = QRandomGenerator::global()->bounded(10);
    qDebug() << "rand: " << rand;
    if(rand < 2) {
        m_item->createItem(m_enemyManager->getEnemyPosition(id) ,m_item->m_itemPossibilities);
    }
}

void GameViewModel::useItem() {
    if(m_item->hasPossessedItem()) {
        int type = m_item->getPossessedItemType();
        switch(type) {
            case ItemViewModel::coin:
                qDebug() << "coin";
                break;
            case ItemViewModel::five_coins:
                qDebug() << "five_coins";
                break;

            case ItemViewModel::extra_life:
                m_player->addLife();
                qDebug() << "extra_life";
                break;
            case ItemViewModel::coffee:
                m_player->setMoveSpeed(m_player->getMoveSpeed() * 1.2);
                qDebug() << "coffee";
                break;
            case ItemViewModel::machine_gun:
                m_player->setShootCooldown(0.1);
                qDebug() << "machine_gun";
                break;

            case ItemViewModel::bomb:
                m_enemyManager->clearAllEnemies();
                qDebug() << "bomb";
                break;
        }
        
        // 发出道具使用信号
        emit itemUsed(type);
    } 
}

void GameViewModel::handleItemUsedImmediately(int itemType) {
    // 处理道具立即使用效果
    switch(itemType) {
        case ItemViewModel::coin:
            qDebug() << "立即使用金币";
            // 金币效果：增加分数或金钱
            break;
        case ItemViewModel::five_coins:
            qDebug() << "立即使用五个金币";
            // 五个金币效果：增加更多分数或金钱
            break;
        case ItemViewModel::extra_life:
            m_player->addLife();
            qDebug() << "立即使用额外生命";
            break;
        case ItemViewModel::coffee:
            m_player->setMoveSpeed(m_player->getMoveSpeed() * 1.2);
            qDebug() << "立即使用咖啡";
            break;
        case ItemViewModel::machine_gun:
            m_player->setShootCooldown(0.1);
            qDebug() << "立即使用机枪";
            break;
        case ItemViewModel::bomb:
            m_enemyManager->clearAllEnemies();
            qDebug() << "立即使用炸弹";
            break;
        case ItemViewModel::shotgun:
            m_player->setShootCooldown(0.15);
            qDebug() << "立即使用霰弹枪";
            break;
        case ItemViewModel::wheel:
            m_player->setMoveSpeed(m_player->getMoveSpeed() * 1.5);
            qDebug() << "立即使用轮子";
            break;
        default:
            qDebug() << "立即使用未知道具:" << itemType;
            break;
    }
    
    // 发出道具使用信号
    emit itemUsed(itemType);
}
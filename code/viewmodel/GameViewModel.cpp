#include "viewmodel/GameViewModel.h"
#include <QRandomGenerator>

GameViewModel::GameViewModel(QObject *parent)
    : QObject(parent)
    , m_gameState(GameState::MENU)
{
    initializeComponents();
    setupConnections();
}

void GameViewModel::startGame()
{
    if (m_gameState != GameState::PLAYING) {
        resetGame();
        m_gameState = GameState::PLAYING;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game started";
    }
}

void GameViewModel::pauseGame()
{
    if (m_gameState == GameState::PLAYING) {
        m_gameState = GameState::PAUSED;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game paused";
    }
}

void GameViewModel::resumeGame()
{
    if (m_gameState == GameState::PAUSED) {
        m_gameState = GameState::PLAYING;
        emit gameStateChanged(m_gameState);
        qDebug() << "Game resumed";
    }
}

void GameViewModel::endGame()
{
    if (m_gameState != GameState::GAME_OVER) {
        m_gameState = GameState::GAME_OVER;
        resetGame();
        emit gameStateChanged(m_gameState);
        qDebug() << "Game over";
    }
}

void GameViewModel::playerAttack(const QPointF& direction) {
    if (m_gameState == GameState::PLAYING && m_player) {
        m_player->shoot(direction);
        // qDebug() << "Player attacked in direction:" << direction;
    }
}

void GameViewModel::setPlayerMoveDirection(const QPointF& direciton, bool isMoving) {
    if(m_gameState == GameState::PLAYING && m_player) {
        m_player->setMovingDirection(direciton, isMoving);
    }
}

void GameViewModel::updateGame(double deltaTime)
{
    // qDebug() << "Updating game state, deltaTime:" << deltaTime;
    
    if (m_gameState != GameState::PLAYING) {
        return;
    }

    m_gameTime += deltaTime;
    m_gameTime = std::min(m_gameTime, MAX_GAMETIME);
    if(m_gameTime == MAX_GAMETIME && m_enemyManager->getActiveEnemyCount() == 0) {
        emit gameTimeChanged(m_gameTime);
        endGame();
        return;
    }
    emit gameTimeChanged(m_gameTime);
    
    // 更新玩家
    m_player->update(deltaTime);
    
    // 更新敌人（传递玩家潜行状态）
    m_enemyManager->updateEnemies(deltaTime, m_player->getPosition(), m_player->isStealthMode(), m_gameTime == MAX_GAMETIME);

    m_collisionSystem->checkCollisions(*m_player, 
                                      m_enemyManager->getEnemies(),
                                      m_player->getActiveBullets());
    
    m_item->updateItems(deltaTime, m_player->getPosition());
    
    // 更新道具效果
    m_itemEffectManager->updateEffects(deltaTime, m_player.get());
    
    // 检查供应商出现
    m_vendorManager->checkVendorAppearance(m_currentArea);
    
    // 只在供应商激活时才发出供应商物品列表变化信号
    static QList<int> lastVendorItems;
    static bool lastVendorActive = false;
    bool currentVendorActive = m_vendorManager->isVendorActive();
    
    if (currentVendorActive) {
        QList<int> currentVendorItems = m_vendorManager->getAvailableUpgradeItems();
        if (lastVendorItems != currentVendorItems || !lastVendorActive) {
            lastVendorItems = currentVendorItems;
            emit vendorItemsChanged(currentVendorItems);
        }
    } else if (lastVendorActive) {
        // 供应商刚刚消失，清空物品列表
        lastVendorItems.clear();
        emit vendorItemsChanged(QList<int>());
    }
    
    lastVendorActive = currentVendorActive;
    
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
    if (!m_player || !m_enemyManager || !m_collisionSystem || !m_vendorManager) {
        return;
    }

    
    // 连接碰撞检测
    connect(m_collisionSystem.get(), &CollisionSystem::playerHitByEnemy,
            this, &GameViewModel::handlePlayerHitByEnemy);
    
    connect(m_collisionSystem.get(), &CollisionSystem::enemyHitByBullet,
            this, &GameViewModel::handleEnemyHitByBullet);
    
    connect(m_collisionSystem.get(), &CollisionSystem::enemyHitByZombie,
            this, &GameViewModel::handleEnemyHitByZombie);

    
    // 连接玩家状态变化
    connect(m_player.get(), &PlayerViewModel::playerDied,
            this, &GameViewModel::handlePlayerDeath);

    connect(m_player.get(), &PlayerViewModel::livesDown,
            this, &GameViewModel::playerLivesDown);
            
    connect(m_player.get(), &PlayerViewModel::positionChanged,
            this, &GameViewModel::playerPositonChanged);
    
    connect(m_enemyManager.get(), &EnemyManager::enemyDestroyed,
            this, &GameViewModel::handleCreateItem);
    
    // 连接道具使用信号
    connect(m_item.get(), &ItemViewModel::itemUsed,
            this, &GameViewModel::handleItemUsed);
    connect(m_item.get(), &ItemViewModel::itemUsedImmediately,
            this, &GameViewModel::handleItemUsedImmediately);

    // 连接供应商信号
    connect(m_vendorManager.get(), &VendorManager::vendorAppeared,
            this, &GameViewModel::vendorAppeared);
    connect(m_vendorManager.get(), &VendorManager::vendorDisappeared,
            this, &GameViewModel::vendorDisappeared);
    connect(m_vendorManager.get(), &VendorManager::itemPurchased,
            this, &GameViewModel::vendorItemPurchased);
}

void GameViewModel::initializeComponents()
{
    m_player = std::make_unique<PlayerViewModel>(this);
    m_item = std::make_unique<ItemViewModel>(this);
    m_enemyManager = std::make_unique<EnemyManager>(this);
    m_collisionSystem = std::make_unique<CollisionSystem>(this);
    m_itemEffectManager = std::make_unique<ItemEffectManager>(this);
    m_vendorManager = std::make_unique<VendorManager>(this);
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
    if (m_item) {
        m_item->clearAllItems();
    }
    if (m_itemEffectManager) {
        m_itemEffectManager->clearAllEffects();
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

void GameViewModel::handleEnemyHitByZombie(int enemyId)
{
    // 僵尸模式：直接击杀敌人
    m_enemyManager->damageEnemy(0, enemyId); // 使用0作为占位符bulletId
    qDebug() << "僵尸模式接触击杀敌人:" << enemyId;
}

void GameViewModel::handleCreateItem(int enemyId, const QPointF& position)
{
    // 直接使用传递的位置参数生成道具
    m_item->spawnItemAtPosition(position);
}

void GameViewModel::handleItemUsed(int itemType) {
    // 使用ItemEffectManager处理道具效果
    m_itemEffectManager->applyItemEffect(itemType, m_player.get(), m_enemyManager.get());
    // 发出道具使用信号
    emit itemUsed(itemType);
}
void GameViewModel::handleItemUsedImmediately(int itemType) {
    handleItemUsed(itemType);
}

// 供应商相关方法实现
void GameViewModel::purchaseVendorItem(int itemType) {
    if (m_vendorManager && m_player) {
        if (m_vendorManager->purchaseItem(itemType, m_player.get())) {
            // 购买成功后，应用物品效果
            m_itemEffectManager->applyItemEffect(itemType, m_player.get(), m_enemyManager.get(), true);
        }
    }
}

QList<int> GameViewModel::getAvailableVendorItems() const {
    return m_vendorManager ? m_vendorManager->getAvailableUpgradeItems() : QList<int>();
}

bool GameViewModel::canPurchaseVendorItem(int itemType) const {
    if (!m_vendorManager || !m_player) {
        return false;
    }
    return m_vendorManager->canPurchaseItem(itemType, m_player->getCoins());
}

int GameViewModel::getVendorItemPrice(int itemType) const {
    return m_vendorManager ? m_vendorManager->getItemPrice(itemType) : 0;
}
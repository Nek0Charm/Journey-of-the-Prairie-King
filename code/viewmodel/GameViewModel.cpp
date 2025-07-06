#include "viewmodel/GameViewModel.h"
#include <QRandomGenerator>
#include "common/GameMap.h"

GameViewModel::GameViewModel(QObject *parent)
    : QObject(parent)
    , m_gameState(GameState::MENU)
{
    initializeComponents();
    setupConnections();
}

void GameViewModel::startGame()
{
    qDebug() << "[GameViewModel::startGame] called";
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

void GameViewModel::nextGame() {
    m_enemyManager->clearAllEnemies();
    m_gameTime = 0.0;
    
    // 更新当前区域编号
    int area1 = m_currentArea / 10;  // 地图编号
    int area2 = m_currentArea % 10;  // 布局编号
    
    // 切换到下一个布局
    area2++;
    
    // 如果布局超出范围，切换到下一个地图
    if (area2 > 5) {  // 假设每个地图最多5个布局
        area1++;
        area2 = 1;
    }
    
    // 如果地图超出范围，回到第一关
    if (area1 > 3) {  // 假设最多3个地图
        area1 = 1;
        area2 = 1;
    }
    
    m_currentArea = area1 * 10 + area2;
    
    // 检查是否是布局1-2结束后（area1=1, area2=3，即map_1的布局3）
    if (area1 == 1 && area2 == 3) {
        qDebug() << "布局1-2结束，触发游戏胜利";
        // 重置供应商状态，防止供应商再次出现
        m_vendorActivated = false;
        m_vendorManager->hideVendor();
        // 触发游戏胜利
        emit gameWin();
        return;
    }
    
    // 根据当前区域加载对应的地图
    QString mapName = QString("map_%1").arg(area1);
    QString layoutName = QString::number(area2);
    
    GameMap::instance().loadFromFile(":/assert/picture/gamemap.json", mapName, layoutName);
    
    // 只在切换到不同地图或布局2结束后重置供应商状态
    if (area1 > 1 || (area1 == 1 && area2 > 2)) {
        m_vendorActivated = false;
        m_vendorManager->hideVendor();
    }
    
    emit mapChanged();
}

void GameViewModel::manualNextGame() {
    // 手动切换到下一个布局，隐藏供应商
    qDebug() << "手动切换到下一个布局，隐藏供应商";
    m_vendorManager->hideVendor();
    m_vendorActivated = false;
    // 发送供应商消失信号，确保UI更新
    emit vendorDisappeared();
    // 然后切换到下一个布局
    nextGame();
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
    
    // 检查是否应该激活供应商（只在布局1-1结束后）
    int currentArea1 = m_currentArea / 10;  // 当前地图编号
    int currentArea2 = m_currentArea % 10;  // 当前布局编号
    if(m_gameTime >= MAX_GAMETIME && m_enemyManager->getActiveEnemyCount() == 0 && !m_vendorActivated && currentArea1 == 1 && currentArea2 == 1) {
        emit gameTimeChanged(m_gameTime);
        qDebug() << "布局1-1结束，激活供应商";
        // 激活供应商
        m_vendorManager->showVendor();
        m_vendorActivated = true;
        // 不要return，让后续的供应商状态更新逻辑执行
    }
    emit gameTimeChanged(m_gameTime);
    m_collisionSystem->checkCollisions(*m_player, 
                                      m_enemyManager->getEnemies(),
                                      m_player->getActiveBullets());
    // 更新玩家
    m_player->update(deltaTime);
    
    // 更新敌人（传递玩家潜行状态）
    m_enemyManager->updateEnemies(deltaTime, m_player->getPosition(), m_player->isStealthMode(), m_player->isZombieMode(), m_gameTime == MAX_GAMETIME);

    m_collisionSystem->checkCollisions(*m_player, 
                                      m_enemyManager->getEnemies(),
                                      m_player->getActiveBullets());
    
    m_item->updateItems(deltaTime, m_player->getPosition());
    
    // 更新道具效果
    m_itemEffectManager->updateEffects(deltaTime, m_player.get());
    
    // 只在供应商激活时才发出供应商物品列表变化信号
    static QList<int> lastVendorItems;
    static bool lastVendorActive = false;
    bool currentVendorActive = m_vendorManager->isVendorActive();
    
    if (currentVendorActive) {
        QList<int> currentVendorItems = m_vendorManager->getAvailableUpgradeItems();
        if (lastVendorItems != currentVendorItems || !lastVendorActive) {
            lastVendorItems = currentVendorItems;
            qDebug() << "供应商状态更新，发送物品列表:" << currentVendorItems;
            emit vendorItemsChanged(currentVendorItems);
        }
    } else if (lastVendorActive) {
        // 供应商刚刚消失，清空物品列表
        lastVendorItems.clear();
        qDebug() << "供应商消失，清空物品列表";
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
    m_player->getBulletViewModel()->clearAllBullets();
    m_enemyManager->clearAllEnemies();
    m_player->getBulletViewModel()->clearAllBullets();
    m_player->takeDamage();
    m_player->setPositon({MAP_WIDTH / 2.0, MAP_HEIGHT / 2.0});
    m_gameTime = std::max(m_gameTime - 5.0, 0.0);
}

void GameViewModel::handleEnemyHitByBullet(int bulletId, int enemyId)
{
    // 获取子弹的当前伤害值
    int bulletDamage = m_player->getBulletViewModel()->getBulletDamage(bulletId);
    
    // 对敌人造成伤害，传递子弹的伤害值
    m_enemyManager->damageEnemy(bulletId, enemyId, bulletDamage);
    
    // 减少子弹的伤害值
    bulletDamage--;
    
    // 更新子弹伤害值，如果伤害值为0或负数，子弹会被自动标记为非活动状态
    m_player->getBulletViewModel()->updateBulletDamage(bulletId, bulletDamage);
}

void GameViewModel::handleEnemyHitByZombie(int enemyId)
{
    // 僵尸模式：直接击杀敌人
    m_enemyManager->damageEnemy(0, enemyId, 999); // 使用999作为占位符bulletId，999伤害确保击杀
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
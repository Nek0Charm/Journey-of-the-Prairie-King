#include "app/GameService.h"

GameService::GameService(MainWindow *mainWindow, GameViewModel *viewModel, AudioEventListener *audioEventListener,QObject *parent)
    :   QObject(parent), 
        m_mainWindow(mainWindow), 
        m_gameViewModel(viewModel), 
        m_audioEventListener(audioEventListener) {
    setupConnections();
}

void GameService::setupConnections() {
    connect(m_mainWindow, &MainWindow::startGameRequested, m_gameViewModel, &GameViewModel::startGame);
    connect(m_mainWindow, &MainWindow::exitGameRequested, m_gameViewModel, &GameViewModel::endGame);

    
    connect(m_mainWindow->getGameWidget(), &GameWidget::setMovingDirection, m_gameViewModel, &GameViewModel::setPlayerMoveDirection);
    connect(m_mainWindow->getGameWidget(), &GameWidget::shoot, m_gameViewModel, &GameViewModel::playerAttack);
    connect(m_mainWindow->getGameWidget(), &GameWidget::gameWin, m_gameViewModel, &GameViewModel::pauseGame);
    connect(m_mainWindow->getGameWidget(), &GameWidget::pauseGame, m_gameViewModel, &GameViewModel::pauseGame);
    connect(m_mainWindow->getGameWidget(), &GameWidget::resumeGame, m_gameViewModel, &GameViewModel::resumeGame);
    connect(m_gameViewModel, &GameViewModel::gameStateChanged, m_mainWindow, &MainWindow::onGameStateChanged);
    connect(m_gameViewModel, &GameViewModel::mapChanged, m_mainWindow->getGameWidget(), &GameWidget::onMapChanged);
    connect(m_gameViewModel, &GameViewModel::gameWin, m_mainWindow->getGameWidget(), &GameWidget::onGameWin);


    connect(m_gameViewModel, &GameViewModel::playerPositonChanged, m_mainWindow->getGameWidget(), &GameWidget::playerPositionChanged);
    connect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemyDestroyed, m_mainWindow->getGameWidget(), &GameWidget::die);

    connect(m_gameViewModel->getPlayer()->getBulletViewModel(), &BulletViewModel::bulletsChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateBullets);

    connect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemiesChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateEnemies);

    connect(m_gameViewModel->getItemViewModel(), &ItemViewModel::itemsChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateItems);

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::playerStealthModeChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updatePlayerStealthMode);

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::healthChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updatePlayerHealth);

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::coinsChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updatePlayerMoney);

    connect(m_gameViewModel->getItemViewModel(), &ItemViewModel::possessedItemChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updatePossessedItem);
            
    connect(m_gameViewModel, &GameViewModel::gameTimeChanged, m_mainWindow->getGameWidget(), &GameWidget::updateGameTime);
    
    
    connect(m_mainWindow->getGameWidget(), &GameWidget::useItem, m_gameViewModel, &GameViewModel::useItem);
            

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::zombieModeChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateZombieMode);

    connect(m_gameViewModel, &GameViewModel::playerDied, m_audioEventListener, &AudioEventListener::onPlayerHit);
    connect(m_gameViewModel, &GameViewModel::playerLivesDown, m_audioEventListener, &AudioEventListener::onPlayerHit);
    
    // 连接敌人爆炸事件信号
    connect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemyDestroyed, m_audioEventListener, &AudioEventListener::onEnemyExplosion);
    connect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemyDamaged, m_mainWindow->getGameWidget(), &GameWidget::onEnemyHitByBullet);
    // 新增：连接游戏状态变化信号
    connect(m_gameViewModel, &GameViewModel::gameStateChanged, m_audioEventListener, &AudioEventListener::onGameStateChanged);
    
    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::positionChanged, m_audioEventListener, &AudioEventListener::onPlayerMove);
    // 新增：连接shot信号到onPlayerShot槽
    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::shot, m_audioEventListener, &AudioEventListener::onPlayerShot);
    
    connect(m_gameViewModel->getItemEffectManager(), &ItemEffectManager::usedTombstone,
            m_audioEventListener, &AudioEventListener::onUsedTombstone);
    connect(m_gameViewModel->getItemEffectManager(), &ItemEffectManager::tombstoneFinished,
            m_audioEventListener, &AudioEventListener::onTombstoneFinished);

    connect(m_gameViewModel, &GameViewModel::itemUsed,
            m_mainWindow->getGameWidget(), &GameWidget::updateItemEffect);

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::playerStealthModeChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateStealthMode);
            
    // 连接供应商相关信号
    connect(m_gameViewModel, &GameViewModel::vendorAppeared,
            m_mainWindow->getGameWidget(), &GameWidget::onVendorAppeared);
    connect(m_gameViewModel, &GameViewModel::vendorDisappeared,
            m_mainWindow->getGameWidget(), &GameWidget::onVendorDisappeared);
    connect(m_gameViewModel, &GameViewModel::vendorItemPurchased,
            m_mainWindow->getGameWidget(), &GameWidget::onVendorItemPurchased);
            
    // 连接供应商购买请求
    connect(m_mainWindow->getGameWidget(), &GameWidget::purchaseVendorItem,
            m_gameViewModel, &GameViewModel::purchaseVendorItem);
            
    // 连接供应商物品列表更新
    connect(m_gameViewModel, &GameViewModel::vendorItemsChanged,
            m_mainWindow->getGameWidget(), &GameWidget::setAvailableVendorItems);
            
    // 连接手动切换布局信号
    connect(m_mainWindow->getGameWidget(), &GameWidget::manualNextGame,
            m_gameViewModel, &GameViewModel::manualNextGame);
}

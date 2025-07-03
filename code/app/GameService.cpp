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

    connect(m_gameViewModel, &GameViewModel::gameStateChanged, m_mainWindow, &MainWindow::onGameStateChanged);


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
    
    // 新增：连接游戏状态变化信号
    connect(m_gameViewModel, &GameViewModel::gameStateChanged, m_audioEventListener, &AudioEventListener::onGameStateChanged);
    
    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::positionChanged, m_audioEventListener, &AudioEventListener::onPlayerMove);
    // 新增：连接shot信号到onPlayerShot槽
    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::shot, m_audioEventListener, &AudioEventListener::onPlayerShot);
    

    connect(m_gameViewModel, &GameViewModel::itemUsed,
            m_mainWindow->getGameWidget(), &GameWidget::updateItemEffect);

    connect(m_gameViewModel->getPlayer(), &PlayerViewModel::playerStealthModeChanged,
            m_mainWindow->getGameWidget(), &GameWidget::updateStealthMode);
}

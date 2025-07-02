#include "common/GameService.h"

GameService::GameService(MainWindow *mainWindow, GameViewModel *viewModel, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_gameViewModel(viewModel) {
    setupConnections();
}

void GameService::setupConnections() {
    connect(m_mainWindow, &MainWindow::startGameRequested, m_gameViewModel, &GameViewModel::startGame);
    connect(m_mainWindow, &MainWindow::exitGameRequested, m_gameViewModel, &GameViewModel::endGame);
    
    connect(m_mainWindow->getGameWidget(), &GameWidget::setMovingDirection, m_gameViewModel, &GameViewModel::setPlayerMoveDirection);
    connect(m_mainWindow->getGameWidget(), &GameWidget::shoot, m_gameViewModel, &GameViewModel::playerAttack);

    connect(m_gameViewModel, &GameViewModel::gameStateChanged, m_mainWindow, &MainWindow::onGameStateChanged);


    connect(m_gameViewModel, &GameViewModel::playerPositonChanged, m_mainWindow->getGameWidget(), &GameWidget::playerPositionChanged);
    connect(m_gameViewModel, &GameViewModel::playerLivesChanged, m_mainWindow->getGameWidget(), &GameWidget::playerLivesChanged);
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
            
}

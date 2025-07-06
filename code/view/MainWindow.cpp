#include "view/MainWindow.h"
#include "view/StartWidget.h"
#include "view/GameWidget.h"
#include "view/EndWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent){
    m_stackedWidget = new QStackedWidget(this);
    m_startWidget = new StartWidget(this);
    m_gameWidget = new GameWidget(this);
    m_endWidget = new EndWidget(this);
    m_stackedWidget->addWidget(m_endWidget);
    m_stackedWidget->addWidget(m_startWidget);
    m_stackedWidget->addWidget(m_gameWidget);
    this->setCentralWidget(m_stackedWidget);
    setWindowTitle("Maodie Adventure");
    setupUi();
    setupConnections();
    update();
}

void MainWindow::setupUi() {
    resize(1080, 1080); 
    this->setStyleSheet("background-color: black;");  
}

void MainWindow::setupConnections() {
    if (m_startWidget) {
        connect(m_startWidget, &StartWidget::startGameClicked, this, &MainWindow::onStartGameRequested);
        connect(m_startWidget, &StartWidget::exitGameClicked, this, &MainWindow::onExitGameRequested);
    }
    if (m_endWidget) {
        connect(m_endWidget, &EndWidget::restartGameClicked, this, &MainWindow::onRestartGameRequested);
        connect(m_endWidget, &EndWidget::exitToMenuClicked, this, &MainWindow::onExitToMenuRequested);
    }
}

void MainWindow::update() {
    if (m_stackedWidget) {
        switch (m_gameState) {
            case GameState::MENU:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
            case GameState::PAUSED:
                
            case GameState::PLAYING:
                m_stackedWidget->setCurrentWidget(m_gameWidget);
                break;
            case GameState::GAME_OVER:
                m_gameWidget->clearKeys();
                m_stackedWidget->setCurrentWidget(m_endWidget);
                break;
            default:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
        }
    }
}

void MainWindow::onStartGameRequested()
{
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    emit startGameRequested();
}

void MainWindow::onExitGameRequested() {
    QApplication::quit();
}

void MainWindow::onRestartGameRequested() {
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    emit startGameRequested();
}

void MainWindow::onExitToMenuRequested() {
    QApplication::quit();
}

void MainWindow::onGameStateChanged(GameState state) {
    m_gameState = state;
    update();
}



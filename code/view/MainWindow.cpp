#include "view/MainWindow.h"
// #include "view/menuview.h"
#include "view/StartWidget.h"
#include "view/GameWidget.h"
#include "view/EndWidget.h"
#include "MainWindow.h"

MainWindow::MainWindow(GameViewModel *viewModel, QWidget *parent)
    : QMainWindow(parent), m_gameViewModel(viewModel) {
    m_stackedWidget = new QStackedWidget(this);
    m_startWidget = new StartWidget(this);
    m_gameWidget = new GameWidget(viewModel ,this);
    m_endWidget = new EndWidget(this);
    m_stackedWidget->addWidget(m_endWidget);
    m_stackedWidget->addWidget(m_startWidget);
    m_stackedWidget->addWidget(m_gameWidget);
    this->setCentralWidget(m_stackedWidget);
    setWindowTitle("Maodie Adventure");
    setupUi();
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
    if (m_stackedWidget && m_gameViewModel) {
        qDebug() << m_gameViewModel->getGameState();
        switch (m_gameViewModel->getGameState()) {
            case GameViewModel::MENU:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
            case GameViewModel::PLAYING:
                m_stackedWidget->setCurrentWidget(m_gameWidget);
                break;
            case GameViewModel::GAME_OVER:
                m_gameWidget->clearKeys();
                m_stackedWidget->setCurrentWidget(m_endWidget);
                break;
            default:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
        }
    }
    setupConnections();
}

void MainWindow::onStartGameRequested()
{
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    m_gameViewModel->startGame();
}

void MainWindow::onExitGameRequested() {
    QApplication::quit();
}

void MainWindow::onRestartGameRequested() {
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    m_gameViewModel->startGame();
}

void MainWindow::onExitToMenuRequested() {
    QApplication::quit();
}

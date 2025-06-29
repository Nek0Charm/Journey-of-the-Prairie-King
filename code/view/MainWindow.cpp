#include "view/MainWindow.h"
// #include "view/menuview.h"
#include "view/StartWidget.h"
#include "view/GameWidget.h"
#include "MainWindow.h"

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent) {
//     m_gameViewModel = new GameViewModel();
//     setWindowTitle("Maodie Adventure");
//     setupUi();
//     setupConnections();
// }

MainWindow::MainWindow(GameViewModel *viewModel, QWidget *parent)
    : QMainWindow(parent), m_gameViewModel(viewModel) {
    m_stackedWidget = new QStackedWidget(this);
    m_startWidget = new StartWidget(this);
    m_gameWidget = new GameWidget(viewModel ,this);
    m_stackedWidget->addWidget(m_startWidget);
    m_stackedWidget->addWidget(m_gameWidget);
    this->setCentralWidget(m_stackedWidget);
    setWindowTitle("Maodie Adventure");
    setupUi();
    setupConnections();
    if (m_stackedWidget && viewModel) {
        switch (viewModel->getGameState()) {
            case GameViewModel::MENU:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
            case GameViewModel::PLAYING:
                m_stackedWidget->setCurrentWidget(m_gameWidget);
                break;
            default:
                m_stackedWidget->setCurrentWidget(m_startWidget);
                break;
        }
    }
}

void MainWindow::setupUi() {
    resize(1550, 1390); // (16*16+54)*5 = 1550, (16*16+22)*5 = 1390
    // setFixedSize(1550, 1390);
    this->setStyleSheet("background-color: black;");  
}

void MainWindow::setupConnections() {
    if (m_startWidget) {
        connect(m_startWidget, &StartWidget::startGameClicked, this, &MainWindow::onStartGameRequested);
        connect(m_startWidget, &StartWidget::exitGameClicked, this, &MainWindow::onExitGameRequested);
    }
}

void MainWindow::onStartGameRequested()
{
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    m_gameViewModel->startGame();
}

void MainWindow::onExitGameRequested() {
    QApplication::quit();
}


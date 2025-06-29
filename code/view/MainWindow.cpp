#include "view/MainWindow.h"
// #include "view/menuview.h"
#include "view/StartWidget.h"
#include "view/GameWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    m_gameViewModel = new GameViewModel();
    setWindowTitle("Maodie Adventure");
    setupUi();
    setupConnections();
}

void MainWindow::setupUi() {
    resize(1550, 1390); // (16*16+54)*5 = 1550, (16*16+22)*5 = 1390
    // setFixedSize(1550, 1390);
    this->setStyleSheet("background-color: black;");  
    m_stackedWidget = new QStackedWidget(this);
    m_startWidget = new StartWidget(this);
    m_gameWidget = new GameWidget(this);
    m_stackedWidget->addWidget(m_startWidget);
    m_stackedWidget->addWidget(m_gameWidget);
    this->setCentralWidget(m_stackedWidget);
    m_stackedWidget->setCurrentWidget(m_startWidget);
}

void MainWindow::setupConnections() {
    if (m_startWidget) {
        connect(m_startWidget, &StartWidget::startGameClicked, this, &MainWindow::onStartGameRequested);
        connect(m_startWidget, &StartWidget::exitGameClicked, this, &MainWindow::onExitGameRequested);
    }
}

void MainWindow::onStartGameRequested() {
    m_stackedWidget->setCurrentWidget(m_gameWidget);
    m_gameViewModel->startGame();
}

void MainWindow::onExitGameRequested() {
    QApplication::quit();
}


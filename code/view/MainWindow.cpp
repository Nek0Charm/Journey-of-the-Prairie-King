#include "view/MainWindow.h"
#include "view/menuview.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_stackedWidget(nullptr)
    , m_menuView(nullptr)
{
}

bool MainWindow::initialize() {
    setWindowTitle("Maodie Adventure");
    setupUi();
    setupConnections();
    return true;
}

void MainWindow::setupUi() {
    resize(800, 600);
    setMinimumSize(600, 400);
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_stackedWidget = new QStackedWidget(this);
    
    m_menuView = new MenuView(this);
    m_menuView->initialize();
    
    m_stackedWidget->addWidget(m_menuView);
    
    m_stackedWidget->setCurrentWidget(m_menuView);
    
    m_layout->addWidget(m_stackedWidget);
    
    setLayout(m_layout);
}

void MainWindow::setupConnections() {
    if (m_menuView) {
        connect(m_menuView, &MenuView::startGameRequested, this, &MainWindow::onStartGameRequested);
        connect(m_menuView, &MenuView::exitGameRequested, this, &MainWindow::onExitGameRequested);
    }
}

void MainWindow::onStartGameRequested() {
}

void MainWindow::onExitGameRequested() {
    QApplication::quit();
}


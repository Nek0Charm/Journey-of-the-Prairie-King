#include "view/MainWindow.h"
#include "view/GameWidget.h"
#include <qapplication.h>
#include <QScreen>
#include <QPixmap>
#include <QPainter>
#include <QtWidgets>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    m_gameWidget = new GameWidget(this); // 创建 GameWidget，将 MainWindow 设置为其父对象
    this->setCentralWidget(m_gameWidget); // 将其设置为中心部件
    auto t = (QMainWindow*)this;
    t->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    QWidget *widget = new QWidget(this);
    // QTimeLine timeline=new QTimeLine(1000);
    // timeLine->setFrameRange(0, 100);
    // connect(timeline,SIGNAL(frameChanged(int)),this,SLOT(yourobjslot(int)));
    // timeline->start();
}

MainWindow::~MainWindow() {
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QMainWindow::paintEvent(event); 
    QPainter painter(this); 
    drawMap(painter);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        showNormal();
    }
    update();
}



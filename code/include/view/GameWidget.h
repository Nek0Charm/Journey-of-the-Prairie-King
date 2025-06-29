#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <iostream>
#include <QKeyEvent>
#include "view/GameMap.h"
#include "view/Animation.h"
#include "view/SpriteManager.h"
#include "view/Entity.h"
#include "viewmodel/GameViewModel.h"
class GameWidget : public QWidget {
    Q_OBJECT

public:
    GameWidget(GameViewModel *viewModel, QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void paintMap(QPainter *painter);
    void paintUi(QPainter *painter);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    // void onStateUpdated();
private slots:
    void gameLoop(); // 临时函数
    void playerPositionChanged();

private:
    QTimer* m_timer;            // 临时变量
    QElapsedTimer m_elapsedTimer; // 临时变量
    GameMap* m_gameMap;
    QPixmap m_spriteSheet;
    Entity* player;
    QList<Entity*> m_entity;
    GameViewModel *m_viewModel;      
    double m_maxTime;      
    double m_currentTime;  
};

#endif

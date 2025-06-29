#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <iostream>
#include "view/GameMap.h"
#include "view/Animation.h"
#include "view/SpriteManager.h"
#include "view/Entity.h"
class GameWidget : public QWidget {
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void paintMap(QPainter *painter);
    void paintUi(QPainter *painter);

public slots:
    // void onStateUpdated();
private slots:
    void gameLoop(); // 临时函数

private:
    QTimer* m_timer;            // 临时变量
    QElapsedTimer m_elapsedTimer; // 临时变量
    GameMap* m_gameMap;
    QPixmap m_spriteSheet;
    QList<Entity*> m_entity;
    // Animation* m_animation;      
    double m_maxTime;      
    double m_currentTime;  
};

#endif

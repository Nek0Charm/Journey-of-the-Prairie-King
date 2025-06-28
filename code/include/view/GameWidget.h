#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "view/GameMap.h"
#include "view/Animation.h"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void paintMap(QPainter *painter);

private slots:
    void gameLoop(); // 游戏主循环

private:
    QTimer* m_timer;            // 驱动游戏循环的定时器
    QElapsedTimer m_elapsedTimer; // 用于计算帧之间的时间差 (deltaTime)
    GameMap* m_gameMap;
    QPixmap m_spriteSheet;      // 雪碧图
    Animation* m_playerWalk;      // 玩家的行走动画实例
};

#endif // GAMEWIDGET_H
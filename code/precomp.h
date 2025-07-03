#ifndef __PRECOMP_H__
#define __PRECOMP_H__

// Qt Core
#include <QObject>
#include <QDebug>
#include <QString>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QTimer>
#include <QElapsedTimer>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QUrl>

// Qt Widgets
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QScreen>
#include <QMainWindow>
#include <QColor>
#include <QRect>
#include <QRandomGenerator>
#include <QPainter>

// Qt GUI
#include <QMovie>

// Qt Multimedia
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

#include <QList>
#include <QRandomGenerator>
#include <QMap>

#define MAP_WIDTH 256
#define MAP_HEIGHT 256

#define MAX_GAMETIME 60.0

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

struct BulletData {
    int id;
    QPointF position;
    QPointF velocity;
    bool isActive; // 是否处于活动状态
};

struct EnemyData {
    int id;
    int health = 1;
    QPointF position;
    QPointF velocity;
    double moveSpeed = 40.0;
    bool isActive = true;
};

struct ItemData {
    int type;
    int id;
    QPoint position;
    bool isPossessed;
    bool isActive;
    double remainTime;
};

#endif
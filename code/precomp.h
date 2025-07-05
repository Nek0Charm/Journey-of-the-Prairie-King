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

// Spikeball动画状态枚举
enum class SpikeballAnimationState {
    Walking,        // 行走
    WaitingToDeploy, // 准备部署
    Deploying,      // 部署中
    Deployed        // 部署完成
};

struct BulletData {
    int id;
    QPointF position;
    QPointF velocity;
    bool isActive; // 是否处于活动状态
    int damage;    // 子弹伤害值，用于穿透性功能
};

struct EnemyData {
    int id;
    int health = 1;
    QPointF position;
    QPointF velocity;
    double moveSpeed = 40.0;
    bool isActive = true;
    bool isSmart = true;
    
    // 新增：敌人类型
    int enemyType = 0; // 0=普通兽人, 1=Spikeball, 2=Ogre
    
    // 新增：动画状态，由ViewModel层设置，View层直接使用
    int animationState = 0; // 0=行走, 1=准备部署, 2=部署中, 3=部署完成
    
    // Spikeball特有属性
    bool isDeployed = false; // 是否已部署
    double deployTimer = 0.0; // 部署计时器
    double deployDelay = 3.0; // 部署延迟时间
    QPointF targetPosition; // 目标部署位置
    bool hasReachedTarget = false; // 是否已到达目标位置
    
    // Ogre特有属性
    double damageResistance = 0.5; // 伤害抗性 (0.5表示只受50%伤害)
    
    // 障碍物相关
    bool hasCreatedObstacle = false; // 是否已创建障碍物
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
#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <iostream>
#include "view/GameMap.h"
#include "view/Animation.h"
#include "view/SpriteManager.h"
#include "view/Entity.h"
class GameWidget : public QWidget {
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);
    void clearKeys() {keys.clear();};
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void paintUi(QPainter *painter, const QPointF& viewOffset);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent();
    void syncEnemies();
    void syncItems();

signals:
    void setMovingDirection(QPointF direction, bool isMoving);
    void shoot(QPointF direction);
    void useItem();


public slots:
    // void onStateUpdated();
    void die(int id);
    void gameLoop(); // 临时函数
    void playerPositionChanged(QPointF position);
    void playerLivesChanged();
    // GameViewModel的游戏时间是已游玩时间，而GameWidget的游戏时间是剩余时间
    void updateGameTime(double gameTime);
    void updateBullets(QList<BulletData> bullets);
    void updateEnemies(QList<EnemyData> enemies);
    void updateItems(QList<ItemData> items);
    void updatePlayerStealthMode(bool isStealth);
    void updatePlayerHealth(int health);
    void updatePlayerMoney(int money);
    void updatePossessedItem(int itemType, bool hasItem); 
    void updateZombieMode(bool isZombieMode);

private:
    QTimer* keyRespondTimer;
    QMap<int, bool> keys;
    QTimer* m_timer;            // 临时变量
    QElapsedTimer m_elapsedTimer; // 临时变量
    GameMapView* m_gameMap;
    QPixmap m_spriteSheet;
    PlayerEntity* player;
    QMap<int, MonsterEntity*> m_monsters; 
    QMap<int, DeadMonsterEntity*> m_deadmonsters;
    QMap<int, ItemEntity*> m_items;


    /*
    这些变量需要随着GameViewModel内值的变化而变化
    */
    QList<BulletData> m_bullets; // 存储子弹数据
    QList<EnemyData> m_enemyDataList;
    QList<ItemData> m_itemDataList;
    bool m_playerStealthMode;
    double m_maxTime;      
    double m_currentTime;
    int m_healthCount = 4;
    int m_moneyCount = 0;
    int m_possessedItemType;
    bool m_hasPossessedItem;
    bool m_isZombieMode = false;
    
    // 道具使用相关
    bool m_spaceKeyPressed = false;  // 防止空格键重复触发  
};

#endif

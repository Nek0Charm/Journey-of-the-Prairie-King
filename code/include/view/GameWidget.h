#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

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
    
    // 设置可购买的供应商物品列表（通过信号槽机制）
    void setAvailableVendorItems(const QList<int>& items);

protected:
    void paintEvent(QPaintEvent *event) override;
    void paintUi(QPainter *painter, const QPointF& viewOffset);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent();
    void syncEnemies();
    void syncItems();
    void playerLivesDown();
    void updateSmoke(double deltaTime);
    void updateExplosion(double deltaTime);
    void startExplosionSequence(double duration);
    void releaseSmoke(double duration);

signals:
    void setMovingDirection(QPointF direction, bool isMoving);
    void shoot(QPointF direction);
    void useItem();
    void vendorAppear();
    void vendorDisappear();
    void purchaseVendorItem(int itemType);  // 购买供应商物品的信号
    
    void gameWin(); // 游戏胜利信号
    void pauseGame();
    void resumeGame();

public slots:
    // void onStateUpdated();
    void die(int id);
    void gameLoop(); // 临时函数
    void playerPositionChanged(QPointF position);
    void startMapTransition(const QString& nextMapName, const QString& nextLayoutName);
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
    void updateStealthMode(bool isStealth);
    void updateItemEffect(int itemType);
    void onVendorAppear();
    void onVendorDisappear();
    void onVendorAppeared();      // 供应商出现时的槽
    void onVendorDisappeared();   // 供应商消失时的槽
    void onVendorItemPurchased(int itemType);  // 供应商物品购买成功时的槽
    
    // 更新供应商可购买物品列表
    void updateVendorItems();
    void onGameWin();

private:
    QTimer* keyRespondTimer;
    QMap<int, bool> keys;
    QTimer* m_timer;            // 临时变量
    QElapsedTimer m_elapsedTimer; // 临时变量
    GameMapView* m_gameMap;
    GameMapView* m_nextMap = nullptr;
    QPixmap m_spriteSheet;
    PlayerEntity* player;
    VendorEntity* vendor;
    QMap<int, MonsterEntity*> m_monsters; 
    QMap<int, DeadMonsterEntity*> m_deadmonsters;
    QMap<int, ItemEntity*> m_items;

    bool m_isTransitioning;
    double m_transitionDuration;
    double m_transitionTimer;
    QPointF m_transitionStartOffset;
    QPointF m_transitionEndOffset;
    bool m_isGamePaused = false;
    bool m_isExplosionSequenceActive = false;   
    double m_explosionSequenceTimer = 0.0;    
    double m_nextExplosionSpawnTimer = 0.0; 
    bool m_isSmokeReleased = false;
    double m_smokeReleaseTimer = 0.0;
    double m_nextSmokeReleaseTimer = 0.0;
    double m_pausedTime = 0.0;
    void triggerLightning(const QPointF& startPosition);
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
    
    // 道具使用相关
    bool m_pauseKeyPressed = false;
    bool m_spaceKeyPressed = false;  // 防止空格键重复触发  
    bool m_isZombieMode = false;
    bool m_isBoomActive = false;
    bool m_isStealthMode = false;
    double m_lightningEffectTimer = 0.0;
    QList<QPointF> m_lightningSegments;
    
    // 供应商相关
    QList<int> m_availableVendorItems;  // 当前可购买的供应商物品列表
    
    // 辅助函数：将EnemyData的enemyType转换为MonsterType
    MonsterType enemyTypeToMonsterType(int enemyType);
};

#endif

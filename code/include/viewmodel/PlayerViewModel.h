#ifndef __PLAYER_VIEW_MODEL_H__
#define __PLAYER_VIEW_MODEL_H__

#include <QObject>
#include <QPointF>
#include <QList>
#include <memory>
#include "viewmodel/BulletViewModel.h"

class PlayerViewModel : public QObject {
    Q_OBJECT
    
public:
    struct PlayerStats {
        int lives = 4;
        int coins = 0;
        QPointF position = QPointF(128, 128);
        QPointF movingDirection = QPointF(0, 0);
        QPointF shootingDirection = QPointF(1, 0);
        bool moving = false;
        double moveSpeed = 100.0;
        double shootCooldown = 0.3;
        bool wheelMode = false;  // 8方向射击模式
        bool shotgunMode = false;  // 霰弹枪模式
        bool stealthMode = false;  // 潜行模式
        bool zombieMode = false;   // 僵尸模式
    };

    
    explicit PlayerViewModel(QObject *parent = nullptr);
    
    // 基础操作
    void move(double deltaTime);
    void shoot(const QPointF& direction);
    void addLife();
    void takeDamage();
    void update(double deltaTime);
    void reset();
    
    
    // 状态查询
    const PlayerStats& getStats() const { return m_stats; }
    bool canShoot() const { return m_currentShootCooldown <= 0; }
    QPointF getPosition() const { return m_stats.position; }
    int getLives() const { return m_stats.lives; }
    int getCoins() const { return m_stats.coins; }
    double getShootCooldown() const { return m_stats.shootCooldown; }
    double getMoveSpeed() const { return m_stats.moveSpeed; }
    QPointF getShootingDirection() const { return m_stats.shootingDirection; }
    BulletViewModel* getBulletViewModel() const { return m_bulletViewModel.get(); }
    QList<BulletData> getActiveBullets() const {
        return m_bulletViewModel->getActiveBullets();
    }
    
    void setMovingDirection(const QPointF& direction, bool isMoving = true) {
        double length = std::hypot(direction.x(), direction.y());
        m_stats.movingDirection = direction / (length > 0 ? length : 1.0);
        m_stats.moving = isMoving;
    }

    void removeBullet(int bulletId) { m_bulletViewModel->removeBullet(bulletId);}

    void setPositon(const QPointF& position) { m_stats.position = position; emit positionChanged(m_stats.position);}
    
    // 传送方法
    void teleportToRandomPosition();

    void setMoveSpeed(double speed) {m_stats.moveSpeed = speed;}

    void setShootCooldown(double cooldown) {m_stats.shootCooldown = cooldown;}
    void addCoins(int amount) {m_stats.coins += amount; emit coinsChanged(m_stats.coins);}
    
    // 轮子模式相关
    void setWheelMode(bool enabled) {m_stats.wheelMode = enabled;}
    bool isWheelMode() const {return m_stats.wheelMode;}
    
    // 霰弹枪模式相关
    void setShotgunMode(bool enabled) {m_stats.shotgunMode = enabled;}
    bool isShotgunMode() const {return m_stats.shotgunMode;}
    
    // 潜行模式相关
    void setStealthMode(bool enabled) {
        m_stats.stealthMode = enabled; 
        emit playerStealthModeChanged(enabled);
    }
    bool isStealthMode() const {return m_stats.stealthMode;}
    
    // 僵尸模式相关
    void setZombieMode(bool enabled) {m_stats.zombieMode = enabled; emit zombieModeChanged(enabled);}
    bool isZombieMode() const {return m_stats.zombieMode;}

signals:
    void playerDied();
    void livesChanged();
    void livesDown();
    void coinsChanged(int coins);
    void positionChanged(const QPointF& position);
    void shot(const QPointF& direction);
    void playerStealthModeChanged(bool enabled);
    void healthChanged(int health);
    void zombieModeChanged(bool enabled);
    
private:
    PlayerStats m_stats;
    double m_currentShootCooldown = 0.0;
    std::unique_ptr<BulletViewModel> m_bulletViewModel;
    
    void updateShootCooldown(double deltaTime);
    void shootInEightDirections();  // 8方向射击
    void shootInShotgunPattern(const QPointF& direction);  // 霰弹枪射击
};

#endif
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
        QPointF position = QPointF(128, 128);
        QPointF movingDirection = QPointF(0, 0);
        QPointF shootingDirection = QPointF(1, 0);
        bool moving = false;
        double moveSpeed = 100.0;
        double shootCooldown = 0.3;
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
    double getShootCooldown() const { return m_stats.shootCooldown; }
    double getMoveSpeed() const { return m_stats.moveSpeed; }
    QPointF getShootingDirection() const { return m_stats.shootingDirection; }
    
    QList<BulletViewModel::BulletData> getActiveBullets() const {
        return m_bulletViewModel->getActiveBullets();
    }
    
    void setMovingDirection(const QPointF& direction, bool isMoving = true) {
        double length = std::hypot(direction.x(), direction.y());
        m_stats.movingDirection = direction / (length > 0 ? length : 1.0);
        m_stats.moving = isMoving;
    }

    void removeBullet(int bulletId) { m_bulletViewModel->removeBullet(bulletId);}

    void setPositon(const QPointF& position) { m_stats.position = position; emit positionChanged(m_stats.position);}

    void setMoveSpeed(double speed) {m_stats.moveSpeed = speed;}

    void setShootCooldown(double cooldown) {m_stats.shootCooldown = cooldown;}

signals:
    void playerDied();
    void livesChanged();
    void positionChanged(const QPointF& position);
    void shot(const QPointF& direction);
    
    
private:
    PlayerStats m_stats;
    double m_currentShootCooldown = 0.0;
    std::unique_ptr<BulletViewModel> m_bulletViewModel;
    
    void updateShootCooldown(double deltaTime);
};

#endif
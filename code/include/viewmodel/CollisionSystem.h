#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "viewmodel/PlayerViewModel.h"
#include "viewmodel/EnemyManager.h"
#include "viewmodel/BulletViewModel.h"

class CollisionSystem : public QObject
{
    Q_OBJECT

public:
    explicit CollisionSystem(QObject *parent = nullptr);
    ~CollisionSystem() = default;

    // 碰撞检测
    void checkCollisions(const PlayerViewModel& player,
                        const QList<EnemyData>& enemies,
                        const QList<BulletData>& bullets);
    
    void checkPlayerEnemyCollisions(const PlayerViewModel& player,
                                   const QList<EnemyData>& enemies);
    
    void checkBulletEnemyCollisions(const QList<BulletData>& bullets,
                                   const QList<EnemyData>& enemies);

    // 碰撞查询
    bool checkPlayerEnemyCollision(const QPointF& playerPos, 
                                  const QPointF& enemyPos) const;
    bool checkBulletEnemyCollision(const QPointF& bulletPos, 
                                  const QPointF& enemyPos) const;
    bool checkBulletPlayerCollision(const QPointF& bulletPos,
                                   const QPointF& playerPos) const;

    // 配置设置
    void setPlayerCollisionRadius(double radius) { m_playerCollisionRadius = radius; }
    void setEnemyCollisionRadius(double radius) { m_enemyCollisionRadius = radius; }
    void setBulletCollisionRadius(double radius) { m_bulletCollisionRadius = radius; }

    // 获取配置
    double getPlayerCollisionRadius() const { return m_playerCollisionRadius; }
    double getEnemyCollisionRadius() const { return m_enemyCollisionRadius; }
    double getBulletCollisionRadius() const { return m_bulletCollisionRadius; }

signals:
    void playerHitByEnemy(int enemyId);
    void enemyHitByBullet(int bulletId, int enemyId);
    void enemyHitByZombie(int enemyId);  // 僵尸模式接触击杀

private:
    double m_playerCollisionRadius = 9.0;
    double m_enemyCollisionRadius = 9.0;
    double m_bulletCollisionRadius = 3.0;

    double calculateDistance(const QPointF& pos1, const QPointF& pos2) const;
    bool isCollision(const QPointF& pos1, const QPointF& pos2, double radius1, double radius2) const;
    void logCollision(const QString& type, int id1, int id2);
};

#endif // COLLISIONSYSTEM_H 
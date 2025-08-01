#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

class PlayerViewModel;
class EnemyManager;
class BulletViewModel;

class CollisionSystem : public QObject
{
    Q_OBJECT

public:
    explicit CollisionSystem(QObject *parent = nullptr);
    static CollisionSystem& instance();
    ~CollisionSystem() = default;

    // 碰撞检测
    void checkCollisions(const PlayerViewModel& player,
                        const QList<EnemyData>& enemies,
                        const QList<BulletData>& bullets);
    
    void checkPlayerEnemyCollisions(const PlayerViewModel& player,
                                   const QList<EnemyData>& enemies);
    
    void checkBulletEnemyCollisions(const QList<BulletData>& bullets,
                                   const QList<EnemyData>& enemies,
                                   int bulletDamage = 1);

    // 碰撞查询
    bool checkPlayerEnemyCollision(const QPointF& playerPos, 
                                  const QPointF& enemyPos) const;
    bool checkBulletEnemyCollision(const QPointF& bulletPos, 
                                  const QPointF& enemyPos) const;
    bool checkBulletPlayerCollision(const QPointF& bulletPos,
                                   const QPointF& playerPos) const;

    // 配置设置
    void setPlayerCollisionRadius(double radius) { m_playerWidth = radius; }
    void setEnemyCollisionRadius(double radius) { m_enemyWidth = radius; }
    void setBulletCollisionRadius(double radius) { m_bulletWidth = radius; }

    // 获取配置
    double getPlayerCollisionRadius() const { return m_playerWidth; }
    double getEnemyCollisionRadius() const { return m_enemyWidth; }
    double getBulletCollisionRadius() const { return m_bulletWidth; }
    bool isCollision(const QPointF& pos1, const QPointF& pos2, double radius1, double radius2) const;
    // 点与地图瓦片碰撞检测
    bool isPointInWalkableTile(const QPointF& point) const;
    // 矩形与地图碰撞检测
    bool isRectCollidingWithMap(const QPointF& position, int size) const;
    

signals:
    void playerHitByEnemy(int enemyId);
    void enemyHitByBullet(int bulletId, int enemyId);
    void enemyHitByZombie(int enemyId);  // 僵尸模式接触击杀

private:
    double m_playerWidth = 16.0;
    double m_enemyWidth = 16.0;
    double m_bulletWidth = 5.0;

    double calculateDistance(const QPointF& pos1, const QPointF& pos2) const;
    void logCollision(const QString& type, int id1, int id2);
};

#endif // COLLISIONSYSTEM_H 
#include "viewmodel/CollisionSystem.h"

CollisionSystem::CollisionSystem(QObject *parent)
    : QObject(parent)
{
}

void CollisionSystem::checkCollisions(const PlayerViewModel& player,
                                    const QList<EnemyData>& enemies,
                                    const QList<BulletData>& bullets)
{
    // 检查玩家与敌人的碰撞
    checkPlayerEnemyCollisions(player, enemies);
    
    // 检查子弹与敌人的碰撞
    checkBulletEnemyCollisions(bullets, enemies);
}

void CollisionSystem::checkPlayerEnemyCollisions(const PlayerViewModel& player,
                                               const QList<EnemyData>& enemies)
{
    QPointF playerPos = player.getStats().position;
    bool isZombieMode = player.isZombieMode();
    
    for (const auto& enemy : enemies) {
        if (enemy.isActive && checkPlayerEnemyCollision(playerPos, enemy.position)) {
            if (isZombieMode) {
                // 僵尸模式：接触击杀敌人
                emit enemyHitByZombie(enemy.id);
                logCollision("Zombie-Enemy", -1, enemy.id);
            } else {
                // 正常模式：玩家被敌人击中
                emit playerHitByEnemy(enemy.id);
                logCollision("Player-Enemy", -1, enemy.id);
            }
            break; // 玩家只能被一个敌人击中
        }
    }
}

void CollisionSystem::checkBulletEnemyCollisions(const QList<BulletData>& bullets,
                                               const QList<EnemyData>& enemies)
{
    for (const auto& bullet : bullets) {
        if (!bullet.isActive) continue;
        
        for (const auto& enemy : enemies) {
            if (enemy.isActive && checkBulletEnemyCollision(bullet.position, enemy.position)) {
                emit enemyHitByBullet(bullet.id, enemy.id);
                logCollision("Bullet-Enemy", bullet.id, enemy.id);
                break;
            }
        }
    }
}

bool CollisionSystem::checkPlayerEnemyCollision(const QPointF& playerPos, 
                                              const QPointF& enemyPos) const
{
    return isCollision(playerPos, enemyPos, m_playerCollisionRadius, m_enemyCollisionRadius);
}

bool CollisionSystem::checkBulletEnemyCollision(const QPointF& bulletPos, 
                                              const QPointF& enemyPos) const
{
    return isCollision(bulletPos, enemyPos, m_bulletCollisionRadius, m_enemyCollisionRadius);
}

bool CollisionSystem::checkBulletPlayerCollision(const QPointF& bulletPos,
                                               const QPointF& playerPos) const
{
    return isCollision(bulletPos, playerPos, m_bulletCollisionRadius, m_playerCollisionRadius);
}

double CollisionSystem::calculateDistance(const QPointF& pos1, const QPointF& pos2) const
{
    return std::sqrt(std::pow(pos2.x() - pos1.x(), 2) + std::pow(pos2.y() - pos1.y(), 2));
}

bool CollisionSystem::isCollision(const QPointF& pos1, const QPointF& pos2, 
                                 double radius1, double radius2) const
{
    double distance = calculateDistance(pos1, pos2);
    double combinedRadius = radius1 + radius2;
    
    return distance <= combinedRadius;
}

void CollisionSystem::logCollision(const QString& type, int id1, int id2)
{
    qDebug() << "Collision detected:" << type << "between" << id1 << "and" << id2;
} 
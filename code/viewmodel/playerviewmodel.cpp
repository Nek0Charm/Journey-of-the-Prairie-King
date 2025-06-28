#include "viewmodel/PlayerViewModel.h"
#include <QDebug>
#include <cmath>

PlayerViewModel::PlayerViewModel(QObject *parent)
    : QObject(parent)
    , m_nextBulletId(0)
    , m_currentShootCooldown(0.0)
    , m_bulletSpeed(200.0)
{
    reset();
}

void PlayerViewModel::move(const QPointF& direction)
{
    if (!direction.isNull()) {
        QPointF oldPosition = m_stats.position;
        m_stats.position += direction * m_stats.moveSpeed * 0.016; // 假设60FPS
        
        // 边界检查
        const int screenWidth = 800;
        const int screenHeight = 600;
        const int margin = 20;
        
        m_stats.position.setX(qBound(margin, m_stats.position.x(), screenWidth - margin));
        m_stats.position.setY(qBound(margin, m_stats.position.y(), screenHeight - margin));
        
        if (oldPosition != m_stats.position) {
            emit positionChanged(m_stats.position);
            emit statsChanged(m_stats);
        }
    }
}

void PlayerViewModel::shoot(const QPointF& direction)
{
    if (canShoot() && !direction.isNull()) {
        m_stats.shootingDirection = direction;
        createBullet(m_stats.position, direction);
        m_currentShootCooldown = m_stats.shootCooldown;
        
        emit bulletFired(m_stats.position, direction);
        emit statsChanged(m_stats);
    }
}

void PlayerViewModel::update(double deltaTime)
{
    updateShootCooldown(deltaTime);
    updateBullets(deltaTime);
}

void PlayerViewModel::updateBullets(double deltaTime)
{
    updateBulletPositions(deltaTime);
    removeExpiredBullets();
}

void PlayerViewModel::takeDamage()
{
    m_stats.lives--;
    emit livesChanged(m_stats.lives);
    emit statsChanged(m_stats);
    
    if (m_stats.lives <= 0) {
        emit playerDied();
    }
}

void PlayerViewModel::addLife()
{
    m_stats.lives++;
    emit livesChanged(m_stats.lives);
    emit statsChanged(m_stats);
}

void PlayerViewModel::addCoins(int amount)
{
    m_stats.coins += amount;
    emit coinsChanged(m_stats.coins);
    emit statsChanged(m_stats);
}

void PlayerViewModel::reset()
{
    m_stats.lives = 4;
    m_stats.coins = 0;
    m_stats.position = QPointF(400, 300);
    m_stats.shootingDirection = QPointF(1, 0);
    m_stats.moveSpeed = 100.0;
    m_stats.shootCooldown = 0.2;
    m_stats.bulletDamage = 1.0;
    m_stats.hasPenetration = false;
    
    clearAllBullets();
    m_currentShootCooldown = 0.0;
    
    emit statsChanged(m_stats);
    emit livesChanged(m_stats.lives);
    emit coinsChanged(m_stats.coins);
    emit positionChanged(m_stats.position);
}

void PlayerViewModel::createBullet(const QPointF& position, const QPointF& direction)
{
    BulletData bullet;
    bullet.id = m_nextBulletId++;
    bullet.position = position;
    bullet.velocity = direction * m_bulletSpeed;
    bullet.damage = m_stats.bulletDamage;
    bullet.canPenetrate = m_stats.hasPenetration;
    bullet.isActive = true;
    bullet.lifetime = 0.0;
    bullet.maxLifetime = 5.0;
    
    m_bullets.append(bullet);
    
    emit bulletCreated(bullet);
}

void PlayerViewModel::removeBullet(int bulletId)
{
    for (int i = 0; i < m_bullets.size(); ++i) {
        if (m_bullets[i].id == bulletId) {
            m_bullets.removeAt(i);
            emit bulletDestroyed(bulletId);
            break;
        }
    }
}

void PlayerViewModel::clearAllBullets()
{
    m_bullets.clear();
}

void PlayerViewModel::updateShootCooldown(double deltaTime)
{
    if (m_currentShootCooldown > 0) {
        m_currentShootCooldown -= deltaTime;
        if (m_currentShootCooldown < 0) {
            m_currentShootCooldown = 0;
        }
    }
}

void PlayerViewModel::updateBulletPositions(double deltaTime)
{
    for (auto& bullet : m_bullets) {
        if (bullet.isActive) {
            bullet.position += bullet.velocity * deltaTime;
            bullet.lifetime += deltaTime;
        }
    }
}

void PlayerViewModel::removeExpiredBullets()
{
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (!m_bullets[i].isActive || 
            m_bullets[i].lifetime >= m_bullets[i].maxLifetime ||
            isBulletOutOfBounds(m_bullets[i])) {
            emit bulletDestroyed(m_bullets[i].id);
            m_bullets.removeAt(i);
        }
    }
}

bool PlayerViewModel::isBulletOutOfBounds(const BulletData& bullet) const
{
    const int margin = 50;
    return bullet.position.x() < -margin || bullet.position.x() > 850 ||
           bullet.position.y() < -margin || bullet.position.y() > 650;
}

double PlayerViewModel::calculateDistance(const QPointF& pos1, const QPointF& pos2) const
{
    return std::sqrt(std::pow(pos2.x() - pos1.x(), 2) + std::pow(pos2.y() - pos1.y(), 2));
}

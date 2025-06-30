#include "viewmodel/PlayerViewModel.h"
#include <QDebug>
#include <cmath>

PlayerViewModel::PlayerViewModel(QObject *parent)
    : QObject(parent)
{
    m_bulletViewModel = std::make_unique<BulletViewModel>(this);
    reset();
}

void PlayerViewModel::move(double deltaTime)
{
    QPointF orig_pos = m_stats.position;

    if(m_stats.moving)
        m_stats.position += m_stats.movingDirection * m_stats.moveSpeed * deltaTime;
    m_stats.position.setX(std::clamp(m_stats.position.x(), 16.0, static_cast<double>(MAP_WIDTH)-16.0));
    m_stats.position.setY(std::clamp(m_stats.position.y(), 16.0, static_cast<double>(MAP_HEIGHT)-16.0));
    if(orig_pos != m_stats.position) {
        emit positionChanged(m_stats.position);
    }
}

void PlayerViewModel::shoot(const QPointF& direction)
{
    if (canShoot() && !direction.isNull()) {
        m_stats.shootingDirection = direction;
        m_bulletViewModel->createBullet(m_stats.position, direction, 300);
        m_currentShootCooldown = m_stats.shootCooldown;
    }
}

void PlayerViewModel::update(double deltaTime) {
    move(deltaTime);
    updateShootCooldown(deltaTime);
    m_bulletViewModel->updateBullets(deltaTime);
}

void PlayerViewModel::takeDamage()
{
    m_stats.lives--;
    emit livesChanged();    
    if (m_stats.lives < 0) {
        emit playerDied();
    }
}

void PlayerViewModel::addLife()
{
    m_stats.lives++;
    emit livesChanged();
}

void PlayerViewModel::reset()
{
    m_stats.lives = 4;
    m_stats.position = QPointF(MAP_WIDTH/2, MAP_HEIGHT/2);
    m_stats.shootingDirection = QPointF(1, 0);
    m_stats.moveSpeed = 100.0;
    m_stats.shootCooldown = 0.2;
    
    m_bulletViewModel->clearAllBullets();
    m_currentShootCooldown = 0.0;
    
}


void PlayerViewModel::updateShootCooldown(double deltaTime)
{
    m_currentShootCooldown = std::max(m_currentShootCooldown-deltaTime, 0.0);
}

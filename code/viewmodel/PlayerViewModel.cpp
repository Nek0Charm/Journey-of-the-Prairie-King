#include "viewmodel/PlayerViewModel.h"
#include <QDebug>
#include <QVector>
#include "../../precomp.h"

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
    m_stats.position.setX(std::clamp(m_stats.position.x(), 16.0, static_cast<double>(MAP_WIDTH)-32.0));
    m_stats.position.setY(std::clamp(m_stats.position.y(), 16.0, static_cast<double>(MAP_HEIGHT)-32.0));
    if(orig_pos != m_stats.position) {
        emit positionChanged(m_stats.position);
    }
}

void PlayerViewModel::shoot(const QPointF& direction)
{
    if (canShoot() && !direction.isNull()) {
        m_stats.shootingDirection = direction;
        
        if (m_stats.wheelMode) {
            // 8方向射击模式：向8个方向发射子弹
            shootInEightDirections();
        } else if (m_stats.shotgunMode) {
            // 霰弹枪模式：向指定方向及其相邻方向发射子弹
            shootInShotgunPattern(direction);
        } else {
            // 正常射击模式：只向指定方向发射一颗子弹
            m_bulletViewModel->createBullet(m_stats.position, direction, 200);
        }
        
        m_currentShootCooldown = m_stats.shootCooldown;
        emit shot(direction);
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
    emit healthChanged(m_stats.lives);    
    if (m_stats.lives < 0) {
        emit playerDied();
    }
}

void PlayerViewModel::addLife()
{
    m_stats.lives++;
    emit healthChanged(m_stats.lives);
    emit livesChanged();
}

void PlayerViewModel::reset()
{
    m_stats.lives = 4;
    m_stats.coins = 0;
    m_stats.position = QPointF(MAP_WIDTH/2, MAP_HEIGHT/2);
    m_stats.shootingDirection = QPointF(1, 0);
    m_stats.moveSpeed = 80.0;
    m_stats.shootCooldown = 0.2;
    
    m_bulletViewModel->clearAllBullets();
    m_currentShootCooldown = 0.0;
    
}


void PlayerViewModel::updateShootCooldown(double deltaTime)
{
    m_currentShootCooldown = std::max(m_currentShootCooldown-deltaTime, 0.0);
}

void PlayerViewModel::shootInEightDirections()
{
    // 8个方向：上、右上、右、右下、下、左下、左、左上
    QVector<QPointF> directions = {
        QPointF(0, -1),    // 上
        QPointF(1, -1),    // 右上
        QPointF(1, 0),     // 右
        QPointF(1, 1),     // 右下
        QPointF(0, 1),     // 下
        QPointF(-1, 1),    // 左下
        QPointF(-1, 0),    // 左
        QPointF(-1, -1)    // 左上
    };
    
    // 向8个方向发射子弹
    for (const auto& direction : directions) {
        m_bulletViewModel->createBullet(m_stats.position, direction, 200);
    }
    
    qDebug() << "8方向射击：向8个方向发射子弹";
}

void PlayerViewModel::shootInShotgunPattern(const QPointF& direction)
{
    // 霰弹枪模式：向指定方向及其相邻方向发射子弹
    QVector<QPointF> directions;
    
    // 确定主要方向
    QPointF mainDir = direction;
    
    // 根据主要方向确定相邻方向
    if (mainDir == QPointF(0, -1)) {  // 上
        directions = {
            QPointF(0, -1),    // 上
            QPointF(1, -1),    // 右上
            QPointF(-1, -1)    // 左上
        };
    } else if (mainDir == QPointF(1, 0)) {  // 右
        directions = {
            QPointF(1, 0),     // 右
            QPointF(1, -1),    // 右上
            QPointF(1, 1)      // 右下
        };
    } else if (mainDir == QPointF(0, 1)) {  // 下
        directions = {
            QPointF(0, 1),     // 下
            QPointF(1, 1),     // 右下
            QPointF(-1, 1)     // 左下
        };
    } else if (mainDir == QPointF(-1, 0)) {  // 左
        directions = {
            QPointF(-1, 0),    // 左
            QPointF(-1, -1),   // 左上
            QPointF(-1, 1)     // 左下
        };
    } else if (mainDir == QPointF(1, -1)) {  // 右上
        directions = {
            QPointF(1, -1),    // 右上
            QPointF(0, -1),    // 上
            QPointF(1, 0)      // 右
        };
    } else if (mainDir == QPointF(1, 1)) {  // 右下
        directions = {
            QPointF(1, 1),     // 右下
            QPointF(1, 0),     // 右
            QPointF(0, 1)      // 下
        };
    } else if (mainDir == QPointF(-1, 1)) {  // 左下
        directions = {
            QPointF(-1, 1),    // 左下
            QPointF(-1, 0),    // 左
            QPointF(0, 1)      // 下
        };
    } else if (mainDir == QPointF(-1, -1)) {  // 左上
        directions = {
            QPointF(-1, -1),   // 左上
            QPointF(-1, 0),    // 左
            QPointF(0, -1)     // 上
        };
    } else {
        // 如果不是标准方向，就只发射一颗子弹
        directions = {mainDir};
    }
    
    // 向确定的方向发射子弹
    for (const auto& dir : directions) {
        m_bulletViewModel->createBullet(m_stats.position, dir, 200);
    }
    
    qDebug() << "霰弹枪射击：向" << directions.size() << "个方向发射子弹";
}

void PlayerViewModel::teleportToRandomPosition()
{
    // 生成随机位置，避开边界
    double margin = 50.0;
    double x = QRandomGenerator::global()->bounded(static_cast<int>(margin), static_cast<int>(MAP_WIDTH - margin));
    double y = QRandomGenerator::global()->bounded(static_cast<int>(margin), static_cast<int>(MAP_HEIGHT - margin));
    
    QPointF newPosition(x, y);
    setPositon(newPosition);
    
    qDebug() << "传送至随机位置:" << newPosition;
}

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
    emit livesDown();
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
    m_stats.stealthMode = false;
    m_stats.wheelMode = false; 
    m_stats.shotgunMode = false;
    m_stats.zombieMode = false;
    m_stats.moving = false;


    m_bulletViewModel->clearAllBullets();
    m_currentShootCooldown = 0.0;
    emit positionChanged(m_stats.position);
    emit healthChanged(m_stats.lives);
    emit coinsChanged(m_stats.coins);
    emit playerStealthModeChanged(m_stats.stealthMode);
    emit zombieModeChanged(m_stats.zombieMode);
    
    
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
    // 霰弹枪模式：向指定方向及其左右30度发射子弹
    QVector<QPointF> directions;
    
    // 计算角度（以弧度为单位）
    double angle = std::atan2(direction.y(), direction.x());
    double angleOffset = M_PI / 6.0; // 30度 = π/6弧度
    
    // 计算三个方向：左30度、中心、右30度
    double leftAngle = angle - angleOffset;
    double centerAngle = angle;
    double rightAngle = angle + angleOffset;
    
    // 转换为单位向量
    QPointF leftDir(std::cos(leftAngle), std::sin(leftAngle));
    QPointF centerDir(std::cos(centerAngle), std::sin(centerAngle));
    QPointF rightDir(std::cos(rightAngle), std::sin(rightAngle));
    
    directions = {leftDir, centerDir, rightDir};
    
    // 向确定的方向发射子弹
    for (const auto& dir : directions) {
        m_bulletViewModel->createBullet(m_stats.position, dir, 200);
    }
    
    qDebug() << "霰弹枪射击：向" << directions.size() << "个方向发射子弹（30度扇形）";
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

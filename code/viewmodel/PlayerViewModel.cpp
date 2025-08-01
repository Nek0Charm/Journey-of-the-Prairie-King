#include "viewmodel/PlayerViewModel.h"
#include "common/GameMap.h"
#include "viewmodel/CollisionSystem.h"
#include <QVector>
#include <QRandomGenerator>
#include <QDebug>

PlayerViewModel::PlayerViewModel(QObject *parent)
    : QObject(parent)
{
    m_bulletViewModel = std::make_unique<BulletViewModel>(this);
    reset(true);
}

void PlayerViewModel::move(double deltaTime)
{
    QPointF orig_pos = m_stats.position;
    QPointF movement = QPointF(0, 0);
    if(m_stats.moving)
        movement= m_stats.movingDirection * m_stats.moveSpeed * deltaTime;

    if(!CollisionSystem::instance().isRectCollidingWithMap(QPointF(orig_pos.x() + movement.x(), orig_pos.y()), 14)) {
        m_stats.position.setX(orig_pos.x() + movement.x());
    } 
    if(!CollisionSystem::instance().isRectCollidingWithMap(QPointF(orig_pos.x(), orig_pos.y() + movement.y()), 14)) {
        m_stats.position.setY(orig_pos.y() + movement.y());
    }
    
    m_stats.position.setX(std::clamp(m_stats.position.x(), 16.0, static_cast<double>(MAP_WIDTH)-32.0));
    m_stats.position.setY(std::clamp(m_stats.position.y(), 16.0, static_cast<double>(MAP_HEIGHT)-32.0));

    emit positionChanged(m_stats.position);

}

void PlayerViewModel::shoot(const QPointF& direction)
{
    if (canShoot() && !direction.isNull()) {
        m_stats.shootingDirection = direction;
        
        if (m_stats.wheelMode && m_stats.shotgunMode) {
            // 轮子+霰弹枪组合模式：向24个方向发射子弹
            shootInWheelShotgunCombination();
        } else if (m_stats.badgeMode) {
            // 治安官徽章模式：包含霰弹枪效果，直接使用霰弹枪射击方法
            shootInShotgunPattern(direction);
        } else if (m_stats.wheelMode) {
            // 8方向射击模式：向8个方向发射子弹
            shootInEightDirections();
        } else if (m_stats.shotgunMode) {
            // 霰弹枪模式：向指定方向及其相邻方向发射子弹
            shootInShotgunPattern(direction);
        } else {
            // 正常射击模式：只向指定方向发射一颗子弹
            m_bulletViewModel->createBullet(m_stats.position, direction, 170, m_stats.bulletDamage);
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

void PlayerViewModel::reset(bool resetVendorEffects)
{
    m_stats.lives = 4;
    m_stats.coins = 0;  // 初始金币为0
    m_stats.position = QPointF(MAP_WIDTH/2, MAP_HEIGHT/2);
    m_stats.shootingDirection = QPointF(1, 0);
    
    if (resetVendorEffects) {
        // 重置供应商效果
        m_stats.moveSpeed = 80.0;
        m_stats.shootCooldown = 0.2;
        m_stats.bulletDamage = 1;  // 重置子弹伤害值为1
        m_stats.shotgunMode = false;
    }
    // 临时效果总是重置
    m_stats.stealthMode = false;
    m_stats.wheelMode = false; 
    m_stats.badgeMode = false;
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
        m_bulletViewModel->createBullet(m_stats.position, direction, 170, m_stats.bulletDamage);
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
        m_bulletViewModel->createBullet(m_stats.position, dir, 170, m_stats.bulletDamage);
    }
    
    qDebug() << "霰弹枪射击：向" << directions.size() << "个方向发射子弹";
}

void PlayerViewModel::shootInWheelShotgunCombination()
{
    // 轮子+霰弹枪组合模式：向8个方向发射霰弹枪子弹
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
    
    // 对每个方向应用霰弹枪模式
    for (const auto& mainDirection : directions) {
        // 计算角度（以弧度为单位）
        double angle = std::atan2(mainDirection.y(), mainDirection.x());
        double angleOffset = M_PI / 6.0; // 30度 = π/6弧度
        
        // 计算三个方向：左30度、中心、右30度
        double leftAngle = angle - angleOffset;
        double centerAngle = angle;
        double rightAngle = angle + angleOffset;
        
        // 转换为单位向量
        QPointF leftDir(std::cos(leftAngle), std::sin(leftAngle));
        QPointF centerDir(std::cos(centerAngle), std::sin(centerAngle));
        QPointF rightDir(std::cos(rightAngle), std::sin(rightAngle));
        
        // 向三个方向发射子弹
        m_bulletViewModel->createBullet(m_stats.position, leftDir, 170, m_stats.bulletDamage);
        m_bulletViewModel->createBullet(m_stats.position, centerDir, 170, m_stats.bulletDamage);
        m_bulletViewModel->createBullet(m_stats.position, rightDir, 170, m_stats.bulletDamage);
    }
    
    qDebug() << "轮子+霰弹枪组合射击：向8个方向发射霰弹枪模式子弹，总共" << (8 * 3) << "颗子弹";
}

void PlayerViewModel::teleportToRandomPosition()
{
    // 生成随机位置，避开边界和障碍物
    double margin = 50.0;
    int maxAttempts = 50; // 最大尝试次数
    QPointF newPosition;
    bool validPositionFound = false;
    
    for (int attempt = 0; attempt < maxAttempts && !validPositionFound; ++attempt) {
        double x = QRandomGenerator::global()->bounded(static_cast<int>(margin), static_cast<int>(MAP_WIDTH - margin));
        double y = QRandomGenerator::global()->bounded(static_cast<int>(margin), static_cast<int>(MAP_HEIGHT - margin));
        
        newPosition = QPointF(x, y);
        
        // 检查位置是否在可行走区域内
        if (CollisionSystem::instance().isPointInWalkableTile(newPosition)) {
            // 检查是否与障碍物碰撞
            if (!CollisionSystem::instance().isRectCollidingWithMap(newPosition, 16)) {
                validPositionFound = true;
                qDebug() << "烟雾弹传送：找到有效位置" << newPosition << "，尝试次数:" << attempt + 1;
            }
        }
    }
    
    // 如果没找到有效位置，使用地图中心位置
    if (!validPositionFound) {
        newPosition = QPointF(MAP_WIDTH/2, MAP_HEIGHT/2);
        qDebug() << "烟雾弹传送：未找到理想位置，使用地图中心位置" << newPosition;
    }
    
    setPositon(newPosition);
    qDebug() << "传送至随机位置:" << newPosition;
}


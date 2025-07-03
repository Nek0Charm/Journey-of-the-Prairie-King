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
        m_bulletViewModel->createBullet(m_stats.position, direction, 200);
    }
    
    qDebug() << "8方向射击：向8个方向发射子弹";
}

void PlayerViewModel::shootInShotgunPattern(const QPointF& direction)
{
    // 霰弹枪模式：向指定方向及其相邻方向发射子弹
    QVector<QPointF> directions;
    
    // 24个方向的单位向量（每15度一个方向）
    QVector<QPointF> allDirections = {
        // 0度 - 右
        QPointF(1.0, 0.0),
        // 15度
        QPointF(0.9659, -0.2588),
        // 30度 - 右上
        QPointF(0.8660, -0.5),
        // 45度
        QPointF(0.7071, -0.7071),
        // 60度
        QPointF(0.5, -0.8660),
        // 75度
        QPointF(0.2588, -0.9659),
        // 90度 - 上
        QPointF(0.0, -1.0),
        // 105度
        QPointF(-0.2588, -0.9659),
        // 120度 - 左上
        QPointF(-0.5, -0.8660),
        // 135度
        QPointF(-0.7071, -0.7071),
        // 150度
        QPointF(-0.8660, -0.5),
        // 165度
        QPointF(-0.9659, -0.2588),
        // 180度 - 左
        QPointF(-1.0, 0.0),
        // 195度
        QPointF(-0.9659, 0.2588),
        // 210度 - 左下
        QPointF(-0.8660, 0.5),
        // 225度
        QPointF(-0.7071, 0.7071),
        // 240度
        QPointF(-0.5, 0.8660),
        // 255度
        QPointF(-0.2588, 0.9659),
        // 270度 - 下
        QPointF(0.0, 1.0),
        // 285度
        QPointF(0.2588, 0.9659),
        // 300度 - 右下
        QPointF(0.5, 0.8660),
        // 315度
        QPointF(0.7071, 0.7071),
        // 330度
        QPointF(0.8660, 0.5),
        // 345度
        QPointF(0.9659, 0.2588)
    };
    
    // 找到最接近输入方向的标准方向
    double maxDot = -1.0;
    int closestIndex = 0;
    
    for (int i = 0; i < allDirections.size(); ++i) {
        double dot = direction.x() * allDirections[i].x() + direction.y() * allDirections[i].y();
        if (dot > maxDot) {
            maxDot = dot;
            closestIndex = i;
        }
    }
    
    // 添加主方向和相邻的两个方向（总共3个方向）
    directions.push_back(allDirections[closestIndex]);
    
    // 添加顺时针相邻方向
    int nextIndex = (closestIndex + 1) % 24;
    directions.push_back(allDirections[nextIndex]);
    
    // 添加逆时针相邻方向
    int prevIndex = (closestIndex - 1 + 24) % 24;
    directions.push_back(allDirections[prevIndex]);
    
    // 向确定的方向发射子弹
    for (const auto& dir : directions) {
        m_bulletViewModel->createBullet(m_stats.position, dir, 200);
    }
    
    qDebug() << "霰弹枪射击：向" << directions.size() << "个方向发射子弹，主方向索引:" << closestIndex;
}

void PlayerViewModel::shootInWheelShotgunCombination()
{
    // 轮子+霰弹枪组合模式：向24个方向发射子弹
    // 24个方向的单位向量（每15度一个方向）
    QVector<QPointF> allDirections = {
        // 0度 - 右
        QPointF(1.0, 0.0),
        // 15度
        QPointF(0.9659, -0.2588),
        // 30度 - 右上
        QPointF(0.8660, -0.5),
        // 45度
        QPointF(0.7071, -0.7071),
        // 60度
        QPointF(0.5, -0.8660),
        // 75度
        QPointF(0.2588, -0.9659),
        // 90度 - 上
        QPointF(0.0, -1.0),
        // 105度
        QPointF(-0.2588, -0.9659),
        // 120度 - 左上
        QPointF(-0.5, -0.8660),
        // 135度
        QPointF(-0.7071, -0.7071),
        // 150度
        QPointF(-0.8660, -0.5),
        // 165度
        QPointF(-0.9659, -0.2588),
        // 180度 - 左
        QPointF(-1.0, 0.0),
        // 195度
        QPointF(-0.9659, 0.2588),
        // 210度 - 左下
        QPointF(-0.8660, 0.5),
        // 225度
        QPointF(-0.7071, 0.7071),
        // 240度
        QPointF(-0.5, 0.8660),
        // 255度
        QPointF(-0.2588, 0.9659),
        // 270度 - 下
        QPointF(0.0, 1.0),
        // 285度
        QPointF(0.2588, 0.9659),
        // 300度 - 右下
        QPointF(0.5, 0.8660),
        // 315度
        QPointF(0.7071, 0.7071),
        // 330度
        QPointF(0.8660, 0.5),
        // 345度
        QPointF(0.9659, 0.2588)
    };
    
    // 向24个方向发射子弹
    for (const auto& direction : allDirections) {
        m_bulletViewModel->createBullet(m_stats.position, direction, 200);
    }
    
    qDebug() << "轮子+霰弹枪组合射击：向24个方向发射子弹";
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



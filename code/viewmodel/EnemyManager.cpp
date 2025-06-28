#include "viewmodel/EnemyManager.h"
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>

EnemyManager::EnemyManager(QObject *parent)
    : QObject(parent)
    , m_nextEnemyId(0)
    , m_spawnTimer(0.0)
    , m_spawnInterval(2.0)
    , m_maxEnemies(10)
    , m_enemyMoveSpeed(50.0)
{
}

void EnemyManager::spawnEnemies(double deltaTime)
{
    m_spawnTimer += deltaTime;
    
    if (m_spawnTimer >= m_spawnInterval && getActiveEnemyCount() < m_maxEnemies) {
        spawnEnemyAtRandomPosition();
        m_spawnTimer = 0.0;
    }
}

void EnemyManager::spawnEnemy(const QPointF& position)
{
    if (getActiveEnemyCount() >= m_maxEnemies) {
        return;
    }
    
    EnemyData enemy;
    enemy.id = m_nextEnemyId++;
    enemy.health = 1;
    enemy.position = position;
    enemy.velocity = QPointF(0, 0);
    enemy.moveSpeed = m_enemyMoveSpeed;
    enemy.isActive = true;
    
    m_enemies.append(enemy);
    
    emit enemySpawned(enemy);
    emit enemyCountChanged(getActiveEnemyCount());
    
    qDebug() << "Enemy spawned at position:" << position << "ID:" << enemy.id;
}

void EnemyManager::spawnEnemyAtRandomPosition()
{
    QPointF position = getRandomSpawnPosition();
    spawnEnemy(position);
}

void EnemyManager::updateEnemies(double deltaTime, const QPointF& playerPos)
{
    for (auto& enemy : m_enemies) {
        if (enemy.isActive) {
            updateEnemyAI(enemy, playerPos);
            
            // 更新位置
            enemy.position += enemy.velocity * deltaTime;
            
            // 检查是否到达玩家位置
            double distanceToPlayer = calculateDistance(enemy.position, playerPos);
            if (distanceToPlayer < 30.0) { // 碰撞距离
                emit enemyReachedPlayer(enemy.id);
            }
        }
    }
    
    removeInactiveEnemies();
}

void EnemyManager::damageEnemy(int enemyId, int damage)
{
    for (auto& enemy : m_enemies) {
        if (enemy.id == enemyId && enemy.isActive) {
            enemy.health -= damage;
            
            emit enemyDamaged(enemyId, enemy.health);
            
            if (enemy.health <= 0) {
                enemy.isActive = false;
                emit enemyDestroyed(enemyId);
                qDebug() << "Enemy destroyed, ID:" << enemyId;
            }
            break;
        }
    }
}

void EnemyManager::removeEnemy(int enemyId)
{
    for (int i = 0; i < m_enemies.size(); ++i) {
        if (m_enemies[i].id == enemyId) {
            m_enemies.removeAt(i);
            emit enemyCountChanged(getActiveEnemyCount());
            break;
        }
    }
}

void EnemyManager::clearAllEnemies()
{
    m_enemies.clear();
    emit enemyCountChanged(0);
}

int EnemyManager::getActiveEnemyCount() const
{
    int count = 0;
    for (const auto& enemy : m_enemies) {
        if (enemy.isActive) {
            count++;
        }
    }
    return count;
}

QPointF EnemyManager::getRandomSpawnPosition() const
{
    // 屏幕边界 (假设800x600)
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int margin = 50;
    
    QPointF position;
    int side = QRandomGenerator::global()->bounded(4); // 0-3: 上右下左
    
    switch (side) {
    case 0: // 上边
        position = QPointF(QRandomGenerator::global()->bounded(screenWidth), -margin);
        break;
    case 1: // 右边
        position = QPointF(screenWidth + margin, QRandomGenerator::global()->bounded(screenHeight));
        break;
    case 2: // 下边
        position = QPointF(QRandomGenerator::global()->bounded(screenWidth), screenHeight + margin);
        break;
    case 3: // 左边
        position = QPointF(-margin, QRandomGenerator::global()->bounded(screenHeight));
        break;
    }
    
    return position;
}

void EnemyManager::updateEnemyAI(EnemyData& enemy, const QPointF& playerPos)
{
    // 计算到玩家的方向
    QPointF direction = calculateDirectionToPlayer(enemy.position, playerPos);
    
    // 设置速度
    enemy.velocity = direction * enemy.moveSpeed;
}

void EnemyManager::removeInactiveEnemies()
{
    for (int i = m_enemies.size() - 1; i >= 0; --i) {
        if (!m_enemies[i].isActive) {
            m_enemies.removeAt(i);
        }
    }
}

bool EnemyManager::isPositionValid(const QPointF& position) const
{
    // 检查位置是否在合理范围内
    return position.x() >= -100 && position.x() <= 900 &&
           position.y() >= -100 && position.y() <= 700;
}

QPointF EnemyManager::calculateDirectionToPlayer(const QPointF& enemyPos, const QPointF& playerPos) const
{
    QPointF direction = playerPos - enemyPos;
    double distance = calculateDistance(enemyPos, playerPos);
    
    if (distance > 0) {
        direction /= distance; // 归一化
    }
    
    return direction;
}

double EnemyManager::calculateDistance(const QPointF& pos1, const QPointF& pos2) const
{
    return std::sqrt(std::pow(pos2.x() - pos1.x(), 2) + std::pow(pos2.y() - pos1.y(), 2));
}

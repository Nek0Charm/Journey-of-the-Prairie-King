#include <QRandomGenerator>
#include <cmath>
#include "viewmodel/EnemyManager.h"
#include "viewmodel/CollisionSystem.h"
#include "common/GameMap.h"

EnemyManager::EnemyManager(QObject *parent)
    : QObject(parent)
    , m_nextEnemyId(0)
    , m_spawnTimer(0.0)
    , m_spawnInterval(2.0)
    , m_maxEnemies(10)
    , m_enemyMoveSpeed(40.0)
{
}

void EnemyManager::spawnEnemies(double deltaTime)
{
    m_spawnTimer += deltaTime;
    
    if (m_spawnTimer >= m_spawnInterval && getActiveEnemyCount() < m_maxEnemies) {
        int spawnCount = QRandomGenerator::global()->bounded(1, 4);
        for (int i = 0; i < spawnCount; ++i) {
            spawnEnemyAtRandomPosition();
        }
        m_spawnTimer = 0.0;
        m_spawnInterval = std::max(0.9, m_spawnInterval * 0.95); // 减少生成间隔
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
    int px = QRandomGenerator::global()->bounded(128)+64;
    int py = QRandomGenerator::global()->bounded(128)+64;
    enemy.targetPosition = QPointF(px, py);
    enemy.moveSpeed = m_enemyMoveSpeed;
    enemy.isActive = true;
    enemy.isSmart = QRandomGenerator::global()->bounded(3) != 2; 
    
    m_enemies.append(enemy);
    
    emit enemySpawned(enemy);
    emit enemyCountChanged(getActiveEnemyCount());
    
    qDebug() << "Enemy spawned at position:" << position << "ID:" << enemy.id;
}

void EnemyManager::spawnEnemyAtRandomPosition()
{
    QPointF position = getRandomSpawnPosition();
    if(position == QPointF(0, 0)) {
        return; 
    }
    spawnEnemy(position);
}

void EnemyManager::updateEnemies(double deltaTime, const QPointF& playerPos, bool playerStealthMode)
{
    // 更新潜行状态
    m_playerStealthMode = playerStealthMode;
    
    for (auto& enemy : m_enemies) {
        if (enemy.isActive) {
            // 如果玩家处于潜行模式，敌人停止移动
            enemy.time += deltaTime;
            if( enemy.time >= 1.0) {
                enemy.time = 0.0;
                enemy.targetPosition = playerPos;
                if(!enemy.isSmart) {
                    int px = QRandomGenerator::global()->bounded(208)+16;
                    int py = QRandomGenerator::global()->bounded(208)+64;
                    enemy.targetPosition = QPointF(px, py);
                }
            }
            if (playerStealthMode) {
                enemy.velocity = QPointF(0, 0);
            } else {
                updateEnemyAI(enemy, enemy.targetPosition);
            }
            
            if(!isPositionValid(enemy.position + enemy.velocity * deltaTime, enemy.id)) {
                continue;
            }
            enemy.position += enemy.velocity * deltaTime;
        }
    }
    spawnEnemies(deltaTime);
    
    removeInactiveEnemies();

    emit enemiesChanged(m_enemies);
}

void EnemyManager::damageEnemy(int bulletId,int enemyId)
{
    for (auto& enemy : m_enemies) {
        if (enemy.id == enemyId && enemy.isActive) {
            enemy.health -= 1;
                        
            if (enemy.health <= 0) {
                // 在标记为非活动状态之前，先发出信号并传递位置信息
                QPointF enemyPosition = enemy.position;
                enemy.isActive = false;
                qDebug() << "Enemy destroyed, ID:" << enemyId << "at position:" << enemyPosition;
                emit enemyDestroyed(enemy.id, enemyPosition);
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

QPointF EnemyManager::getEnemyPosition(int id) const {
     for (const EnemyData& enemy : m_enemies) {
        if (enemy.id == id) {
            return enemy.position;
        }
    }
    return QPointF(-1, -1); 
}

QPointF EnemyManager::getRandomSpawnPosition() const
{
    static int count = 0;
    
    QPointF position;
    int side = QRandomGenerator::global()->bounded(4); // 0-3: 上右下左
    
    switch (side) {
    case 0: // 下边
        position = QPointF(QRandomGenerator::global()->bounded(3)*16+7*16, MAP_HEIGHT-16);
        break;
    case 1: // 右边
        position = QPointF(MAP_WIDTH-16, QRandomGenerator::global()->bounded(3)*16+7*16);
        break;
    case 2: // 上边
        position = QPointF(QRandomGenerator::global()->bounded(3)*16+7*16, 0);
        break;
    case 3: // 左边
        position = QPointF(0, QRandomGenerator::global()->bounded(3)*16+7*16);
        break;
    }
    
    if(!isPositionValid(position) && count < 5) {
        count++;
        return getRandomSpawnPosition();
    } else if (count >= 5) {
        count = 0;
        return QPointF(0, 0);
    }
    count = 0;
    return position;
}



void EnemyManager::updateEnemyAI(EnemyData& enemy, const QPointF& playerPos)
{
    // 计算到玩家的方向
    QPointF direction = QPointF(0, 0);
    if(enemy.isSmart) {
        direction = calculateDirectionToPlayer(enemy.position, playerPos, enemy.id);
    } else {
        direction = calculateDirectionToPlayer(enemy.position,QPointF(255, 255)-playerPos, enemy.id);
    }
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
    for(const auto& enemy : m_enemies) {
        if (enemy.isActive ) {
            if(CollisionSystem::instance().isCollision(position, enemy.position, 16, 16)) {
                return false;
            }
        }
    }
    return true;
}

bool EnemyManager::isPositionValid(const QPointF& position, const int enemyId) const
{
    for(const auto& enemy : m_enemies) {
        if (enemy.isActive && enemy.id != enemyId) {
            if(CollisionSystem::instance().isCollision(position, enemy.position, 16, 16)) {
                return false;
            }
        }
    }
    return true;
}

QPointF EnemyManager::calculateDirectionToPlayer(const QPointF& enemyPos, const QPointF& playerPos, int enemyId) const
{
    QPointF direction = QPointF(0, 0);
    QPoint enemyPosInt = QPoint(static_cast<int>(enemyPos.x())/16, static_cast<int>(enemyPos.y())/16);
    QPoint playerPosInt = QPoint(static_cast<int>(playerPos.x())/16, static_cast<int>(playerPos.y())/16);
    int ex = enemyPosInt.x();
    int ey = enemyPosInt.y();
    int px = playerPosInt.x();
    int py = playerPosInt.y();
    int cost = 0, minCost = 100;

    if(!GameMap::instance().isWalkable(ey, ex)) {
        return direction;
    }
    // 向上
    if(GameMap::instance().isWalkable(ey-1, ex) 
    && isPositionValid(QPointF(ex*16,(ey-1)*16), enemyId)) {
        cost = 1 + std::abs(px - ex) + std::abs(py - (ey-1));
        if(cost <= minCost) {
            minCost = cost;
            direction = QPointF(0, -1);
        }
    }
    // 向下
    if(GameMap::instance().isWalkable(ey+1, ex) 
    && isPositionValid(QPointF(ex*16,(ey+1)*16), enemyId)) {
        
        cost = 1 + std::abs(px - ex) + std::abs(py - (ey+1));
        if(cost <= minCost) {
            minCost = cost;
            direction = QPointF(0, 1);
        }
    }
    // 向左
    if(GameMap::instance().isWalkable(ey, ex-1)
    && isPositionValid(QPointF((ex-1)*16,ey*16), enemyId)) {
        cost = 1 + std::abs(px - (ex-1)) + std::abs(py - ey);
        if(cost <= minCost) {
            minCost = cost;
            direction = QPointF(-1, 0);
        }
    }
    // 向右
    if(GameMap::instance().isWalkable(ey, ex+1)
    && isPositionValid(QPointF((ex+1)*16,ey*16), enemyId)) {
        cost = 1 + std::abs(px - (ex+1)) + std::abs(py - ey);
        if(cost <= minCost) {
            minCost = cost;
            direction = QPointF(1, 0);
        }
    }
    
    return direction;
}

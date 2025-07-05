#include <QRandomGenerator>
#include <QPointF>
#include <cmath>
#include <QtMath>
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
    m_obstacles.clear();
}

void EnemyManager::spawnEnemies(double deltaTime)
{
    m_spawnTimer += deltaTime;
    
    if (m_spawnTimer >= m_spawnInterval && getActiveEnemyCount() < m_maxEnemies) {
        int spawnCount = QRandomGenerator::global()->bounded(1, 4);
        for (int i = 0; i < spawnCount; ++i) {
            // 随机选择敌人类型
            int enemyType = QRandomGenerator::global()->bounded(10); // 0-9
            QPointF position = getRandomSpawnPosition();
            
            if (position == QPointF(0, 0)) {
                continue;
            }
            
            if (enemyType < 6) {
                // 60%概率生成普通兽人
                spawnEnemy(position);
            } else if (enemyType < 8) {
                // 20%概率生成Spikeball
                spawnSpikeball(position);
            } else {
                // 20%概率生成Ogre
                spawnOgre(position);
            }
        }
        m_spawnTimer = 0.0;
        m_spawnInterval = std::max(0.9, m_spawnInterval * 0.95); // 减少生成间隔
    }
}

void EnemyManager::spawnEnemy(const QPointF& position, int enemyType)
{
    if (getActiveEnemyCount() >= m_maxEnemies) {
        return;
    }
    
    EnemyData enemy;
    enemy.id = m_nextEnemyId++;
    enemy.position = position;
    enemy.velocity = QPointF(0, 0);
    enemy.isActive = true;
    enemy.hasReachedTarget = false;
    enemy.hasCreatedObstacle = false;
    
    // 使用Orc的稳定生成逻辑：简单的随机目标位置
    int px = QRandomGenerator::global()->bounded(128)+64;
    int py = QRandomGenerator::global()->bounded(128)+64;
    enemy.targetPosition = QPointF(px, py);
    
    // 根据敌人类型设置属性
    switch (enemyType) {
        case 0: // 普通兽人
            enemy.health = 1;
            enemy.moveSpeed = m_enemyMoveSpeed;
            enemy.isSmart = QRandomGenerator::global()->bounded(3) != 2;
            enemy.enemyType = 0;
            qDebug() << "普通兽人 spawned at position:" << position << "ID:" << enemy.id;
            break;
            
        case 1: // Spikeball
            enemy.health = 2;
            enemy.moveSpeed = m_enemyMoveSpeed * 0.8;
            enemy.isSmart = true;
            enemy.enemyType = 1;
            enemy.isDeployed = false;
            enemy.deployTimer = 0.0;
            enemy.deployDelay = 3.0;
            qDebug() << "Spikeball spawned at position:" << position << "ID:" << enemy.id << "Target:" << enemy.targetPosition;
            break;
            
        case 2: // Ogre
            enemy.health = 3;
            enemy.moveSpeed = m_enemyMoveSpeed * 0.6;
            enemy.isSmart = true;
            enemy.enemyType = 2;
            enemy.damageResistance = 0.5;
            qDebug() << "Ogre spawned at position:" << position << "ID:" << enemy.id;
            break;
            
        default:
            return; // 无效的敌人类型
    }
    
    m_enemies.append(enemy);
    
    emit enemySpawned(enemy);
    emit enemyCountChanged(getActiveEnemyCount());
}

void EnemyManager::spawnEnemy(const QPointF& position)
{
    spawnEnemy(position, 0); // 默认生成普通兽人
}

void EnemyManager::spawnSpikeball(const QPointF& position)
{
    spawnEnemy(position, 1); // 生成Spikeball
}

void EnemyManager::spawnOgre(const QPointF& position)
{
    spawnEnemy(position, 2); // 生成Ogre
}

void EnemyManager::spawnEnemyAtRandomPosition()
{
    QPointF position = getRandomSpawnPosition();
    if(position == QPointF(0, 0)) {
        return; 
    }
    spawnEnemy(position);
}

void EnemyManager::updateEnemies(double deltaTime, const QPointF& playerPos, bool playerStealthMode, bool playerZombieMode, bool gameOver)
{
    // 更新潜行状态
    m_playerStealthMode = playerStealthMode;
    
    for (auto& enemy : m_enemies) {
        if (enemy.isActive) {
            // 如果玩家处于潜行模式，敌人停止移动
            enemy.time += deltaTime;
            enemy.targetPosition = playerPos;   
            if(!enemy.isSmart && enemy.time >= 1) {
                enemy.time = 0.0;
                // Spikeball不应该改变目标位置，它有自己的部署目标
                if (enemy.enemyType != 1) {
                    enemy.targetPosition = playerPos;
                    if(!enemy.isSmart) {
                        int px = QRandomGenerator::global()->bounded(208)+16;
                        int py = QRandomGenerator::global()->bounded(208)+64;
                        enemy.targetPosition = QPointF(px, py);
                    }
                }
            }
            if (playerStealthMode) {
                enemy.velocity = QPointF(0, 0);
            } else if(playerZombieMode){
                int ex = (2*enemy.position-enemy.targetPosition).x();
                int ey = (2*enemy.position-enemy.targetPosition).y();
                ex = std::max(16, std::min(ex, 223));
                ey = std::max(16, std::min(ey, 223));
                updateEnemyAI(enemy, QPointF(ex, ey));
            } else {
                                if (enemy.enemyType == 1) {
                    // Spikeball使用专门的AI逻辑
                    updateSpikeballAI(enemy, enemy.targetPosition, deltaTime);
                } else {
                    // 其他敌人使用普通AI逻辑
                    updateEnemyAI(enemy, enemy.targetPosition);
                }
            }
            QPointF new_pos = enemy.position;
            new_pos.setX(new_pos.x() + enemy.velocity.x() * deltaTime);
            if(!CollisionSystem::instance().isRectCollidingWithMap(new_pos,16)) {
                enemy.position.setX(new_pos.x());
            }
            new_pos.setY(new_pos.y() + enemy.velocity.y() * deltaTime);
            if(!CollisionSystem::instance().isRectCollidingWithMap(new_pos,16)) {
                enemy.position.setY(new_pos.y());
            }
        }
    }
    if(!gameOver) spawnEnemies(deltaTime);
    
    removeInactiveEnemies();

    emit enemiesChanged(m_enemies);
}

void EnemyManager::damageEnemy(int bulletId, int enemyId, int damage)
{
    for (auto& enemy : m_enemies) {
        if (enemy.id == enemyId && enemy.isActive) {
            // 根据敌人类型处理伤害
            int actualDamage = damage;
            if (enemy.enemyType == 2) { // Ogre
                // Ogre有伤害抗性，只受50%伤害
                actualDamage = static_cast<int>(damage * enemy.damageResistance);
                if (actualDamage < 1) actualDamage = 1; // 至少造成1点伤害
            }
            
            enemy.health -= actualDamage;
            emit enemyHitByBullet(enemyId); // 发出敌人被子弹击中的信号
            qDebug() << "Enemy ID:" << enemyId << "damaged by bullet ID:" << bulletId
                     << ", remaining health:" << enemy.health;
            if (enemy.health <= 0) {
                // 在标记为非活动状态之前，先发出信号并传递位置信息
                QPointF enemyPosition = enemy.position;
                enemy.isActive = false;
                qDebug() << "Enemy destroyed, ID:" << enemyId << "at position:" << enemyPosition;
                emit enemyDestroyed(enemy.id, enemyPosition);
            } else {
                emit enemyDamaged(enemyId, enemy.health);
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
    emit enemiesChanged(m_enemies);
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

    direction = calculateDirectionToPlayer(enemy.position, playerPos, enemy.id);

    // 设置速度
    enemy.velocity = direction * enemy.moveSpeed;
}

void EnemyManager::updateSpikeballAI(EnemyData& enemy, const QPointF& playerPos, double deltaTime)
{
    // 部署后不再更新AI，彻底静止
    if (enemy.isDeployed) {
        enemy.velocity = QPointF(0, 0);
        return;
    }
    // 未部署状态：移动到目标位置
    if (!enemy.hasReachedTarget) {
        // 检查是否到达目标位置
        double distanceToTarget = EnemyManager::calculateDistance(enemy.position, enemy.targetPosition);
        qDebug() << "Spikeball ID:" << enemy.id << "距离目标:" << distanceToTarget << "位置:" << enemy.position << "目标:" << enemy.targetPosition;
        if (distanceToTarget < 10.0) {
            enemy.hasReachedTarget = true;
            enemy.velocity = QPointF(0, 0);
            // 直接进入部署状态
            enemy.isDeployed = true;
            enemy.health = 6; // 部署后血量变为6
            qDebug() << "Spikeball reached target position and deployed at:" << enemy.targetPosition;
        } else {
            // 继续移动到目标位置
            QPointF direction = calculateDirectionToPlayer(enemy.position, enemy.targetPosition);
            
            // 如果路径被阻塞，使用简单的直线方向作为备选
            if (direction == QPointF(0, 0)) {
                QPointF diff = enemy.targetPosition - enemy.position;
                double length = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
                if (length > 0.1) {
                    direction = diff / length;
                    qDebug() << "Spikeball ID:" << enemy.id << "使用直线路径，方向:" << direction;
                } else {
                    // 如果距离太近，随机选择一个方向
                    double angle = QRandomGenerator::global()->bounded(2 * M_PI);
                    direction = QPointF(std::cos(angle), std::sin(angle));
                    qDebug() << "Spikeball ID:" << enemy.id << "距离太近，使用随机方向:" << direction;
                }
            }
            
            // 确保方向不为零向量
            if (direction == QPointF(0, 0)) {
                // 最后的备选方案：向地图中心移动
                QPointF center(128, 128);
                QPointF diff = center - enemy.position;
                double length = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
                if (length > 0.1) {
                    direction = diff / length;
                } else {
                    direction = QPointF(1, 0); // 向右移动
                }
                qDebug() << "Spikeball ID:" << enemy.id << "使用备选方向:" << direction;
            }
            
            enemy.velocity = direction * enemy.moveSpeed;
        }
    }
}

void EnemyManager::updateOgreAI(EnemyData& enemy, const QPointF& playerPos)
{
    // 计算到玩家的方向
    QPointF direction = calculateDirectionToPlayer(enemy.position, playerPos);
    // 设置速度
    enemy.velocity = direction * enemy.moveSpeed;
    
    // 检查是否接触Spikeball，如果接触则破坏Spikeball
    for (auto& otherEnemy : m_enemies) {
        if (otherEnemy.id != enemy.id && otherEnemy.isActive && otherEnemy.enemyType == 1) {
            double distance = EnemyManager::calculateDistance(enemy.position, otherEnemy.position);
            if (distance < 20.0) { // 接触距离
                // 破坏Spikeball
                otherEnemy.health = 0;
                otherEnemy.isActive = false;
                qDebug() << "Ogre destroyed Spikeball ID:" << otherEnemy.id;
                emit enemyDestroyed(otherEnemy.id, otherEnemy.position);
            }
        }
    }
}

void EnemyManager::createObstacle(const QPointF& position)
{
    // 在指定位置创建障碍物
    m_obstacles.append(position);
    qDebug() << "Obstacle created at:" << position;
}

bool EnemyManager::isObstacleAt(const QPointF& position) const
{
    for (const auto& obstacle : m_obstacles) {
        double distance = EnemyManager::calculateDistance(position, obstacle);
        if (distance < 15.0) { // 障碍物影响范围
            return true;
        }
    }
    return false;
}

void EnemyManager::clearObstacles()
{
    m_obstacles.clear();
    qDebug() << "All obstacles cleared";
}

QPointF EnemyManager::getRandomDeployPosition() const
{
    // 在地图中央区域随机选择一个部署位置
    double centerX = MAP_WIDTH / 2.0;
    double centerY = MAP_HEIGHT / 2.0;
    int range = 80; // 部署范围
    
    QPointF position;
    int attempts = 0;
    const int maxAttempts = 10;
    
    do {
        position = QPointF(
            centerX + QRandomGenerator::global()->bounded(-range, range),
            centerY + QRandomGenerator::global()->bounded(-range, range)
        );
        attempts++;
    } while (isObstacleAt(position) && attempts < maxAttempts);
    
    return position;
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
    // 检查是否与其他敌人重叠
    for(const auto& enemy : m_enemies) {
        if (enemy.isActive ) {
            if(CollisionSystem::instance().isCollision(position, enemy.position, 16, 16)) {
                return false;
            }
        }
    }
    
    // 检查是否与障碍物重叠
    if (isObstacleAt(position)) {
        return false;
    }
    
    return true;
}

bool EnemyManager::isPositionValid(const QPointF& position, const int enemyId) const
{
    // 检查是否与其他敌人重叠
    for(const auto& enemy : m_enemies) {
        if (enemy.isActive && enemy.id != enemyId) {
            if(CollisionSystem::instance().isCollision(position, enemy.position, 16, 16)) {
                return false;
            }
        }
    }
    
    // 检查是否与障碍物重叠
    if (isObstacleAt(position)) {
        return false;
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

QPointF EnemyManager::calculateDirectionToPlayer(const QPointF& enemyPos, const QPointF& playerPos) const {
    return calculateDirectionToPlayer(enemyPos, playerPos, -1);
}

double EnemyManager::calculateDistance(const QPointF& p1, const QPointF& p2) {
    double dx = p1.x() - p2.x();
    double dy = p1.y() - p2.y();
    return std::sqrt(dx * dx + dy * dy);
}

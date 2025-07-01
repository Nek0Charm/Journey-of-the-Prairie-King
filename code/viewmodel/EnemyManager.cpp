#include "viewmodel/EnemyManager.h"
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>
#include "../../precomp.h"

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
    if(position == QPointF(0, 0)) {
        return; 
    }
    spawnEnemy(position);
}

void EnemyManager::updateEnemies(double deltaTime, const QPointF& playerPos)
{
    for (auto& enemy : m_enemies) {
        if (enemy.isActive) {
            updateEnemyAI(enemy, playerPos);
            if(!isPositionValid(enemy.position + enemy.velocity * deltaTime, enemy.id)) {
                continue;
            }
            enemy.position += enemy.velocity * deltaTime;
        }
    }
    spawnEnemies(deltaTime);
    
    removeInactiveEnemies();
}

void EnemyManager::damageEnemy(int bulletId,int enemyId)
{
    for (auto& enemy : m_enemies) {
        if (enemy.id == enemyId && enemy.isActive) {
            enemy.health -= 1;
                        
            if (enemy.health <= 0) {
                enemy.isActive = false;
                qDebug() << "Enemy destroyed, ID:" << enemyId;
                emit enemyDestroyed(enemy.position);
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
    static int count = 0;
    const int margin = 50;
    
    QPointF position;
    int side = QRandomGenerator::global()->bounded(4); // 0-3: 上右下左
    
    switch (side) {
    case 0: // 上边
        position = QPointF(QRandomGenerator::global()->bounded(3)*16+7*16+8, MAP_HEIGHT-8);
        break;
    case 1: // 右边
        position = QPointF(MAP_WIDTH-8, QRandomGenerator::global()->bounded(3)*16+6*16+8);
        break;
    case 2: // 下边
        position = QPointF(QRandomGenerator::global()->bounded(3)*16+7*16+8, 8);
        break;
    case 3: // 左边
        position = QPointF(8, QRandomGenerator::global()->bounded(3)*16+6*16+8);
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
    for(const auto& enemy : m_enemies) {
        if (enemy.isActive ) {
            if(enemy.position.x() < position.x() + ENEMY_WIDTH &&
               enemy.position.x() + ENEMY_WIDTH > position.x() &&
               enemy.position.y() < position.y() + ENEMY_WIDTH &&
               enemy.position.y() + ENEMY_WIDTH > position.y()) {
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
            if(enemy.position.x() < position.x() + ENEMY_WIDTH &&
               enemy.position.x() + ENEMY_WIDTH > position.x() &&
               enemy.position.y() < position.y() + ENEMY_WIDTH &&
               enemy.position.y() + ENEMY_WIDTH > position.y()) {
                return false;
            }
        }
    }
    return true;
}

QPointF EnemyManager::calculateDirectionToPlayer(const QPointF& enemyPos, const QPointF& playerPos) const
{
    QPointF direction = playerPos - enemyPos;
    double distance = calculateDistance(enemyPos, playerPos);
    
    if (distance > 0) {
        direction /= distance; // 归一化
    }
    if (std::abs(direction.x()) > std::abs(direction.y())) {
        direction.setY(0);
        direction.setX(direction.x() > 0 ? 1 : -1);
    } else {
        direction.setX(0);
        direction.setY(direction.y() > 0 ? 1 : -1);
    }
    
    return direction;
}

double EnemyManager::calculateDistance(const QPointF& pos1, const QPointF& pos2) const
{
    return std::sqrt(std::pow(pos2.x() - pos1.x(), 2) + std::pow(pos2.y() - pos1.y(), 2));
}

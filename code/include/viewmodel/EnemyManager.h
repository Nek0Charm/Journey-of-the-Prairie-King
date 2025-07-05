#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include <memory>

class EnemyManager : public QObject {
    Q_OBJECT
    
public:
    
    
    explicit EnemyManager(QObject *parent = nullptr);
    ~EnemyManager() = default;
    
    // 敌人生成
    void spawnEnemies(double deltaTime);
    void spawnEnemy(const QPointF& position);
    void spawnEnemyAtRandomPosition();
    
    // 敌人管理
    void updateEnemies(double deltaTime, const QPointF& playerPos, bool playerStealthMode = false, bool gameOver = false);
    
    // 潜行状态查询
    bool isPlayerStealthMode() const { return m_playerStealthMode; }
    

    
    void damageEnemy(int bulletId ,int enemyId);
    void removeEnemy(int enemyId);
    void clearAllEnemies();
    
    // 状态查询
    const QList<EnemyData>& getEnemies() const { return m_enemies; }
    int getEnemyCount() const { return m_enemies.size(); }
    int getActiveEnemyCount() const;
    bool hasEnemies() const { return !m_enemies.isEmpty(); }
    QPointF getEnemyPosition(int id) const;
    
    // 配置设置
    void setSpawnInterval(double interval) { m_spawnInterval = interval; }
    void setMaxEnemies(int max) { m_maxEnemies = max; }
    void setEnemyMoveSpeed(double speed) { m_enemyMoveSpeed = speed; }
    
signals:
    void enemySpawned(const EnemyData& enemy);
    void enemyDestroyed(int id, const QPointF& position);
    void enemyReachedPlayer(int enemyId);
    void enemyDamaged(int enemyId, int remainingHealth);
    void enemyCountChanged(int count);
    void enemiesChanged(const QList<EnemyData>& enemies);
    
private:
    QList<EnemyData> m_enemies;
    int m_nextEnemyId = 0;
    double m_spawnTimer = 0.0;
    double m_spawnInterval = 2.0;
    int m_maxEnemies = 10;
    double m_enemyMoveSpeed = 40.0;
    bool m_playerStealthMode = false;

    static constexpr double ENEMY_WIDTH = 15.0;
    
    QPointF getRandomSpawnPosition() const;
    void updateEnemyAI(EnemyData& enemy, const QPointF& playerPos);
    void removeInactiveEnemies();
    bool isPositionValid(const QPointF& position) const;
    bool isPositionValid(const QPointF& position, const int enemyId) const;
    QPointF calculateDirectionToPlayer(const QPointF& enemyPos, const QPointF& playerPos, int enemyId) const;
};

#endif // ENEMYMANAGER_H

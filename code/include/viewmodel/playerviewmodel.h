class PlayerViewModel : public QObject {
    Q_OBJECT
    
public:
    struct PlayerStats {
        int lives = 4;
        QPointF position = QPointF(400, 300);
        QPointF shootingDirection = QPointF(1, 0);
        double moveSpeed = 100.0;
        double shootCooldown = 0.2;
    };
    
    explicit PlayerViewModel(QObject *parent = nullptr);
    
    // 基础操作
    void move(const QPointF& direction);
    void shoot(const QPointF& direction);
    void update(double deltaTime);
    
    // 状态管理
    void takeDamage();
    
    // 状态查询
    const PlayerStats& getStats() const { return m_stats; }
    bool canShoot() const { return m_currentShootCooldown <= 0; }
    
signals:
    void statsChanged(const PlayerStats& stats);
    void bulletFired(const QPointF& position, const QPointF& direction);
    void playerDied();
    
private:
    PlayerStats m_stats;
    double m_currentShootCooldown = 0.0;
    
    void updateShootCooldown(double deltaTime);
};

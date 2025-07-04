#ifndef __BULLET_VIEW_MODEL_H__
#define __BULLET_VIEW_MODEL_H__

class BulletViewModel : public QObject {
    Q_OBJECT
public:

    BulletViewModel(QObject *parent = nullptr);
    ~BulletViewModel();
    void createBullet(const QPointF& position, const QPointF& direction, double speed, int damage = 1);
    void updateBullets(double deltaTime);
    void removeBullet(int bulletId);
    void removeBullets();
    void clearAllBullets();
    QList<BulletData> getActiveBullets() const;
    int getBulletCount() const { return m_bullets.size(); }
    int getBulletDamage(int bulletId) const; // 获取指定子弹的伤害值
    void updateBulletDamage(int bulletId, int newDamage); // 更新子弹伤害值
signals:
    void bulletsChanged(QList<BulletData> bullets);

private:
    QList<BulletData> m_bullets; // 存储所有子弹数据
    int m_nextBulletId = 0; 
    QPointF normalize(const QPointF& point);
};
    


#endif
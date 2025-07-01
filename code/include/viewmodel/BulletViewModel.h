#ifndef __BULLET_VIEW_MODEL_H__
#define __BULLET_VIEW_MODEL_H__

#include <QObject>
#include <QPointF>
#include <QList>

class BulletViewModel : public QObject {
    Q_OBJECT
public:
    struct BulletData {
        int id;
        QPointF position;
        QPointF velocity;
        bool isActive; // 是否处于活动状态
    };
    BulletViewModel(QObject *parent = nullptr);
    ~BulletViewModel();
    void createBullet(const QPointF& position, const QPointF& direction, double speed);
    void updateBullets(double deltaTime);
    void removeBullet(int bulletId);
    void removeBullets();
    void clearAllBullets();
    QList<BulletData> getActiveBullets() const;
    int getBulletCount() const { return m_bullets.size(); }

private:
    QList<BulletData> m_bullets; // 存储所有子弹数据
    int m_nextBulletId = 0; 
    QPointF normalize(const QPointF& point);
};
    


#endif
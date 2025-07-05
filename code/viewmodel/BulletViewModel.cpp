#include "viewmodel/BulletViewModel.h"
#include "viewmodel/CollisionSystem.h"

BulletViewModel::BulletViewModel(QObject *parent)
    : QObject(parent) {
}
BulletViewModel::~BulletViewModel() {
    clearAllBullets();
}

void BulletViewModel::createBullet(const QPointF& position, const QPointF& direction, double speed) {
    BulletData bullet;
    bullet.id = m_nextBulletId++;
    bullet.position = position;
    bullet.velocity = normalize(direction) * speed;
    bullet.isActive = true;

    m_bullets.append(bullet);
}

void BulletViewModel::updateBullets(double deltaTime){
    for(auto& bullet: m_bullets) {
        if(bullet.isActive) {
            bullet.position += bullet.velocity * deltaTime;
            if(bullet.position.x() < 0 || bullet.position.x() > MAP_WIDTH ||
               bullet.position.y() < 0 || bullet.position.y() > MAP_HEIGHT) {
                bullet.isActive = false;
            } else if(CollisionSystem::instance().isRectCollidingWithMap(bullet.position, 5)) {
                bullet.isActive = false; 
            }
        }
    }
    removeBullets();
    emit bulletsChanged(m_bullets);
}

void BulletViewModel::removeBullet(int bulletId) {
    for (int i = 0; i < m_bullets.size(); ++i) {
        if (m_bullets[i].id == bulletId) {
            m_bullets[i].isActive = false; 
            return; 
        }
    }
}

void BulletViewModel::removeBullets() {
    for (int i = 0; i < m_bullets.size(); ++i) {
        if (m_bullets[i].isActive == false) {
            m_bullets.removeAt(i);
        }
    }
}
void BulletViewModel::clearAllBullets(){
    m_bullets.clear();
}

QList<BulletData> BulletViewModel::getActiveBullets() const{
    QList<BulletData> activeBullets;
    for (const auto& bullet : m_bullets) {
        if (bullet.isActive) {
            activeBullets.append(bullet);
        }
    }
    return activeBullets;
}



QPointF BulletViewModel::normalize(const QPointF& point) {
    qreal length = sqrt(point.x() * point.x() + point.y() * point.y());
    if (length > 0) {
        return QPointF(point.x() / length, point.y() / length);
    }
    return point;
}
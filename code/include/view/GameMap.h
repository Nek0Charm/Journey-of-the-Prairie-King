#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "view/SpriteManager.h"
#include "view/Animation.h"

class ExplosionEffect {
public:
    ExplosionEffect(const QPointF& position);
    ~ExplosionEffect();
    void update(double deltaTime);
    void paint(QPainter* painter, const QPixmap& spriteSheet, const QPointF& viewOffset);
    bool isFinished() const { return m_animation ? m_animation->isFinished() : true; }
private:
    QPointF m_position; 
    Animation* m_animation; 
};

class GameMapView {
public:
    GameMapView(QString map_title);
    ~GameMapView() { qDeleteAll(m_animations); qDeleteAll(m_explosions); }
    bool loadFromFile(const QString& path, const QString& mapName, const QString& layoutName);
    int getTileIdAt(int row, int col) const;
    QString getTileSpriteName(int tileId) const;
    void paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset);
    void update(double deltaTime);
    int getWidth() const;
    int getHeight() const;
    QString getMapTitle() const { return map_title; }
    void createExplosion(const QPointF& position);
private:
    QList<QList<int>> m_tiles;       // 存储地图布局的二维列表
    QMap<int, QString> m_tileLegend; // 存储图块ID到精灵名字的映射
    QMap<QString, Animation*> m_animations;
    QList<ExplosionEffect*> m_explosions;
    QString map_title;
    int m_width;
    int m_height;  
};

#endif // GAMEMAP_H
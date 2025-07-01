#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QMap>
#include <QString>
#include <QFile>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "view/SpriteManager.h"
#include "view/Animation.h"
class GameMap {
public:
    GameMap(QString map_title);
    ~GameMap() { qDeleteAll(m_animations); }
    bool loadFromFile(const QString& path, const QString& mapName, const QString& layoutName);
    int getTileIdAt(int row, int col) const;
    QString getTileSpriteName(int tileId) const;
    void paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset);
    void update(double deltaTime);
    int getWidth() const;
    int getHeight() const;

private:
    QList<QList<int>> m_tiles;       // 存储地图布局的二维列表
    QMap<int, QString> m_tileLegend; // 存储图块ID到精灵名字的映射
    QMap<QString, Animation*> m_animations;
    QString map_title;
    int m_width;
    int m_height;
};

#endif // GAMEMAP_H
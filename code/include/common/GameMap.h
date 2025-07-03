#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class GameMap {
public:
    GameMap();
    static GameMap& instance();
    ~GameMap() {}
    bool loadFromFile(const QString& path, const QString& mapName, const QString& layoutName);
    bool isWalkable(int row, int col) const;
    int getTileIdAt(int row, int col) const;
    QString getTileSpriteName(int tileId) const;
    void paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset);
    void update(double deltaTime);
    int getWidth() const;
    int getHeight() const;
private:
    QList<QList<int>> m_tiles;       // 存储地图布局的二维列表
    QMap<int, QString> m_tileLegend; // 存储图块ID到精灵名字的映射
    QString map_title;
    int m_width;
    int m_height;
};

#endif
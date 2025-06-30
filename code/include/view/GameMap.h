#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QMap>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
class GameMap {
public:
    GameMap();

    bool loadFromFile(const QString& path, const QString& mapName, const QString& layoutName);
    int getTileIdAt(int row, int col) const;
    QString getTileSpriteName(int tileId) const;
    int getWidth() const;
    int getHeight() const;

private:
    QList<QList<int>> m_tiles;       // 存储地图布局的二维列表
    QMap<int, QString> m_tileLegend; // 存储图块ID到精灵名字的映射
    int m_width;
    int m_height;
};

#endif // GAMEMAP_H
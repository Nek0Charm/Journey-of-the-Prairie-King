#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QList>
#include <QMap>
#include <QString>

class GameMap {
public:
    GameMap();

    bool loadFromFile(const QString& path, const QString& mapName, const QString& layoutName);

    // 获取指定行列的图块ID
    int getTileIdAt(int row, int col) const;

    // 根据图块ID获取它对应的精灵名字
    QString getTileSpriteName(int tileId) const;

    // 获取地图的宽度（列数）和高度（行数）
    int getWidth() const;
    int getHeight() const;

private:
    QList<QList<int>> m_tiles;       // 存储地图布局的二维列表
    QMap<int, QString> m_tileLegend; // 存储图块ID到精灵名字的映射
    int m_width;
    int m_height;
};

#endif // GAMEMAP_H
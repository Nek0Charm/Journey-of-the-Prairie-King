#include "view/Gamemap.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

GameMap::GameMap() : m_width(16), m_height(16) {}

// GameMap.cpp

bool GameMap::loadFromFile(const QString& path, const QString& mapName, const QString& layoutName) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开地图文件:" << path;
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (doc.isNull()) {
        qWarning() << "解析地图文件失败:" << error.errorString();
        return false;
    }

    QJsonObject root = doc.object();

    // 首先进入指定的地图对象，例如 "map_1"
    QJsonObject mapObject = root[mapName].toObject();
    if (mapObject.isEmpty()) {
        qWarning() << "在JSON中找不到名为" << mapName << "的地图对象";
        return false;
    }

    // 从地图对象中解析图例
    m_tileLegend.clear();
    QJsonObject legendObject = mapObject["tile_definitions"].toObject();
    for (auto it = legendObject.begin(); it != legendObject.end(); ++it) {
        m_tileLegend[it.key().toInt()] = it.value().toString();
    }

    // 从地图对象中，根据布局名解析地图布局
    m_tiles.clear();
    m_width = 0; // 重置尺寸
    m_height = 0;
    QJsonArray layoutArray = mapObject[layoutName].toArray();
    if (layoutArray.isEmpty()) {
        qWarning() << "在地图" << mapName << "中找不到名为" << layoutName << "的布局，或者布局为空";
        return false;
    }
    
    m_height = layoutArray.count();
    for (const QJsonValue& rowVal : layoutArray) {
        QList<int> row;
        QJsonArray rowArray = rowVal.toArray();
        if (m_height > 0 && m_width == 0) {
            m_width = rowArray.count();
        }
        for (const QJsonValue& tileVal : rowArray) {
            row.append(tileVal.toInt());
        }
        m_tiles.append(row);
    }
    
    qDebug() << "地图" << mapName << "的布局" << layoutName << "加载成功，尺寸:" << m_width << "x" << m_height;
    return true;
}

int GameMap::getTileIdAt(int row, int col) const {
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        return 0; 
    }
    return m_tiles[row][col];
}

QString GameMap::getTileSpriteName(int tileId) const {
    return m_tileLegend.value(tileId, "empty");
}

int GameMap::getWidth() const { return m_width; }
int GameMap::getHeight() const { return m_height; }
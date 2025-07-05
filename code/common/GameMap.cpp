#include "common/GameMap.h"
GameMap::GameMap() : m_width(16), m_height(16)
                                {
}

GameMap& GameMap::instance() {
    static GameMap instance;
    return instance;
}

bool GameMap::loadFromFile(const QString& path, const QString& mapName, const QString& layoutName) {
    map_title = mapName + "_" + layoutName;
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

    QJsonObject mapObject = root[mapName].toObject();
    if (mapObject.isEmpty()) {
        qWarning() << "在JSON中找不到名为" << mapName << "的地图对象";
        return false;
    }        
    m_tiles.clear();
    m_width = 0; 
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

bool GameMap::isWalkable(int row, int col) const{
    return getTileIdAt(row, col) == 1 || getTileIdAt(row, col) == 3 || 
           getTileIdAt(row, col) == 4 || getTileIdAt(row, col) == 5;
}

/*
    * 获取指定位置的图块类型
    * @param row 行索引
    * @param col 列索引
    * @return 返回图块类型
    * 「1: 刷怪点, 2: , 3~5: 空地, 7: 地图边界」
*/
int GameMap::getTileIdAt(int row, int col) const {
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        return 0; 
    }
    if(m_tiles[row][col] == 7) {
        // qDebug() << "cannot pass position: " << row << col;
    }
    return m_tiles[row][col];
}

int GameMap::getWidth() const { return m_width; }
int GameMap::getHeight() const { return m_height; }

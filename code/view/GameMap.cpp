#include "view/GameMap.h"

GameMap::GameMap(QString map_title) : m_width(16), m_height(16), map_title(map_title) {
    loadFromFile(":/assert/picture/gamemap.json", "map_1", "1");
}

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

    QJsonObject mapObject = root[mapName].toObject();
    if (mapObject.isEmpty()) {
        qWarning() << "在JSON中找不到名为" << mapName << "的地图对象";
        return false;
    }

    m_tileLegend.clear();
    QJsonObject legendObject = mapObject["tile_definitions"].toObject();
    qDeleteAll(m_animations);
    for (auto it = legendObject.begin(); it != legendObject.end(); ++it) {
            int tileId = it.key().toInt();
            QString name = it.value().toString();
            m_tileLegend[tileId] = name;
            QList<QString> animFrames = SpriteManager::instance().getAnimationSequence(name);
            if (!animFrames.isEmpty()) {
                m_animations[name] = new Animation(animFrames, 1.0, true); 
            }
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


void GameMap::update(double deltaTime) {
    for (Animation* anim : m_animations) {
        anim->update(deltaTime);
    }
}

void GameMap::paint(QPainter *painter, const QPixmap &spriteSheet, const QPointF &viewOffset) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (getWidth() > 0) {
        for (int row = 0; row < getHeight(); ++row) {
        for (int col = 0; col < getWidth(); ++col) {
            int tileId = getTileIdAt(row, col);
            QString spriteName = getTileSpriteName(tileId);
            
            QRect sourceRect; 
            if (m_animations.contains(spriteName)) {
                Animation* anim = m_animations[spriteName];
                const QString& currentFrameName = anim->getCurrentFrameName();
                sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
            } else {
                sourceRect = SpriteManager::instance().getSpriteRect(spriteName);
            }

            if (sourceRect.isNull()) continue;
                double destX = (col * sourceRect.width())* 5;
                double destY = (row * sourceRect.height())* 5;
                QRectF destRect(destX, destY, sourceRect.width() * 5, sourceRect.height() * 5);
                destRect.translate(viewOffset);
                painter->drawPixmap(destRect, spriteSheet, sourceRect);
            }
        }
    }
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
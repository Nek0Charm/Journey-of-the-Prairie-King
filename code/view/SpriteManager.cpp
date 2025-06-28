#include "view/SpriteManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

SpriteManager& SpriteManager::instance() {
    static SpriteManager instance;
    return instance;
}

bool SpriteManager::loadFromFile(const QString& path) {
    // 加载和解析 JSON 文件
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_rootObject = doc.object();
    
    // 解析 "frames"
    m_spriteRects.clear();
    QJsonObject frames = m_rootObject["frames"].toObject();
    for (const QString& key : frames.keys()) {
        QJsonObject frameData = frames[key].toObject();
        m_spriteRects[key] = QRect(frameData["x"].toInt(), frameData["y"].toInt(), frameData["w"].toInt(), frameData["h"].toInt());
    }

    // 解析 "composites"
    m_composites.clear();
    QJsonObject composites = m_rootObject["composites"].toObject();
    for (const QString& key : composites.keys()) {
        QJsonArray compositeData = composites[key].toArray();
        QList<SpritePart> parts;
        for (const QJsonValue& val : compositeData) {
            QJsonObject partData = val.toObject();
            SpritePart part;
            part.frameName = partData["frame"].toString();
            part.offset.setX(partData["offset"].toObject()["x"].toInt());
            part.offset.setY(partData["offset"].toObject()["y"].toInt());
            parts.append(part);
        }
        m_composites[key] = parts;
    }

    return true;
}

QRect SpriteManager::getSpriteRect(const QString& name) const {
    return m_spriteRects.value(name, QRect());
}

QList<SpritePart> SpriteManager::getCompositeParts(const QString& compositeName) const {
    return m_composites.value(compositeName, QList<SpritePart>());
}

QList<QString> SpriteManager::getAnimationSequence(const QString& animationName) const {
    QJsonObject animations = m_rootObject["animations"].toObject();
    QJsonArray animData = animations[animationName].toArray();
    QList<QString> frameNames;
    for (const QJsonValue& val : animData) {
        frameNames.append(val.toString());
    }
    return frameNames;
}
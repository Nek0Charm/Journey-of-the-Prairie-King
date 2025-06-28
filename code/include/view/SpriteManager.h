#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <QObject>
#include <QMap>
#include <QRect>
#include <QString>
#include <QJsonObject>
#include <QPoint>

// 一个描述复合精灵的部件的结构体
struct SpritePart {
    QString frameName; // 引用的基础帧的名字
    QPoint offset;     // 相对偏移
};

class SpriteManager {
public:
    static SpriteManager& instance();
    bool loadFromFile(const QString& path);
    QRect getSpriteRect(const QString& name) const;

    //根据名字获取复合精灵的所有部件
    QList<SpritePart> getCompositeParts(const QString& compositeName) const;

    //返回组成动画的（简单或复合）帧名列表
    QList<QString> getAnimationSequence(const QString& animationName) const;

private:
    SpriteManager() = default;
    ~SpriteManager() = default;
    SpriteManager(const SpriteManager&) = delete;
    SpriteManager& operator=(const SpriteManager&) = delete;
    
    QMap<QString, QRect> m_spriteRects;
    QMap<QString, QList<SpritePart>> m_composites; // 用于存储复合精灵
    QJsonObject m_rootObject;
};

#endif // SPRITEMANAGER_H
#ifndef ANIMATION_H
#define ANIMATION_H

#include <QList>
#include <QString>


class Animation {
public:
    // 构造函数：接收一个字符串列表（帧名），帧率，以及是否循环
    Animation(const QList<QString>& frameNames, double frameRate = 10.0, bool loops = true);

    // 根据流逝的时间更新动画
    void update(double deltaTime);

    // 获取当前应该显示的帧的名字
    const QString& getCurrentFrameName() const;

    // 重置动画到第一帧
    void reset();

private:
    QList<QString> m_frameNames; 

    bool m_loops;                 // 动画是否循环播放
    double m_frameDuration;       // 每一帧的持续时间 (秒)
    double m_elapsedTime;         // 从上一帧开始流逝的时间
    int m_currentIndex;           // 当前帧的索引
};

#endif // ANIMATION_H
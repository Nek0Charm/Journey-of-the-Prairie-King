#include "view/Animation.h"

Animation::Animation(const QList<QString>& frameNames, double frameRate, bool loops)
    : m_frameNames(frameNames), m_loops(loops), m_elapsedTime(0.0), m_currentIndex(0) {
    if (frameRate > 0.0) {
        m_frameDuration = 1.0 / frameRate;
    } else {
        m_frameDuration = 0.0;
    }
}

bool Animation::isFinished() const {
    if (!m_loops && m_currentIndex >= m_frameNames.count() - 1) {
        return true;
    }
    return false;
}

void Animation::update(double deltaTime) {
    if (m_frameDuration == 0.0 || m_frameNames.isEmpty()) return;
    m_elapsedTime += deltaTime;
    if (m_elapsedTime >= m_frameDuration) {
        m_elapsedTime -= m_frameDuration;
        m_currentIndex++;
        if (m_currentIndex >= m_frameNames.count()) {
            if (m_loops) m_currentIndex = 0;
            else m_currentIndex = m_frameNames.count() - 1;
        }
    }
}

const QString& Animation::getCurrentFrameName() const {
    return m_frameNames.at(m_currentIndex);
}

void Animation::reset() {
    m_currentIndex = 0;
    m_elapsedTime = 0.0;
}
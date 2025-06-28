#include "viewmodel/InputManager.h"
#include <QDebug>
#include <cmath>

InputManager::InputManager(QObject *parent)
    : QObject(parent)
{
    resetInput();
}

void InputManager::handleKeyPress(Qt::Key key)
{
    m_pressedKeys.insert(key);
    
    // 处理特殊按键
    if (key == m_pauseKey) {
        m_inputState.isPaused = true;
        emit pauseRequested();
    } else if (key == m_menuKey) {
        m_inputState.isMenu = true;
        emit menuRequested();
    }
    
    updateInput();
}

void InputManager::handleKeyRelease(Qt::Key key)
{
    m_pressedKeys.remove(key);
    
    // 处理特殊按键释放
    if (key == m_pauseKey) {
        m_inputState.isPaused = false;
    } else if (key == m_menuKey) {
        m_inputState.isMenu = false;
    }
    
    updateInput();
}

void InputManager::updateInput()
{
    QPointF oldMovementDirection = m_inputState.movementDirection;
    QPointF oldShootingDirection = m_inputState.shootingDirection;
    bool oldIsShooting = m_inputState.isShooting;
    
    // 更新移动方向
    updateMovementDirection();
    
    // 更新射击方向
    updateShootingDirection();
    
    // 检查是否有输入变化
    if (oldMovementDirection != m_inputState.movementDirection) {
        emit movementChanged(m_inputState.movementDirection);
    }
    
    if (oldShootingDirection != m_inputState.shootingDirection || 
        oldIsShooting != m_inputState.isShooting) {
        emit shootingChanged(m_inputState.shootingDirection, m_inputState.isShooting);
    }
    
    emit inputChanged(m_inputState);
}

void InputManager::resetInput()
{
    m_inputState.movementDirection = QPointF(0, 0);
    m_inputState.shootingDirection = QPointF(1, 0); // 默认向右
    m_inputState.isShooting = false;
    m_inputState.isPaused = false;
    m_inputState.isMenu = false;
    m_pressedKeys.clear();
}

bool InputManager::hasMovementInput() const
{
    return !m_inputState.movementDirection.isNull();
}

bool InputManager::hasShootingInput() const
{
    return m_inputState.isShooting;
}

void InputManager::updateMovementDirection()
{
    m_inputState.movementDirection = calculateDirectionFromKeys(m_movementKeys);
    m_inputState.movementDirection = normalizeDirection(m_inputState.movementDirection);
}

void InputManager::updateShootingDirection()
{
    QPointF shootingDirection = calculateDirectionFromKeys(m_shootingKeys);
    
    if (!shootingDirection.isNull()) {
        m_inputState.shootingDirection = normalizeDirection(shootingDirection);
        m_inputState.isShooting = true;
    } else {
        m_inputState.isShooting = false;
    }
}

QPointF InputManager::normalizeDirection(const QPointF& direction) const
{
    if (direction.isNull()) {
        return QPointF(0, 0);
    }
    
    double length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        return direction / length;
    }
    
    return QPointF(0, 0);
}

QPointF InputManager::calculateDirectionFromKeys(const QSet<Qt::Key>& keys) const
{
    QPointF direction(0, 0);
    
    for (Qt::Key key : m_pressedKeys) {
        if (keys.contains(key)) {
            switch (key) {
            case Qt::Key_W:
            case Qt::Key_Up:
                direction.setY(direction.y() - 1);
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                direction.setY(direction.y() + 1);
                break;
            case Qt::Key_A:
            case Qt::Key_Left:
                direction.setX(direction.x() - 1);
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                direction.setX(direction.x() + 1);
                break;
            default:
                break;
            }
        }
    }
    
    return direction;
}

bool InputManager::isMovementKey(Qt::Key key) const
{
    return m_movementKeys.contains(key);
}

bool InputManager::isShootingKey(Qt::Key key) const
{
    return m_shootingKeys.contains(key);
} 
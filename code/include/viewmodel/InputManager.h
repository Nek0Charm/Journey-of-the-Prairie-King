#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>
#include <QPointF>
#include <QSet>
#include <Qt>

class InputManager : public QObject
{
    Q_OBJECT

public:
    struct InputState {
        QPointF movementDirection;    // WASD移动方向
        QPointF shootingDirection;    // 方向键射击方向
        bool isShooting = false;
        bool isPaused = false;
        bool isMenu = false;
    };

    explicit InputManager(QObject *parent = nullptr);
    ~InputManager() = default;

    // 输入处理
    void handleKeyPress(Qt::Key key);
    void handleKeyRelease(Qt::Key key);
    void updateInput();
    void resetInput();

    // 状态查询
    const InputState& getInputState() const { return m_inputState; }
    QPointF getMovementDirection() const { return m_inputState.movementDirection; }
    QPointF getShootingDirection() const { return m_inputState.shootingDirection; }
    bool isShooting() const { return m_inputState.isShooting; }
    bool isPaused() const { return m_inputState.isPaused; }
    bool isMenu() const { return m_inputState.isMenu; }

    // 按键状态查询
    bool isKeyPressed(Qt::Key key) const { return m_pressedKeys.contains(key); }
    bool hasMovementInput() const;
    bool hasShootingInput() const;

    // 配置设置
    void setMovementKeys(const QSet<Qt::Key>& keys) { m_movementKeys = keys; }
    void setShootingKeys(const QSet<Qt::Key>& keys) { m_shootingKeys = keys; }
    void setPauseKey(Qt::Key key) { m_pauseKey = key; }
    void setMenuKey(Qt::Key key) { m_menuKey = key; }

signals:
    void inputChanged(const InputState& state);
    void pauseRequested();
    void menuRequested();
    void movementChanged(QPointF direction);
    void shootingChanged(QPointF direction, bool isShooting);

private:
    InputState m_inputState;
    QSet<Qt::Key> m_pressedKeys;
    
    // 按键映射
    QSet<Qt::Key> m_movementKeys = {Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D};
    QSet<Qt::Key> m_shootingKeys = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
    Qt::Key m_pauseKey = Qt::Key_Escape;
    Qt::Key m_menuKey = Qt::Key_M;

    void updateMovementDirection();
    void updateShootingDirection();
    QPointF normalizeDirection(const QPointF& direction) const;
    QPointF calculateDirectionFromKeys(const QSet<Qt::Key>& keys) const;
    bool isMovementKey(Qt::Key key) const;
    bool isShootingKey(Qt::Key key) const;
};

#endif // INPUTMANAGER_H 
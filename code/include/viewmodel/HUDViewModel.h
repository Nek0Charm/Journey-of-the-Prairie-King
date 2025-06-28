#ifndef HUDVIEWMODEL_H
#define HUDVIEWMODEL_H

#include <QObject>
#include <QString>

class HUDViewModel : public QObject
{
    Q_OBJECT

public:
    explicit HUDViewModel(QObject *parent = nullptr);
    ~HUDViewModel() = default;

    // 状态查询
    int getLives() const { return m_lives; }
    int getEnemyCount() const { return m_enemyCount; }
    int getScore() const { return m_score; }
    QString getLivesText() const;
    QString getEnemyCountText() const;
    QString getScoreText() const;
    QString getGameStateText() const;

    // 状态更新
    void updateLives(int lives);
    void updateEnemyCount(int count);
    void updateScore(int score);
    void setGameState(const QString& state);

    // 游戏状态
    void setGameActive(bool active) { m_isGameActive = active; }
    bool isGameActive() const { return m_isGameActive; }

    // 重置状态
    void reset();

signals:
    void livesChanged(int lives);
    void enemyCountChanged(int count);
    void scoreChanged(int score);
    void gameStateChanged(const QString& state);
    void hudDataChanged();

private:
    int m_lives = 4;
    int m_enemyCount = 0;
    int m_score = 0;
    QString m_gameState = "Ready";
    bool m_isGameActive = false;

    void emitHUDDataChanged();
};

#endif // HUDVIEWMODEL_H 
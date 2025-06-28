#include "viewmodel/HUDViewModel.h"
#include <QString>

HUDViewModel::HUDViewModel(QObject *parent)
    : QObject(parent)
    , m_lives(4)
    , m_enemyCount(0)
    , m_score(0)
    , m_gameState("Ready")
    , m_isGameActive(false)
{
}

QString HUDViewModel::getLivesText() const
{
    return QString("Lives: x%1").arg(m_lives - 1); // 显示为x3, x2, x1, x0
}

QString HUDViewModel::getEnemyCountText() const
{
    return QString("Enemies: %1").arg(m_enemyCount);
}

QString HUDViewModel::getScoreText() const
{
    return QString("Score: %1").arg(m_score);
}

QString HUDViewModel::getGameStateText() const
{
    return m_gameState;
}

void HUDViewModel::updateLives(int lives)
{
    if (m_lives != lives) {
        m_lives = lives;
        emit livesChanged(m_lives);
        emitHUDDataChanged();
    }
}

void HUDViewModel::updateEnemyCount(int count)
{
    if (m_enemyCount != count) {
        m_enemyCount = count;
        emit enemyCountChanged(m_enemyCount);
        emitHUDDataChanged();
    }
}

void HUDViewModel::updateScore(int score)
{
    if (m_score != score) {
        m_score = score;
        emit scoreChanged(m_score);
        emitHUDDataChanged();
    }
}

void HUDViewModel::setGameState(const QString& state)
{
    if (m_gameState != state) {
        m_gameState = state;
        emit gameStateChanged(m_gameState);
        emitHUDDataChanged();
    }
}

void HUDViewModel::reset()
{
    m_lives = 4;
    m_enemyCount = 0;
    m_score = 0;
    m_gameState = "Ready";
    m_isGameActive = false;
    
    emit livesChanged(m_lives);
    emit enemyCountChanged(m_enemyCount);
    emit scoreChanged(m_score);
    emit gameStateChanged(m_gameState);
    emitHUDDataChanged();
}

void HUDViewModel::emitHUDDataChanged()
{
    emit hudDataChanged();
} 
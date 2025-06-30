#include "../precomp.h"
#include "../include/view/AudioEventListener.h"
#include "../include/viewmodel/GameViewModel.h"
#include "../include/viewmodel/PlayerViewModel.h"
#include "../include/view/AudioManager.h"
#include <QObject>
#include <QDebug>
#include <QString>

AudioEventListener::AudioEventListener(QObject *parent)
    : QObject(parent)
    , m_gameViewModel(nullptr)
    , m_playerViewModel(nullptr)
{
}

void AudioEventListener::setGameViewModel(GameViewModel* gameViewModel)
{
    // 如果之前有连接，先断开
    if (m_gameViewModel) {
        disconnectGameEvents();
    }
    
    m_gameViewModel = gameViewModel;
    
    // 如果设置了新的ViewModel，自动开始监听
    if (m_gameViewModel) {
        m_playerViewModel = m_gameViewModel->getPlayer();
        startListening();
    }
}

void AudioEventListener::startListening()
{
    if (!m_gameViewModel) {
        qWarning() << "AudioEventListener: 无法开始监听，GameViewModel未设置";
        return;
    }
    
    connectGameEvents();
    qDebug() << "AudioEventListener: 开始监听游戏事件";
}

void AudioEventListener::stopListening()
{
    disconnectGameEvents();
    qDebug() << "AudioEventListener: 停止监听游戏事件";
}

void AudioEventListener::connectGameEvents()
{
    if (!m_gameViewModel) return;
    
    // 连接实际存在的游戏事件信号
    connect(m_gameViewModel, &GameViewModel::playerDied, this, &AudioEventListener::onPlayerHit);
    connect(m_gameViewModel, &GameViewModel::playerLivesChanged, this, &AudioEventListener::onPlayerHit);
    
    // 连接玩家事件信号
    if (m_playerViewModel) {
        connect(m_playerViewModel, &PlayerViewModel::positionChanged, this, &AudioEventListener::onPlayerMove);
    }
}

void AudioEventListener::disconnectGameEvents()
{
    if (!m_gameViewModel) return;
    
    // 断开实际存在的游戏事件信号
    disconnect(m_gameViewModel, &GameViewModel::playerDied, this, &AudioEventListener::onPlayerHit);
    disconnect(m_gameViewModel, &GameViewModel::playerLivesChanged, this, &AudioEventListener::onPlayerHit);
    
    // 断开玩家事件信号
    if (m_playerViewModel) {
        disconnect(m_playerViewModel, &PlayerViewModel::positionChanged, this, &AudioEventListener::onPlayerMove);
    }
}

// 游戏事件音效槽函数实现
void AudioEventListener::onPlayerHit()
{
    qDebug() << "AudioEventListener: 播放玩家受伤音效";
    AudioManager::instance().playSound(AudioManager::PLAYER_HURT);
}

// 玩家事件音效槽函数实现
void AudioEventListener::onPlayerMove()
{
    qDebug() << "AudioEventListener: 播放玩家移动音效";
    // 移动音效可以降低频率，避免过于频繁
    static int moveCounter = 0;
    if (++moveCounter % 5 == 0) {  // 每5次移动播放一次
        AudioManager::instance().playSound(AudioManager::FOOTSTEP);
    }
} 
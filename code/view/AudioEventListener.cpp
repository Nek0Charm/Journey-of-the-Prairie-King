#include "../precomp.h"
#include "../include/view/AudioEventListener.h"
#include "../include/viewmodel/GameViewModel.h"
#include "../include/viewmodel/PlayerViewModel.h"
#include "../include/view/AudioManager.h"
#include "../include/view/GameWidget.h"
#include <QObject>
#include <QDebug>
#include <QString>
#include <QPointF>
/**
 * @file AudioEventListener.cpp
 * @brief 音频事件监听器
 * 负责监听相关事件，调用audiomanager播放音效
 */

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
    if (!m_gameViewModel) {
        qDebug() << "[DEBUG] connectGameEvents: m_gameViewModel is null";
        return;
    }
    
    qDebug() << "[DEBUG] connectGameEvents: 开始连接游戏事件信号";
    
    // 连接实际存在的游戏事件信号
    bool connected1 = connect(m_gameViewModel, &GameViewModel::playerDied, this, &AudioEventListener::onPlayerHit);
    bool connected2 = connect(m_gameViewModel, &GameViewModel::playerLivesChanged, this, &AudioEventListener::onPlayerHit);
    
    qDebug() << "[DEBUG] GameViewModel信号连接结果: playerDied=" << connected1 << ", playerLivesChanged=" << connected2;
    
    // 连接玩家事件信号
    if (m_playerViewModel) {
        qDebug() << "[DEBUG] m_playerViewModel存在，准备连接positionChanged信号";
        qDebug() << "[DEBUG] m_playerViewModel地址:" << m_playerViewModel;
        qDebug() << "[DEBUG] this地址:" << this;
        
        bool connected3 = connect(m_playerViewModel, &PlayerViewModel::positionChanged, this, &AudioEventListener::onPlayerMove);
        qDebug() << "[DEBUG] positionChanged信号连接结果:" << connected3;
        if (connected3) {
            qDebug() << "[DEBUG] positionChanged信号连接成功！";
        } else {
            qDebug() << "[DEBUG] positionChanged信号连接失败！";
        }
        // 新增：连接shot信号到onPlayerShot槽
        bool connected4 = connect(m_playerViewModel, &PlayerViewModel::shot, this, &AudioEventListener::onPlayerShot);
        qDebug() << "[DEBUG] shot信号连接结果:" << connected4;
        if (connected4) {
            qDebug() << "[DEBUG] shot信号连接成功！";
        } else {
            qDebug() << "[DEBUG] shot信号连接失败！";
        }
    } else {
        qDebug() << "[DEBUG] m_playerViewModel为null，无法连接positionChanged信号";
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
    AudioManager::instance().playSound(PLAYER_HURT);
}

// 玩家事件音效槽函数实现
void AudioEventListener::onPlayerMove()
{
    qDebug() << "[DEBUG] onPlayerMove called";
    qDebug() << "AudioEventListener: 播放玩家移动音效";
    static int moveCounter = 0;
    if (++moveCounter % 5 == 0) {
        AudioManager::instance().playSound(FOOTSTEP);
    }
}

// 射击事件音效槽函数实现
void AudioEventListener::onPlayerShot(const QPointF& direction)
{
    qDebug() << "AudioEventListener: 播放射击音效";
    AudioManager::instance().playSound(SHOOT);
} 
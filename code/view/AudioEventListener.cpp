#include "../include/view/AudioEventListener.h"
#include "../include/viewmodel/GameViewModel.h"
#include "../include/viewmodel/PlayerViewModel.h"
#include "../include/viewmodel/EnemyManager.h"
#include "../include/view/AudioManager.h"
#include "../include/view/GameWidget.h"
// 音频事件监听器实现 - 负责监听游戏事件信号并调用AudioManager播放相应的音效
// 遵循MVVM架构，只监听ViewModel层的信号

// 构造函数 - 初始化音频事件监听器，设置默认音量配置
AudioEventListener::AudioEventListener(QObject *parent)
    : QObject(parent)
    , m_gameViewModel(nullptr)
    , m_playerViewModel(nullptr)
{
    // 只调整爆炸音效的音量，其他音效保持默认
    AudioManager::instance().setSoundTypeVolume(EXPLOSION, 12); 
    // 调整背景音乐音量
    AudioManager::instance().setMusicVolume(25);  
}

// 设置游戏视图模型 - 如果之前有连接，先断开旧连接，设置新的ViewModel并自动开始监听
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

// 开始监听游戏事件 - 连接所有游戏事件信号到对应的音效播放槽函数
void AudioEventListener::startListening()
{
    if (!m_gameViewModel) {
        qWarning() << "AudioEventListener: 无法开始监听，GameViewModel未设置";
        return;
    }
    
    connectGameEvents();
    qDebug() << "AudioEventListener: 开始监听游戏事件";
}

// 停止监听游戏事件 - 断开所有游戏事件信号连接
void AudioEventListener::stopListening()
{
    disconnectGameEvents();
    qDebug() << "AudioEventListener: 停止监听游戏事件";
}

// 连接所有游戏事件信号 - 将游戏事件信号连接到对应的音效播放槽函数
void AudioEventListener::connectGameEvents()
{
    if (!m_gameViewModel) {
        return;
    }
    
    // 连接实际存在的游戏事件信号
    bool connected1 = connect(m_gameViewModel, &GameViewModel::playerDied, this, &AudioEventListener::onPlayerHit);
    bool connected2 = connect(m_gameViewModel, &GameViewModel::playerLivesDown, this, &AudioEventListener::onPlayerHit);
    
    // 连接敌人爆炸事件信号
    bool connected6 = connect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemyDestroyed, this, &AudioEventListener::onEnemyExplosion);
    
    // 新增：连接游戏状态变化信号
    bool connected5 = connect(m_gameViewModel, &GameViewModel::gameStateChanged, this, &AudioEventListener::onGameStateChanged);
    
    // 连接玩家事件信号
    if (m_playerViewModel) {
        bool connected3 = connect(m_playerViewModel, &PlayerViewModel::positionChanged, this, &AudioEventListener::onPlayerMove);
        // 新增：连接shot信号到onPlayerShot槽
        bool connected4 = connect(m_playerViewModel, &PlayerViewModel::shot, this, &AudioEventListener::onPlayerShot);
    }
    
    qDebug() << "AudioEventListener: 开始监听游戏事件";
}

// 断开所有游戏事件信号 - 断开所有已连接的游戏事件信号
void AudioEventListener::disconnectGameEvents()
{
    if (!m_gameViewModel) return;
    
    // 断开实际存在的游戏事件信号
    disconnect(m_gameViewModel, &GameViewModel::playerDied, this, &AudioEventListener::onPlayerHit);
    disconnect(m_gameViewModel, &GameViewModel::playerLivesDown, this, &AudioEventListener::onPlayerHit);
    
    // 断开敌人爆炸事件信号
    if (m_gameViewModel->getEnemyManager()) {
        disconnect(m_gameViewModel->getEnemyManager(), &EnemyManager::enemyDestroyed, this, &AudioEventListener::onEnemyExplosion);
    }
    
    // 断开玩家事件信号
    if (m_playerViewModel) {
        disconnect(m_playerViewModel, &PlayerViewModel::positionChanged, this, &AudioEventListener::onPlayerMove);
        disconnect(m_playerViewModel, &PlayerViewModel::shot, this, &AudioEventListener::onPlayerShot);
    }
}

// 游戏事件音效槽函数实现
void AudioEventListener::onPlayerHit()
{
    AudioManager::instance().playSound(PLAYER_HURT);
}

// 玩家事件音效槽函数实现
void AudioEventListener::onPlayerMove(const QPointF& position)
{
    static int moveCounter = 0;
    if (++moveCounter % 5 == 0) {
        AudioManager::instance().playSound(FOOTSTEP);
    }
}

// 射击事件音效槽函数实现
void AudioEventListener::onPlayerShot(const QPointF& direction)
{
    AudioManager::instance().playSound(SHOOT);
}

// 游戏状态变化事件音效槽函数实现
void AudioEventListener::onGameStateChanged(GameState state)
{
    // 根据游戏状态变化播放相应的背景音乐
    switch (state) {
        case GameState::MENU:
            qDebug() << "AudioEventListener: 播放菜单音乐";
            AudioManager::instance().playMusic(MENU);
            break;
        case GameState::PLAYING:
            qDebug() << "AudioEventListener: 播放游戏音乐";
            AudioManager::instance().playMusic(OVERWORLD);
            break;
        case GameState::PAUSED:
            qDebug() << "AudioEventListener: 暂停音乐";
            AudioManager::instance().pauseMusic();
            break;
        case GameState::GAME_OVER:
            qDebug() << "AudioEventListener: 播放游戏结束音乐";
            AudioManager::instance().playMusic(THE_OUTLAW);
            break;
    }
}

// 敌人爆炸事件音效槽函数实现
void AudioEventListener::onEnemyExplosion(int enemyId, const QPointF& position)
{
    AudioManager::instance().playSound(EXPLOSION);
} 
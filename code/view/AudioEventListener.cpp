#include "view/AudioEventListener.h"
#include "view/AudioManager.h"
#include "view/GameWidget.h"
// 音频事件监听器实现 - 负责监听游戏事件信号并调用AudioManager播放相应的音效
// 遵循MVVM架构，只监听ViewModel层的信号

// 构造函数 - 初始化音频事件监听器，设置默认音量配置
AudioEventListener::AudioEventListener(QObject *parent)
    : QObject(parent)
{
    // 只调整爆炸音效的音量，其他音效保持默认
    AudioManager::instance().setSoundTypeVolume(EXPLOSION, 12); 
    // 调整背景音乐音量
    AudioManager::instance().setMusicVolume(25);  
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
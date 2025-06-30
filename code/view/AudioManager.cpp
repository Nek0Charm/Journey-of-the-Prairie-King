#include "../precomp.h"
#include "../include/view/AudioManager.h"
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QMap>
#include <memory>

AudioManager::AudioManager(QObject *parent)
    : QObject(parent)
    , m_musicPlayer(std::make_unique<QMediaPlayer>())
    , m_audioOutput(new QAudioOutput(this))
{
    m_musicPlayer->setAudioOutput(m_audioOutput);
    connect(m_musicPlayer.get(), &QMediaPlayer::playbackStateChanged, this, &AudioManager::onMusicPlaybackStateChanged);
}

AudioManager& AudioManager::instance()
{
    static AudioManager instance;
    return instance;
}

bool AudioManager::initialize()
{
    qDebug() << "AudioManager::initialize() - Start";
    
    // 加载音效
    loadSoundEffects();
    
    // 加载音乐
    loadMusic();
    
    qDebug() << "AudioManager::initialize() - Success";
    return true;
}

void AudioManager::loadSoundEffects()
{
    qDebug() << "Loading sound effects...";
    
    // 创建音效映射
    QMap<SoundType, QString> soundPaths = {
        {SHOOT, ":/assert/sound/Cowboy_gunshot.mp3"},
        {HIT, ":/assert/sound/cowboy_monsterhit.mp3"},
        {ENEMY_DEATH, ":/assert/sound/cowboy_gopher.mp3"},
        {PLAYER_HURT, ":/assert/sound/cowboy_dead.mp3"},
        {POWERUP, ":/assert/sound/cowboy_powerup.mp3"},
        {EXPLOSION, ":/assert/sound/cowboy_explosion.mp3"},
        {FOOTSTEP, ":/assert/sound/Cowboy_Footstep.mp3"},
        {GUNLOAD, ":/assert/sound/cowboy_gunload.mp3"},
        {SECRET, ":/assert/sound/Cowboy_Secret.mp3"},
        {UNDEAD, ":/assert/sound/cowboy_undead.mp3"},
        {COIN_PICKUP, ":/assert/sound/Pickup_Coin15.mp3"},
        {BUTTON_CLICK, ":/assert/sound/Cowboy_gunshot.mp3"} // 使用射击音效作为按钮音效
    };
    
    // 加载每个音效
    for (auto it = soundPaths.begin(); it != soundPaths.end(); ++it) {
        auto soundEffect = new QSoundEffect();
        soundEffect->setSource(QUrl(it.value()));
        soundEffect->setVolume(m_soundVolume / 100.0);
        soundEffect->setLoopCount(1);
        
        if (soundEffect->status() == QSoundEffect::Error) {
             qWarning("Failed to load sound effect: %s, error: %d", 
                    it.value().toStdString().c_str(), 
                    soundEffect->status());
            delete soundEffect;
        } else {
            m_soundEffects[it.key()] = soundEffect;
            qDebug() << "Loaded sound effect:" << it.value();
        }
    }
    
    qDebug() << "Loaded" << m_soundEffects.size() << "sound effects";
}

void AudioManager::loadMusic()
{
    qDebug() << "Loading music...";
    
    // 设置音乐播放器
    m_musicPlayer->setSource(QUrl(getMusicPath(m_currentMusic)));

    if (m_musicPlayer->error() != QMediaPlayer::NoError) {
        qFatal("Failed to load music. Error: %d, Message: %s", 
               m_musicPlayer->error(), 
               m_musicPlayer->errorString().toStdString().c_str());
    }
}

void AudioManager::playSound(SoundType type)
{
    if (!m_soundEnabled || m_muted) {
        return;
    }
    
    auto it = m_soundEffects.find(type);
    if (it != m_soundEffects.end() && it.value()) {
        it.value()->play();
        qDebug() << "Playing sound:" << type;
    } else {
        qWarning() << "Sound effect not found:" << type;
    }
}

void AudioManager::stopSound(SoundType type)
{
    auto it = m_soundEffects.find(type);
    if (it != m_soundEffects.end() && it.value()) {
        it.value()->stop();
    }
}

void AudioManager::setSoundVolume(int volume)
{
    m_soundVolume = qBound(0, volume, 100);
    
    // 更新所有音效的音量
    for (auto it = m_soundEffects.begin(); it != m_soundEffects.end(); ++it) {
        if (it.value()) {
            it.value()->setVolume(m_soundVolume / 100.0);
        }
    }
    
    emit soundVolumeChanged(m_soundVolume);
    qDebug() << "Sound volume set to:" << m_soundVolume;
}

int AudioManager::getSoundVolume() const
{
    return m_soundVolume;
}

void AudioManager::playMusic(MusicType type)
{
    if (!m_musicEnabled || m_muted) {
        return;
    }
    
    QString musicPath = getMusicPath(type);
    if (musicPath.isEmpty()) {
        qWarning() << "Music path not found for type:" << type;
        return;
    }
    
    // 如果当前正在播放相同的音乐，不重复播放
    if (m_currentMusic == type && m_musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
        return;
    }
    
    m_currentMusic = type;
    m_musicPlayer->setSource(QUrl(musicPath));
    m_musicPlayer->play();
    
    qDebug() << "Playing music:" << musicPath;
}

void AudioManager::stopMusic()
{
    m_musicPlayer->stop();
    qDebug() << "Music stopped";
}

void AudioManager::pauseMusic()
{
    m_musicPlayer->pause();
    qDebug() << "Music paused";
}

void AudioManager::resumeMusic()
{
    if (m_musicEnabled && !m_muted) {
        m_musicPlayer->play();
        qDebug() << "Music resumed";
    }
}

void AudioManager::setMusicVolume(int volume)
{
    m_musicVolume = qBound(0, volume, 100);
    m_audioOutput->setVolume(m_musicVolume / 100.0f);
    
    emit musicVolumeChanged(m_musicVolume);
    qDebug() << "Music volume set to:" << m_musicVolume;
}

int AudioManager::getMusicVolume() const
{
    return m_musicVolume;
}

void AudioManager::setMuted(bool muted)
{
    m_muted = muted;
    
    if (muted) {
        // 静音时停止音乐
        if (m_musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            m_musicPlayer->pause();
        }
    } else {
        // 取消静音时恢复音乐
        if (m_musicEnabled && m_musicPlayer->playbackState() == QMediaPlayer::PausedState) {
            m_musicPlayer->play();
        }
    }
    
    emit mutedChanged(m_muted);
    qDebug() << "Muted set to:" << m_muted;
}

bool AudioManager::isMuted() const
{
    return m_muted;
}

void AudioManager::setSoundEnabled(bool enabled)
{
    m_soundEnabled = enabled;
    emit soundEnabledChanged(m_soundEnabled);
    qDebug() << "Sound enabled set to:" << m_soundEnabled;
}

void AudioManager::setMusicEnabled(bool enabled)
{
    m_musicEnabled = enabled;
    
    if (!enabled) {
        m_musicPlayer->pause();
    } else if (!m_muted) {
        m_musicPlayer->play();
    }
    
    emit musicEnabledChanged(m_musicEnabled);
    qDebug() << "Music enabled set to:" << m_musicEnabled;
}

bool AudioManager::isSoundEnabled() const
{
    return m_soundEnabled;
}

bool AudioManager::isMusicEnabled() const
{
    return m_musicEnabled;
}

QString AudioManager::getSoundPath(SoundType type) const
{
    QMap<SoundType, QString> soundPaths = {
        {SHOOT, ":/assert/sound/Cowboy_gunshot.mp3"},
        {HIT, ":/assert/sound/cowboy_monsterhit.mp3"},
        {ENEMY_DEATH, ":/assert/sound/cowboy_gopher.mp3"},
        {PLAYER_HURT, ":/assert/sound/cowboy_dead.mp3"},
        {POWERUP, ":/assert/sound/cowboy_powerup.mp3"},
        {EXPLOSION, ":/assert/sound/cowboy_explosion.mp3"},
        {FOOTSTEP, ":/assert/sound/Cowboy_Footstep.mp3"},
        {GUNLOAD, ":/assert/sound/cowboy_gunload.mp3"},
        {SECRET, ":/assert/sound/Cowboy_Secret.mp3"},
        {UNDEAD, ":/assert/sound/cowboy_undead.mp3"},
        {COIN_PICKUP, ":/assert/sound/Pickup_Coin15.mp3"},
        {BUTTON_CLICK, ":/assert/sound/Cowboy_gunshot.mp3"}
    };
    
    return soundPaths.value(type, QString());
}

QString AudioManager::getMusicPath(MusicType type) const
{
    QMap<MusicType, QString> musicPaths = {
        {OVERWORLD, ":/assert/music/Overworld.mp3"},
        {THE_OUTLAW, ":/assert/music/TheOutlaw.mp3"},
        {FINAL_BOSS, ":/assert/music/FinalBossEnding.mp3"},
        {MENU, ":/assert/music/Overworld.mp3"}
    };
    
    return musicPaths.value(type, QString());
}

void AudioManager::onMusicPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState) {
        if (m_musicPlayer->position() > 0) { // 确保不是因为错误而停止
            m_musicPlayer->play();
        }
    }
}

AudioManager::~AudioManager()
{
    qDeleteAll(m_soundEffects);
    m_soundEffects.clear();
    
    // m_musicPlayer 是 unique_ptr，会自动清理
    qDebug() << "AudioManager destroyed";
} 
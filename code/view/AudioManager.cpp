#include "../precomp.h"
#include "../include/view/AudioManager.h"
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QMap>
#include <memory>
#include <QFile>

AudioManager::AudioManager(QObject *parent)
    : QObject(parent)
    , m_musicPlayer(std::make_shared<QMediaPlayer>())
    , m_audioOutput(new QAudioOutput(this))
{
    m_musicPlayer->setAudioOutput(m_audioOutput);
    connect(m_musicPlayer.get(), &QMediaPlayer::playbackStateChanged, 
            this, [this](QMediaPlayer::PlaybackState state) {
                onMusicPlaybackStateChanged(static_cast<int>(state));
            });
    connect(m_musicPlayer.get(), &QMediaPlayer::errorOccurred, 
            this, [this](QMediaPlayer::Error error, const QString& errorString) {
                onMediaPlayerError(static_cast<int>(error), errorString);
            });
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
    
    // 清理旧的音效播放器
    m_soundPlayers.clear();
    
    // 为每个音效类型创建独立的播放器
    for (int i = SHOOT; i <= BUTTON_CLICK; ++i) {
        SoundType type = static_cast<SoundType>(i);
        QString path = getSoundPath(type);
        if (path.isEmpty()) {
            qWarning() << "No path for sound type" << type;
            continue;
        }

        // 检查文件是否存在
        QFile file(path);
        if (!file.exists()) {
            qWarning() << "Sound file does not exist:" << path;
            continue;
        }

        // 检查文件大小
        qint64 fileSize = file.size();
        qDebug() << "Sound file size for type" << type << ":" << fileSize << "bytes";

        // 创建新的播放器和音频输出
        auto soundPlayer = std::make_shared<SoundPlayer>();
        soundPlayer->player = std::make_shared<QMediaPlayer>();
        soundPlayer->output = std::make_shared<QAudioOutput>();
        
        // 设置音频输出
        soundPlayer->player->setAudioOutput(soundPlayer->output.get());
        soundPlayer->output->setVolume(m_soundVolume / 100.0f);
        
        // 设置音频源
        QUrl url = QUrl::fromLocalFile(path);
        soundPlayer->player->setSource(url);
        
        // 检查设置后的状态
        qDebug() << "Created sound player for type:" << type 
                 << "from path:" << path
                 << "source:" << soundPlayer->player->source()
                 << "error:" << soundPlayer->player->error();
        
        m_soundPlayers[type] = std::move(soundPlayer);
    }
    
    qDebug() << "Created" << m_soundPlayers.size() << "sound players";
}

void AudioManager::loadMusic()
{
    qDebug() << "Loading music...";
    
    m_musicPlayer->setSource(QUrl::fromLocalFile(getMusicPath(m_currentMusic)));
}

void AudioManager::playSound(SoundType type)
{
    qDebug() << "[playSound] called, type:" << type
             << "enabled:" << m_soundEnabled
             << "muted:" << m_muted
             << "volume:" << m_soundVolume;
    if (!m_soundEnabled || m_muted) {
        qDebug() << "[playSound] skipped due to disabled or muted.";
        return;
    }
    auto it = m_soundPlayers.find(type);
    if (it != m_soundPlayers.end() && it.value()) {
        auto& soundPlayer = it.value();
        
        // 检查音频输出状态
        if (soundPlayer->output) {
            qDebug() << "[playSound] Audio output volume:" << soundPlayer->output->volume()
                     << "muted:" << soundPlayer->output->isMuted();
        } else {
            qWarning() << "[playSound] Audio output is null!";
        }
        
        // 检查播放器状态
        qDebug() << "[playSound] Player state before:" << soundPlayer->player->playbackState()
                 << "error:" << soundPlayer->player->error()
                 << "source:" << soundPlayer->player->source();
        
        // 如果正在播放，先停止再播放
        if (soundPlayer->player->playbackState() == QMediaPlayer::PlayingState) {
            soundPlayer->player->stop();
        }
        
        // 重新设置源文件以确保从头播放
        soundPlayer->player->setPosition(0);
        soundPlayer->player->play();
        
        qDebug() << "[playSound] triggered play, QMediaPlayer state:" << soundPlayer->player->playbackState()
                 << "error:" << soundPlayer->player->error();
                 
        // 检查播放后的状态
        if (soundPlayer->player->error() != QMediaPlayer::NoError) {
            qWarning() << "[playSound] Playback error:" << soundPlayer->player->errorString();
        }
    } else {
        qWarning() << "[playSound] Sound player not found for type:" << type;
    }
}

void AudioManager::stopSound(SoundType type)
{
    auto it = m_soundPlayers.find(type);
    if (it != m_soundPlayers.end() && it.value()) {
        it.value()->player->stop();
    }
}

void AudioManager::setSoundVolume(int volume)
{
    m_soundVolume = qBound(0, volume, 100);
    
    // 更新所有音效播放器的音量
    for (auto& soundPlayer : m_soundPlayers) {
        if (soundPlayer && soundPlayer->output) {
            soundPlayer->output->setVolume(m_soundVolume / 100.0f);
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
    qDebug() << "[playMusic] called, type:" << type
             << "enabled:" << m_musicEnabled
             << "muted:" << m_muted
             << "volume:" << m_musicVolume;
    if (!m_musicEnabled || m_muted) {
        qDebug() << "[playMusic] skipped due to disabled or muted.";
        return;
    }
    QString musicPath = getMusicPath(type);
    if (musicPath.isEmpty()) {
        qWarning() << "[playMusic] Music path not found for type:" << type;
        return;
    }
    if (m_currentMusic == type && m_musicPlayer->playbackState() != QMediaPlayer::StoppedState) {
        qDebug() << "[playMusic] already playing this music.";
        return;
    }
    m_currentMusic = type;
    m_musicPlayer->setSource(QUrl::fromLocalFile(musicPath));
    m_musicPlayer->play();
    qDebug() << "[playMusic] triggered play, QMediaPlayer state:" << m_musicPlayer->playbackState()
             << "error:" << m_musicPlayer->error();
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
        if (m_musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            m_musicPlayer->pause();
        }
    } else {
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
    // 获取项目根目录（可执行文件的上级目录）
    QString appDir = QCoreApplication::applicationDirPath();
    QString projectRoot = appDir;
    
    // 如果在Debug或Release子目录中，需要向上两级到项目根目录
    if (projectRoot.contains("/Debug") || projectRoot.contains("/Release") || 
        projectRoot.contains("\\Debug") || projectRoot.contains("\\Release")) {
        QDir dir(projectRoot);
        if (dir.cdUp()) {
            if (dir.cdUp()) {
                projectRoot = dir.absolutePath();
            }
        }
    }
    
    QString basePath = projectRoot + "/assert/sound";
    
    // 添加调试信息
    qDebug() << "Sound base path:" << basePath;

    switch (type) {
        case SHOOT:       return basePath + "/Cowboy_gunshot.wav";
        case HIT:         return basePath + "/cowboy_monsterhit.wav";
        case ENEMY_DEATH: return basePath + "/cowboy_gopher.wav";
        case PLAYER_HURT: return basePath + "/cowboy_dead.wav";
        case POWERUP:     return basePath + "/cowboy_powerup.wav";
        case EXPLOSION:   return basePath + "/cowboy_explosion.wav";
        case FOOTSTEP:    return basePath + "/Cowboy_Footstep.wav";
        case GUNLOAD:     return basePath + "/cowboy_gunload.wav";
        case SECRET:      return basePath + "/Cowboy_Secret.wav";
        case UNDEAD:      return basePath + "/cowboy_undead.wav";
        case COIN_PICKUP: return basePath + "/Pickup_Coin15.wav";
        case BUTTON_CLICK:return basePath + "/Cowboy_gunshot.wav"; 
        default:          return "";
    }
}

QString AudioManager::getMusicPath(MusicType type) const
{
    // 获取项目根目录（可执行文件的上级目录）
    QString appDir = QCoreApplication::applicationDirPath();
    QString projectRoot = appDir;
    
    // 如果在Debug或Release子目录中，需要向上两级到项目根目录
    if (projectRoot.contains("/Debug") || projectRoot.contains("/Release") || 
        projectRoot.contains("\\Debug") || projectRoot.contains("\\Release")) {
        QDir dir(projectRoot);
        if (dir.cdUp()) {
            if (dir.cdUp()) {
                projectRoot = dir.absolutePath();
            }
        }
    }
    
    QString basePath = projectRoot + "/assert/music";
    
    // 添加调试信息
    qDebug() << "Music base path:" << basePath;

    switch (type) {
        case OVERWORLD:   return basePath + "/Overworld.wav";
        case THE_OUTLAW:  return basePath + "/TheOutlaw.wav";
        case FINAL_BOSS:  return basePath + "/FinalBossEnding.wav";
        case MENU:        return basePath + "/Overworld.wav"; 
        default:          return "";
    }
}

void AudioManager::onMusicPlaybackStateChanged(int state)
{
    auto playbackState = static_cast<QMediaPlayer::PlaybackState>(state);
    if (playbackState == QMediaPlayer::StoppedState) {
        qDebug() << "Music finished, replaying.";
        m_musicPlayer->play();
    } else if (playbackState == QMediaPlayer::PlayingState) {
        m_musicLoaded = true;
    }
}

void AudioManager::onMediaPlayerError(int error, const QString &errorString)
{
    qWarning() << "Music player error:" << error << errorString;
    // 尝试重新加载
    if (!errorString.contains("internal service error")) {
        playMusic(m_currentMusic);
    }
}

AudioManager::~AudioManager()
{
    // 停止所有音效
    for (auto& soundPlayer : m_soundPlayers) {
        if (soundPlayer && soundPlayer->player) {
            soundPlayer->player->stop();
        }
    }
    
    // 清理音效播放器
    m_soundPlayers.clear();
    
    // m_musicPlayer 是 unique_ptr，会自动清理
    qDebug() << "AudioManager destroyed";
} 
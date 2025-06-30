#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <memory>
#include <QMediaPlayer>

// 前向声明
class QSoundEffect;
class QAudioOutput;

class AudioManager : public QObject
{
    Q_OBJECT

public:
    // 音效类型枚举
    enum SoundType {
        SHOOT, HIT, ENEMY_DEATH, PLAYER_HURT, POWERUP, EXPLOSION,
        FOOTSTEP, GUNLOAD, SECRET, UNDEAD, COIN_PICKUP, BUTTON_CLICK
    };

    // 音乐类型枚举
    enum MusicType {
        OVERWORLD, THE_OUTLAW, FINAL_BOSS, MENU
    };

    static AudioManager& instance();

    // 音效控制
    void playSound(SoundType type);
    void stopSound(SoundType type);
    void setSoundVolume(int volume);
    int getSoundVolume() const;

    // 音乐控制
    void playMusic(MusicType type);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume);
    int getMusicVolume() const;

    // 全局控制
    void setMuted(bool muted);
    bool isMuted() const;
    void setSoundEnabled(bool enabled);
    void setMusicEnabled(bool enabled);
    bool isSoundEnabled() const;
    bool isMusicEnabled() const;

    // 初始化
    bool initialize();

signals:
    void soundVolumeChanged(int volume);
    void musicVolumeChanged(int volume);
    void mutedChanged(bool muted);
    void soundEnabledChanged(bool enabled);
    void musicEnabledChanged(bool enabled);

private slots:
    void onMusicPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // 私有方法
    void loadSoundEffects();
    void loadMusic();
    QString getSoundPath(SoundType type) const;
    QString getMusicPath(MusicType type) const;

    // 成员变量
    QMap<SoundType, QSoundEffect*> m_soundEffects;
    std::unique_ptr<QMediaPlayer> m_musicPlayer;
    QAudioOutput* m_audioOutput; // 用于控制音乐音量
    
    // 设置
    int m_soundVolume = 80;
    int m_musicVolume = 60;
    bool m_muted = false;
    bool m_soundEnabled = true;
    bool m_musicEnabled = true;
    
    // 当前播放的音乐
    MusicType m_currentMusic = OVERWORLD;
    bool m_musicLoaded = false;
};

#endif // AUDIOMANAGER_H 
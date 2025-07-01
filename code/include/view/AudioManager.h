#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <memory>
#include <QMediaPlayer>

// Forward declarations
class QMediaPlayer;
class QAudioOutput;

// 音效类型枚举
enum SoundType {
    SHOOT = 0,        // 射击
    HIT,              // 命中
    ENEMY_DEATH,      // 敌人死亡
    PLAYER_HURT,      // 玩家受伤
    POWERUP,          // 道具拾取
    EXPLOSION,        // 爆炸
    FOOTSTEP,         // 脚步声
    GUNLOAD,          // 装弹
    SECRET,           // 秘密
    UNDEAD,           // 不死族
    COIN_PICKUP,      // 金币拾取
    BUTTON_CLICK      // 按钮点击
};

// 背景音乐类型枚举
enum MusicType {
    MENU = 0,         // 菜单音乐
    OVERWORLD,        // 主世界音乐
    THE_OUTLAW,       // 亡命之徒音乐
    FINAL_BOSS        // 最终Boss音乐
};

class AudioManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static AudioManager& instance();

    // 删除拷贝构造和赋值操作符
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // 初始化音频系统
    bool initialize();

    // 音效控制
    void playSound(SoundType type);
    void stopSound(SoundType type);
    void setSoundVolume(int volume);  // 0-100
    int getSoundVolume() const;

    // 背景音乐控制
    void playMusic(MusicType type);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume);  // 0-100
    int getMusicVolume() const;

    // 全局控制
    void setMuted(bool muted);
    bool isMuted() const;

    // 设置启用/禁用
    void setSoundEnabled(bool enabled);
    void setMusicEnabled(bool enabled);
    bool isSoundEnabled() const;
    bool isMusicEnabled() const;

signals:
    void soundVolumeChanged(int volume);
    void musicVolumeChanged(int volume);
    void mutedChanged(bool muted);
    void soundEnabledChanged(bool enabled);
    void musicEnabledChanged(bool enabled);

private slots:
    void onMusicPlaybackStateChanged(int state);
    void onMediaPlayerError(int error, const QString& errorString);

private:
    // 私有构造函数
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();

    // 加载音效和音乐
    void loadSoundEffects();
    void loadMusic();

    // 获取资源路径
    QString getSoundPath(SoundType type) const;
    QString getMusicPath(MusicType type) const;

    // 音效播放器管理 - 使用QMediaPlayer替代QSoundEffect
    struct SoundPlayer {
        std::shared_ptr<QMediaPlayer> player;
        std::shared_ptr<QAudioOutput> output;
    };
    QMap<SoundType, std::shared_ptr<SoundPlayer>> m_soundPlayers;

    // 背景音乐播放器
    std::shared_ptr<QMediaPlayer> m_musicPlayer;
    QAudioOutput* m_audioOutput = nullptr;

    // 音量设置
    int m_soundVolume = 70;
    int m_musicVolume = 10;

    // 静音和启用状态
    bool m_muted = false;
    bool m_soundEnabled = true;
    bool m_musicEnabled = true;

    // 当前播放的音乐
    MusicType m_currentMusic = OVERWORLD;
    bool m_musicLoaded = false;
};

#endif // AUDIOMANAGER_H 
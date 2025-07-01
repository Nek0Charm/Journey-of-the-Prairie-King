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
    SHOOT = 0,        // 射击音效
    HIT,              // 命中音效
    ENEMY_DEATH,      // 敌人死亡音效
    PLAYER_HURT,      // 玩家受伤音效
    POWERUP,          // 道具拾取音效
    EXPLOSION,        // 敌人爆炸音效
    FOOTSTEP,         // 脚步声
    GUNLOAD,          // 装弹音效
    SECRET,           // 
    UNDEAD,           // 
    COIN_PICKUP,      // 金币拾取音效
    BUTTON_CLICK      // 按钮点击音效
};

// 背景音乐类型枚举
enum MusicType {
    MENU = 0,         // 菜单背景音乐
    OVERWORLD,       
    THE_OUTLAW,       
    FINAL_BOSS       
};

// 音频管理器 - 单例模式，负责管理游戏中的所有音频资源
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
    void playSound(SoundType type);                    // 播放指定类型音效
    void stopSound(SoundType type);                    // 停止指定类型音效
    void setSoundVolume(int volume);                   // 设置全局音效音量 (0-100)
    int getSoundVolume() const;                        // 获取全局音效音量
    
    // 设置特定音效的音量
    void setSoundTypeVolume(SoundType type, int volume); // 设置特定音效音量 (0-100)
    int getSoundTypeVolume(SoundType type) const;        // 获取特定音效音量

    // 背景音乐控制
    void playMusic(MusicType type);                    // 播放指定类型背景音乐
    void stopMusic();                                  // 停止背景音乐
    void pauseMusic();                                 // 暂停背景音乐
    void resumeMusic();                                // 恢复背景音乐播放
    void setMusicVolume(int volume);                   // 设置背景音乐音量 (0-100)
    int getMusicVolume() const;                        // 获取背景音乐音量

    // 全局控制
    void setMuted(bool muted);                         // 设置全局静音
    bool isMuted() const;                              // 检查是否静音

    // 设置启用/禁用
    void setSoundEnabled(bool enabled);                // 启用/禁用音效
    void setMusicEnabled(bool enabled);                // 启用/禁用背景音乐
    bool isSoundEnabled() const;                       // 检查音效是否启用
    bool isMusicEnabled() const;                       // 检查背景音乐是否启用

signals:
    void soundVolumeChanged(int volume);               // 音效音量变化信号
    void musicVolumeChanged(int volume);               // 背景音乐音量变化信号
    void mutedChanged(bool muted);                     // 静音状态变化信号
    void soundEnabledChanged(bool enabled);            // 音效启用状态变化信号
    void musicEnabledChanged(bool enabled);            // 背景音乐启用状态变化信号

private slots:
    void onMusicPlaybackStateChanged(int state);       // 音乐播放状态变化处理
    void onMediaPlayerError(int error, const QString& errorString); // 媒体播放器错误处理

private:
    // 私有构造函数
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();

    // 加载音效和音乐
    void loadSoundEffects();                           // 加载所有音效文件
    void loadMusic();                                  // 加载背景音乐文件

    // 获取资源路径
    QString getSoundPath(SoundType type) const;        // 获取音效文件路径
    QString getMusicPath(MusicType type) const;        // 获取背景音乐文件路径

    // 音效播放器管理 - 使用QMediaPlayer替代QSoundEffect
    struct SoundPlayer {
        std::shared_ptr<QMediaPlayer> player;          // 媒体播放器
        std::shared_ptr<QAudioOutput> output;          // 音频输出
    };
    QMap<SoundType, std::shared_ptr<SoundPlayer>> m_soundPlayers; // 音效播放器映射

    // 背景音乐播放器
    std::shared_ptr<QMediaPlayer> m_musicPlayer;       // 背景音乐播放器
    QAudioOutput* m_audioOutput = nullptr;             // 背景音乐音频输出

    // 音量设置
    int m_soundVolume = 70;                            // 全局音效音量 (0-100)
    int m_musicVolume = 10;                            // 背景音乐音量 (0-100)

    // 静音和启用状态
    bool m_muted = false;                              // 全局静音状态
    bool m_soundEnabled = true;                        // 音效启用状态
    bool m_musicEnabled = true;                        // 背景音乐启用状态

    // 当前播放的音乐
    MusicType m_currentMusic = OVERWORLD;              // 当前播放的音乐类型
    bool m_musicLoaded = false;                        // 音乐是否已加载
};

#endif // AUDIOMANAGER_H 
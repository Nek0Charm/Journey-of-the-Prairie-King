#ifndef AUDIOEVENTLISTENER_H
#define AUDIOEVENTLISTENER_H

// 前向声明
class PlayerViewModel;

// 音频事件监听器 - 专门负责监听游戏事件信号并触发相应的音效播放
// 独立于GameWidget，保持MVVM架构的清晰性
class AudioEventListener : public QObject
{
    Q_OBJECT  // MOC: 重新生成元对象信息

public:
    explicit AudioEventListener(QObject *parent = nullptr);
    ~AudioEventListener() = default;
    
    // 设置游戏视图模型    
    // 开始监听游戏事件
    void startListening();
    
    // 停止监听游戏事件
    void stopListening();

public slots:
    // 游戏事件音效槽函数
    void onPlayerHit();                                // 玩家受伤音效
    
    // 玩家事件音效槽函数
    void onPlayerMove(const QPointF& position);        // 玩家移动音效（脚步声）
    
    // 射击事件音效槽函数
    void onPlayerShot(const QPointF& direction);       // 玩家射击音效
    
    // 敌人爆炸事件音效槽函数
    void onEnemyExplosion(int enemyId, const QPointF& position); // 敌人爆炸音效
    
    // 游戏状态变化槽函数
    void onGameStateChanged(GameState state); // 游戏状态变化（背景音乐）

private:
    
    // 连接所有游戏事件信号
    void connectGameEvents();
    
    // 断开所有游戏事件信号
    void disconnectGameEvents();
};

#endif // AUDIOEVENTLISTENER_H 
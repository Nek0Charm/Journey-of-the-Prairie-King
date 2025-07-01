#ifndef AUDIOEVENTLISTENER_H
#define AUDIOEVENTLISTENER_H

#include <QObject>
#include <QPointF>
#include "viewmodel/GameViewModel.h"

// 前向声明
class PlayerViewModel;

/**
 * @brief 音频事件监听器
 * 
 * 专门负责监听游戏事件信号并触发相应的音效播放
 * 独立于GameWidget，保持MVVM架构的清晰性
 */
class AudioEventListener : public QObject
{
    Q_OBJECT  // MOC: 重新生成元对象信息

public:
    explicit AudioEventListener(QObject *parent = nullptr);
    ~AudioEventListener() = default;
    
    /**
     * @brief 设置游戏视图模型
     * @param gameViewModel 游戏视图模型指针
     */
    void setGameViewModel(GameViewModel* gameViewModel);
    
    /**
     * @brief 开始监听游戏事件
     */
    void startListening();
    
    /**
     * @brief 停止监听游戏事件
     */
    void stopListening();

private slots:
    // 游戏事件音效槽函数
    void onPlayerHit();
    
    // 玩家事件音效槽函数
    void onPlayerMove(const QPointF& position);
    
    // 射击事件音效槽函数
    void onPlayerShot(const QPointF& direction);
    
    // 游戏状态变化槽函数
    void onGameStateChanged(GameViewModel::GameState state);

private:
    GameViewModel* m_gameViewModel;
    PlayerViewModel* m_playerViewModel;
    
    /**
     * @brief 连接所有游戏事件信号
     */
    void connectGameEvents();
    
    /**
     * @brief 断开所有游戏事件信号
     */
    void disconnectGameEvents();
};

#endif // AUDIOEVENTLISTENER_H 
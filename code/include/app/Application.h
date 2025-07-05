#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <view/MainWindow.h>
#include <viewmodel/GameViewModel.h>
#include <view/AudioEventListener.h>
#include <app/GameService.h>


class Application: public QApplication {
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application() override = default;
    int run();

private slots:
    void gameLoop();
    void onGameStateChanged();

private:
    void setupGameLoop();
    void calculateDeltaTime();
    void initalizeComponents();
    /*
     * 由于Application类继承自QApplication，不能够使用connect
     * 所以不能把connect写在Application中
    */

    std::unique_ptr<MainWindow> m_view;
    std::unique_ptr<GameViewModel> m_viewModel;
    std::unique_ptr<GameService> m_service;
    std::unique_ptr<AudioEventListener> m_audioEventListener;

    QTimer m_gameTimer;
    QElapsedTimer m_frameTimer;
    double m_deltaTime;
    qint64 m_lastFrameTime;

    static const int TARGET_FPS = 60; 
    static const int FRAME_INTERVAL = 1000 / TARGET_FPS;
};
#endif
#include "app/Application.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_gameTimer(this)
    , m_frameTimer()
    , m_deltaTime(0.0)
    , m_lastFrameTime(0) {
    setupGameLoop();
}


void Application::setupGameLoop() {
    m_viewModel = std::make_unique<GameViewModel>(this);
    m_view = std::make_unique<MainWindow>();
    /*
    TODO: 在MainWindow中设置游戏试图模型（可能需要）
    m_view->setGameViewModel(m_viewModel);
    */

    m_gameTimer.setInterval(FRAME_INTERVAL);
    m_gameTimer.setTimerType(Qt::PreciseTimer);
    /*
    QTimer: 闹钟，每隔FRAME_INTERVAL毫秒触发一次
    QElapsedTimer: 用于测量时间间隔，
        m_frameTimer->elapsed() - lastFrameTime才是真正的deltaTime
    */
    connect(&m_gameTimer, &QTimer::timeout, this, &Application::gameLoop);
    connect(m_viewModel.get(), &GameViewModel::gameStateChanged, this, &Application::onGameStateChanged);

    m_frameTimer.start();
    m_lastFrameTime = m_frameTimer.elapsed();
}

int Application::run() {
    m_view->show();
    m_gameTimer.start();
    return exec();
}

void Application::gameLoop() {
    calculateDeltaTime();

    if(m_viewModel) {
        m_viewModel->updateGame(m_deltaTime);
    }
    processEvents();
}

void Application::calculateDeltaTime() {
    qint64 currentTime = m_frameTimer.elapsed();
    m_deltaTime = (currentTime - m_lastFrameTime) / 1000.0; 
    m_lastFrameTime = currentTime;

    if (m_deltaTime > 1.0 / TARGET_FPS) {
        m_deltaTime = 1.0 / TARGET_FPS;
    }
}

void Application::onGameStateChanged() {
    return;
}
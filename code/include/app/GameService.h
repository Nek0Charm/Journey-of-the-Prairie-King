#ifndef __GAME_SERVICE_H__
#define __GAME_SERVICE_H__

#include "viewmodel/GameViewModel.h"
#include "view/MainWindow.h"
#include "view/GameWidget.h"
#include "view/AudioEventListener.h"

class GameService: public QObject{
    Q_OBJECT

    public:
    explicit GameService(MainWindow *mainWindow, GameViewModel *viewModel,AudioEventListener *audioEventListener, QObject *parent = nullptr);

    ~GameService() = default;

    void setupConnections();

    private:
    MainWindow *m_mainWindow;
    GameViewModel *m_gameViewModel;
    AudioEventListener *m_audioEventListener;
};

#endif
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
using namespace std;
class GameWidget;
class StartWidget;
class EndWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
    void setupUi();
    void setupConnections();
    void update();
    GameWidget *getGameWidget() const { return m_gameWidget; }
signals:
    void startGameRequested();
    void exitGameRequested();
public slots:
    void onStartGameRequested();
    void onExitGameRequested();
    void onRestartGameRequested();
    void onExitToMenuRequested();
    void onGameStateChanged(GameState state);
private:
    QStackedWidget *m_stackedWidget;
    GameWidget *m_gameWidget;
    StartWidget *m_startWidget;
    EndWidget *m_endWidget; 
    GameState m_gameState = GameState::MENU;
};
#endif // MAINWINDOW_H

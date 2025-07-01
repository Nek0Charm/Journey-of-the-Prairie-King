#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "viewmodel/GameViewModel.h"

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
    explicit MainWindow(GameViewModel *viewModel, QWidget *parent = nullptr);
    ~MainWindow() = default;
    void setupUi();
    void setupConnections();
    void update();
private slots:
    void onStartGameRequested();
    void onExitGameRequested();
    void onRestartGameRequested();
    void onExitToMenuRequested();
private:
    QStackedWidget *m_stackedWidget;
    GameWidget *m_gameWidget;
    StartWidget *m_startWidget;
    GameViewModel *m_gameViewModel;
    EndWidget *m_endWidget; 
};
#endif // MAINWINDOW_H

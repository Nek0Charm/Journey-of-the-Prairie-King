#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "viewmodel/GameViewModel.h"

#include <QMainWindow>
#include <QPainter>
#include <QColor>
#include <QString>
#include <QKeyEvent>
#include <iostream>
using namespace std;
class GameWidget;
class StartWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(GameViewModel *viewModel, QWidget *parent = nullptr);
    ~MainWindow() = default;
    void setupUi();
    void setupConnections();
private slots:
    void onStartGameRequested();
    void onExitGameRequested();
private:
    QStackedWidget *m_stackedWidget;
    GameWidget *m_gameWidget;
    StartWidget *m_startWidget;
    GameViewModel *m_gameViewModel;
};
#endif // MAINWINDOW_H

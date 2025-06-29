#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QColor>
#include <QString>
#include <QKeyEvent>
#include <iostream>
using namespace std;
class GameWidget; 

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

    bool initialize();
    
};
#endif // MAINWINDOW_H

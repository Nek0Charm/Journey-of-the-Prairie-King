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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    void paintEvent(QPaintEvent* painter1) override;
    void keyPressEvent(QKeyEvent* event) override;
private slots:

private:
    GameWidget *m_gameWidget;
};
#endif // MAINWINDOW_H

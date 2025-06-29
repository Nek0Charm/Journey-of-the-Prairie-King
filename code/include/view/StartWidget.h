#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QPushButton>
#include <QVBoxLayout> 

class QPushButton;
class QLabel;

class StartWidget : public QWidget
{
    Q_OBJECT // 别忘了 Q_OBJECT 宏，因为我们要用信号

public:
    explicit StartWidget(QWidget *parent = nullptr);
    ~StartWidget();

signals:
    // 定义两个信号，用来通知外部按钮被点击了
    void startGameClicked();
    void exitGameClicked();

private:
    QLabel *m_titleLabel;
    QPushButton *m_startButton;
    QPushButton *m_exitButton;
};

#endif // STARTSCREEN_H
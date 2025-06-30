#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QPushButton>
#include <QVBoxLayout> 

class QPushButton;
class QLabel;

class StartWidget : public QWidget {
    Q_OBJECT

public:
    explicit StartWidget(QWidget *parent = nullptr);
    ~StartWidget();

signals:
    void startGameClicked();
    void exitGameClicked();

private:
    QLabel *m_titleLabel;
    QPushButton *m_startButton;
    QPushButton *m_exitButton;
};

#endif // STARTSCREEN_H
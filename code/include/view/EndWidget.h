#ifndef ENDWIDGET_H
#define ENDWIDGET_H
#include <QPushButton>
#include <QVBoxLayout> 

class QPushButton;
class QLabel;

class EndWidget : public QWidget {
    Q_OBJECT

public:
    explicit EndWidget(QWidget *parent = nullptr);
    ~EndWidget();
signals:
    void restartGameClicked();
    void exitToMenuClicked(); 
private:
    QLabel *m_gameOverLabel; 
    QPushButton *m_restartButton;
    QPushButton *m_exitButton;
};
#endif
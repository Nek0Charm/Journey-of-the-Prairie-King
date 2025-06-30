#include "view/EndWidget.h"

EndWidget::EndWidget(QWidget *parent) : QWidget(parent)
{
    int scaleFactor = 2;
    this->setStyleSheet("background-color: black;"); 
    m_gameOverLabel = new QLabel(this);
    QPixmap gameOverPixmap(":/assert/gameover.png");
    if (gameOverPixmap.isNull()) {
        qWarning("错误：无法加载 gameover.png！");
        m_gameOverLabel->setText("GAME OVER");
    } else {
        QSize newSize = gameOverPixmap.size() * scaleFactor;
        QPixmap scaledPixmap = gameOverPixmap.scaled(newSize, 
                                                    Qt::KeepAspectRatio, 
                                                    Qt::FastTransformation);

        m_gameOverLabel->setPixmap(scaledPixmap);
        m_gameOverLabel->setFixedSize(scaledPixmap.size());
    }
    m_gameOverLabel->setAlignment(Qt::AlignCenter);
    int baseFontSize = 30; 
    int finalFontSize = baseFontSize * scaleFactor; 
    m_restartButton = new QPushButton("重新开始", this);
    m_exitButton = new QPushButton("退出游戏", this);
    m_restartButton->setMinimumHeight(50);
    m_exitButton->setMinimumHeight(50);
    QString buttonStyle = QString(
        "QPushButton {"
        "    color: white;"           
        "    font-size: %1px;"       
        "    font-weight: bold;"      
        "    background-color: black;" 
        "    border: none;"    
        "}"
        
        "QPushButton:hover {"           
        "    color: gray;"
        "}"
    ).arg(finalFontSize);
    m_restartButton->setStyleSheet(buttonStyle);
    m_exitButton->setStyleSheet(buttonStyle);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_gameOverLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(100);
    layout->addWidget(m_restartButton);
    layout->addWidget(m_exitButton);
    layout->addStretch();

    connect(m_restartButton, &QPushButton::clicked, this, &EndWidget::restartGameClicked);
    connect(m_exitButton, &QPushButton::clicked, this, &EndWidget::exitToMenuClicked);
}

EndWidget::~EndWidget() {}
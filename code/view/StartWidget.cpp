#include "StartWidget.h"

StartWidget::StartWidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("background-color: black;"); 

    m_titleLabel = new QLabel(this);    
    QPixmap fullSpriteSheet(":/assert/picture/sprite.png"); 
    QRect cropRect(0, 96, 94, 55);
    QPixmap titlePixmap = fullSpriteSheet.copy(cropRect);

    int baseFontSize = 6; 
    int scaleFactor = 10; 
    QSize newSize = titlePixmap.size() * scaleFactor;
    QPixmap scaledPixmap = titlePixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    m_titleLabel->setPixmap(scaledPixmap);
    m_titleLabel->setFixedSize(scaledPixmap.size());

    m_startButton = new QPushButton("开始游戏", this);
    m_exitButton = new QPushButton("退出游戏", this);

    m_titleLabel->setAlignment(Qt::AlignCenter); 
    m_startButton->setStyleSheet("color: white;");
    m_exitButton->setStyleSheet("color: white;");

    m_startButton->setMinimumHeight(50);
    m_exitButton->setMinimumHeight(50);

    int finalFontSize = baseFontSize * scaleFactor;
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
    m_startButton->setStyleSheet(buttonStyle);
    m_exitButton->setStyleSheet(buttonStyle);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(); 
    layout->addWidget(m_titleLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(20*scaleFactor); 
    layout->addWidget(m_startButton);
    layout->addWidget(m_exitButton);
    layout->addStretch(); 

    connect(m_startButton, &QPushButton::clicked, this, &StartWidget::startGameClicked);
    connect(m_exitButton, &QPushButton::clicked, this, &StartWidget::exitGameClicked);
}

StartWidget::~StartWidget()
{
    
}
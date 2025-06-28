#include "view/GameWidget.h"
#include "view/SpriteManager.h"
#include <QPainter>
#include <iostream>
#include <QDebug>

#define UI_LEFT 27
#define UI_UP 16

GameWidget::GameWidget(QWidget *parent) : QWidget(parent) {
    // 加载精灵数据
    bool isLoaded = SpriteManager::instance().loadFromFile(":/assert/siprite.json");
    if (!isLoaded) {
        qDebug() << "错误：加载 :/assert/spirite.json 文件失败！";
        return;
    } else {
        qDebug() << "成功加载 :/assert/sprite.json 文件。";
    }
    m_spriteSheet.load(":/assert/sprite.png");
    if (m_spriteSheet.isNull()) {
        qDebug() << "错误：加载 :/assert/sprite.png 文件失败！";
    }

    m_gameMap = new GameMap();
    if (!m_gameMap->loadFromFile(":/assert/gamemap.json", "map_1", "1")) { 
        qWarning() << "GameWidget: 地图未能加载，地图将不会被绘制。";
    }
    // 创建动画
    QList<QString> FrameNames = SpriteManager::instance().getAnimationSequence("orc_walk");
    qDebug() << "请求动画序列，获取到的帧名列表是:" << FrameNames;
    m_playerWalk = new Animation(FrameNames, 8.0, true);
    // 设置游戏循环
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    m_timer->start(16);
    m_maxTime = 60.0; 
    m_currentTime = 60.0; 
    m_elapsedTimer.start();
}

GameWidget::~GameWidget() {
    delete m_playerWalk;
    delete m_gameMap;
}

void GameWidget::gameLoop() {
    double deltaTime = m_elapsedTimer.restart() / 1000.0;
    m_playerWalk->update(deltaTime);
    if (m_currentTime > 0) {
        m_currentTime -= deltaTime;
    } else {
        m_currentTime = 0;
    }
    this->update();
}

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), Qt::darkCyan);
    paintMap(&painter);
    // painter.fillRect(rect(), Qt::darkCyan);

    // 获取当前动画帧的名字，例如 "player_walk_down_frame_2"
    const QString& currentFrameName = m_playerWalk->getCurrentFrameName();
    // qDebug().noquote() << "--- 正在绘制新的一帧 --- 帧名:" << currentFrameName;
    QList<SpritePart> parts = SpriteManager::instance().getCompositeParts(currentFrameName);
    double scale = 5.0; // 放大倍数
    QPointF destinationAnchor((8*16 + UI_LEFT)*scale, (8*16 + UI_UP)*scale);

    if (!parts.isEmpty()) {
        // qDebug() << "  -> 已识别为复合精灵，包含" << parts.size() << "个部件。";
        for (const SpritePart& part : parts) {
            QRect sourceRect = SpriteManager::instance().getSpriteRect(part.frameName);
            // qDebug() << "    - 正在绘制部件:" << part.frameName
            //          << "，偏移:" << part.offset
            //          << "，源矩形:" << sourceRect;
            QPointF finalPos = destinationAnchor + (part.offset * scale);
            QRectF destinationRect(finalPos, sourceRect.size() * scale);
            painter.drawPixmap(destinationRect, m_spriteSheet, sourceRect);
        }
    } else {
        // qDebug() << "  -> 已识别为简单精灵。";
        QRect sourceRect = SpriteManager::instance().getSpriteRect(currentFrameName);
        if (!sourceRect.isNull()) {
            // qDebug() << "    - 正在从源矩形绘制:" << sourceRect;
            QRectF destinationRect(destinationAnchor, sourceRect.size() * scale);
            painter.drawPixmap(destinationRect, m_spriteSheet, sourceRect);
        }
    }
}

void GameWidget::paintMap(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    if (m_gameMap && m_gameMap->getWidth() > 0) {
        int healthCount = 3;
        int moneyCount = 5; // 任意数字
        double map_scale = 5.0;
        double ui_margin = 3.0;
        QRect circleRect = SpriteManager::instance().getSpriteRect("ui_circle");
        QRectF circleRectF(UI_LEFT*map_scale, 0, 
            circleRect.width()*map_scale , circleRect.height()*map_scale);
        painter->drawPixmap(circleRectF, m_spriteSheet, circleRect);

        QRect itemRect = SpriteManager::instance().getSpriteRect("ui_item_ground");
        QRectF itemRectF(ui_margin*map_scale, UI_UP*map_scale, 
            itemRect.width()*map_scale, itemRect.height()*map_scale);
        painter->drawPixmap(itemRectF, m_spriteSheet, itemRect);

        QRect healthRect = SpriteManager::instance().getSpriteRect("ui_helth");
        QRectF healthRectF((ui_margin-2)*map_scale, itemRectF.bottom()+ui_margin*map_scale, 
            healthRect.width()*map_scale, healthRect.height()*map_scale);
        painter->drawPixmap(healthRectF, m_spriteSheet, healthRect);

        QRect moneyRect = SpriteManager::instance().getSpriteRect("ui_money");
        QRectF moneyRectF((ui_margin-2)*map_scale, healthRectF.bottom()+ui_margin*map_scale, 
            moneyRect.width()*map_scale, moneyRect.height()*map_scale);
        painter->drawPixmap(moneyRectF, m_spriteSheet, moneyRect);
    
        if (m_maxTime > 0) { 
            double barWidth = 15*16*map_scale; // 时间条总宽度
            double barHeight = 4*map_scale; // 时间条高度
            QColor barBackgroundColor = Qt::darkGray;
            QColor barFillColor = Qt::green;

            QPointF barTopLeft(
                circleRectF.right() + (ui_margin-2)*map_scale, 
                circleRectF.center().y() - barHeight / 2.0 + 3*map_scale
            );

            // 定义背景框的矩形
            QRectF barBackgroundRect(barTopLeft, QSizeF(barWidth, barHeight));

            double timeRatio = m_currentTime / m_maxTime;
            double fillWidth = barWidth * timeRatio;

            //  定义前景填充条的矩形
            QRectF barFillRect(barTopLeft, QSizeF(fillWidth, barHeight));

            // 深色的背景框
            painter->fillRect(barBackgroundRect, barBackgroundColor);
            // 亮色的填充条
            painter->fillRect(barFillRect, barFillColor);

            // 给时间条画一个黑色的边框
            painter->setPen(Qt::black);
            painter->drawRect(barBackgroundRect);
        }

        QString healthText = QString("x%1").arg(healthCount);
        // 设置文本的字体和颜色
        QFont Hfont = painter->font();
        Hfont.setPointSize(21); // 设置字体大小
        painter->setFont(Hfont);
        painter->setPen(Qt::white); // 设置文字颜色为白色
        QPointF healthtextPos(
            healthRectF.right() + (ui_margin-1) * map_scale, 
            healthRectF.center().y() + Hfont.pointSize() / 2.0 // 大致的垂直居中
        );
        painter->drawText(healthtextPos, healthText);

        QString moneyText = QString("x%1").arg(healthCount);
        QFont Mfont = painter->font();
        Mfont.setPointSize(21); 
        painter->setFont(Mfont);
        painter->setPen(Qt::white); 
        QPointF moneyTextPos(
            moneyRectF.right() + (ui_margin-1) * map_scale, 
            moneyRectF.center().y() + Mfont.pointSize() / 2.0 
        );
        painter->drawText(moneyTextPos, moneyText);

        for (int row = 0; row < m_gameMap->getHeight(); ++row) {
            for (int col = 0; col < m_gameMap->getWidth(); ++col) {
                int tileId = m_gameMap->getTileIdAt(row, col);
                QString spriteName = m_gameMap->getTileSpriteName(tileId);
                QRect sourceRect = SpriteManager::instance().getSpriteRect(spriteName);
                if (sourceRect.isNull()) continue;
                double destX = (col * sourceRect.width() + UI_LEFT)* map_scale;
                double destY = (row * sourceRect.height() + UI_UP)* map_scale;
                QRectF destRect(destX, destY, sourceRect.width() * map_scale, sourceRect.height() * map_scale);
                painter->drawPixmap(destRect, m_spriteSheet, sourceRect);
            }
        }
    } else {
        painter->fillRect(rect(), Qt::darkCyan);
    }
}
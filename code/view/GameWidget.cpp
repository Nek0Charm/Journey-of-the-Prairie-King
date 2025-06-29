#include "view/GameWidget.h"
#include "GameWidget.h"

#define UI_LEFT 27
#define UI_UP 16

GameWidget::GameWidget(QWidget *parent) : QWidget(parent) {
    bool isLoaded = SpriteManager::instance().loadFromFile(":/assert/sprite.json");
    if (!isLoaded) {
        qDebug() << "错误：加载 :/assert/sprite.json 文件失败！";
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
    m_entity.append(new Entity("player_walk_left")); 
    Entity* another_orc = new Entity("orc_walk");
    another_orc->setPosition(QPointF(200, 200));
    m_entity.append(another_orc);
    m_maxTime = 60.0; 
    m_currentTime = 60.0; 
    m_timer = new QTimer(this); // 临时时钟
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    m_timer->start(16);
    m_elapsedTimer.start();
}

GameWidget::~GameWidget() {
    // delete m_animation;
    delete m_gameMap;
}

void GameWidget::gameLoop() {
    double deltaTime = m_elapsedTimer.restart() / 1000.0;
    // m_animation->update(deltaTime);
    if (m_currentTime > 0) {
        m_currentTime -= deltaTime;
    } else {
        m_currentTime = 0;
    }
    for (Entity* enemy : m_entity) {
        enemy->update(deltaTime);
    }
    this->update();
}

// void GameWidget::onStateUpdated() {
//     this->update(); 
// }

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    paintMap(&painter);
    paintUi(&painter);
    for (Entity* enemy : m_entity) {
        enemy->paint(&painter, m_spriteSheet); 
    }
}

void GameWidget::paintMap(QPainter *painter) {
    double map_scale = 5.0;
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (m_gameMap && m_gameMap->getWidth() > 0) {
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
void GameWidget::paintUi(QPainter *painter) {
    int healthCount = 3;
    int moneyCount = 5; 
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
        double barWidth = 15*16*map_scale; 
        double barHeight = 4*map_scale; 
        QColor barBackgroundColor = Qt::darkGray;
        QColor barFillColor = Qt::green;

        QPointF barTopLeft(
            circleRectF.right() + (ui_margin-2)*map_scale, 
            circleRectF.center().y() - barHeight / 2.0 + 3*map_scale
        );
        QRectF barBackgroundRect(barTopLeft, QSizeF(barWidth, barHeight));
        double timeRatio = m_currentTime / m_maxTime;
        double fillWidth = barWidth * timeRatio;
        QRectF barFillRect(barTopLeft, QSizeF(fillWidth, barHeight));
        painter->fillRect(barBackgroundRect, barBackgroundColor);
        painter->fillRect(barFillRect, barFillColor);
        painter->setPen(Qt::black);
        painter->drawRect(barBackgroundRect);
    }

    QString healthText = QString("x%1").arg(healthCount);
    QFont Hfont = painter->font();
    Hfont.setPointSize(21); 
    painter->setFont(Hfont);
    painter->setPen(Qt::white);
    QPointF healthtextPos(
        healthRectF.right() + (ui_margin-1) * map_scale, 
        healthRectF.center().y() + Hfont.pointSize() / 2.0 
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
}
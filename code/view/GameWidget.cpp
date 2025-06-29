#include "view/GameWidget.h"
#include "GameWidget.h"

#define UI_LEFT 27
#define UI_UP 16
#define SCALE 5

GameWidget::GameWidget(GameViewModel *viewModel, QWidget *parent) 
    : QWidget(parent), m_viewModel(viewModel) {
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
    player = new Entity("player_walk_left");
    m_entity.append(new Entity("orc_walk")); 
    Entity* another_orc = new Entity("orc_walk");
    another_orc->setPosition(QPointF(200, 200));
    m_entity.append(another_orc);
    m_maxTime = 60.0; 
    m_currentTime = 60.0; 
    m_timer = new QTimer(this); // 临时时钟
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    connect(m_viewModel, &GameViewModel::playerPositonChanged, this, &GameWidget::playerPositionChanged);
    m_timer->start(16);
    m_elapsedTimer.start();
}

GameWidget::~GameWidget() {
    // delete m_animation;
    delete m_gameMap;
}

void GameWidget::gameLoop() {
    double deltaTime = m_elapsedTimer.restart() / 1000.0;
    if (player) {
        player->update(deltaTime, player->getPosition(), m_viewModel->getPlayer()->getShootingDirection());
    }
    // m_animation->update(deltaTime);
    if (m_currentTime > 0) {
        m_currentTime -= deltaTime;
    } else {
        m_currentTime = 0;
    }
    for (Entity* enemy : m_entity) {
        enemy->update(deltaTime, {150, 200});
    }
    this->update();
}

void GameWidget::playerPositionChanged() {
    player->setPosition(m_viewModel->getPlayerPosition());
    // qDebug() << m_viewModel->getPlayerPosition();
}

// void GameWidget::onStateUpdated() {
//     this->update(); 
// }

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    paintMap(&painter);
    paintUi(&painter);
    player->paint(&painter, m_spriteSheet);
    for (Entity* enemy : m_entity) {
        enemy->paint(&painter, m_spriteSheet); 
    }

    if (m_viewModel && m_viewModel->getPlayer()) {
        const auto& bullets = m_viewModel->getPlayer()->getActiveBullets();
        QRect bulletSourceRect = SpriteManager::instance().getSpriteRect("player_bullet");
        if (!bulletSourceRect.isNull()) {
            for (const auto& bullet : bullets) {
                qDebug() << "bullet";
                QPointF topLeft = (bullet.position - QPointF(bulletSourceRect.width()/2.0, bulletSourceRect.height()/2.0) + QPointF(UI_LEFT, UI_UP) + QPointF(8, 8)) * (SCALE, SCALE);
                qDebug() << bullet.position;
                QSizeF scaledSize(bulletSourceRect.width() * SCALE, bulletSourceRect.height() * SCALE);
                QRectF destRect(topLeft, scaledSize);
                painter.drawPixmap(destRect, m_spriteSheet, bulletSourceRect);
            }
        }
    }

}

void GameWidget::paintMap(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (m_gameMap && m_gameMap->getWidth() > 0) {
        for (int row = 0; row < m_gameMap->getHeight(); ++row) {
            for (int col = 0; col < m_gameMap->getWidth(); ++col) {
                int tileId = m_gameMap->getTileIdAt(row, col);
                QString spriteName = m_gameMap->getTileSpriteName(tileId);
                QRect sourceRect = SpriteManager::instance().getSpriteRect(spriteName);
                if (sourceRect.isNull()) continue;
                double destX = (col * sourceRect.width() + UI_LEFT)* SCALE;
                double destY = (row * sourceRect.height() + UI_UP)* SCALE;
                QRectF destRect(destX, destY, sourceRect.width() * SCALE, sourceRect.height() * SCALE);
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
    double ui_margin = 3.0;
    
    QRect circleRect = SpriteManager::instance().getSpriteRect("ui_circle");
    QRectF circleRectF(UI_LEFT*SCALE, 0, 
        circleRect.width()*SCALE , circleRect.height()*SCALE);
    painter->drawPixmap(circleRectF, m_spriteSheet, circleRect);

    QRect itemRect = SpriteManager::instance().getSpriteRect("ui_item_ground");
    QRectF itemRectF(ui_margin*SCALE, UI_UP*SCALE, 
        itemRect.width()*SCALE, itemRect.height()*SCALE);
    painter->drawPixmap(itemRectF, m_spriteSheet, itemRect);

    QRect healthRect = SpriteManager::instance().getSpriteRect("ui_helth");
    QRectF healthRectF((ui_margin-2)*SCALE, itemRectF.bottom()+ui_margin*SCALE, 
        healthRect.width()*SCALE, healthRect.height()*SCALE);
    painter->drawPixmap(healthRectF, m_spriteSheet, healthRect);

    QRect moneyRect = SpriteManager::instance().getSpriteRect("ui_money");
    QRectF moneyRectF((ui_margin-2)*SCALE, healthRectF.bottom()+ui_margin*SCALE, 
        moneyRect.width()*SCALE, moneyRect.height()*SCALE);
    painter->drawPixmap(moneyRectF, m_spriteSheet, moneyRect);

    if (m_maxTime > 0) { 
        double barWidth = 15*16*SCALE; 
        double barHeight = 4*SCALE; 
        QColor barBackgroundColor = Qt::darkGray;
        QColor barFillColor = Qt::green;

        QPointF barTopLeft(
            circleRectF.right() + (ui_margin-2)*SCALE, 
            circleRectF.center().y() - barHeight / 2.0 + 3*SCALE
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
        healthRectF.right() + (ui_margin-1) * SCALE, 
        healthRectF.center().y() + Hfont.pointSize() / 2.0 
    );
    painter->drawText(healthtextPos, healthText);

    QString moneyText = QString("x%1").arg(healthCount);
    QFont Mfont = painter->font();
    Mfont.setPointSize(21); 
    painter->setFont(Mfont);
    painter->setPen(Qt::white); 
    QPointF moneyTextPos(
        moneyRectF.right() + (ui_margin-1) * SCALE, 
        moneyRectF.center().y() + Mfont.pointSize() / 2.0 
    );
    painter->drawText(moneyTextPos, moneyText);
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
    PlayerViewModel* playerVM = m_viewModel->getPlayer();
    if (!playerVM) return;
    switch (event->key()) {
        case Qt::Key_W:
            m_viewModel->setPlayerMoveDirection({0,-1}, true);
            player->setDirection({0,-1});
            break;
        case Qt::Key_S:
            m_viewModel->setPlayerMoveDirection({0,1}, true);
            player->setDirection({0,1});
            break;
        case Qt::Key_A:
            m_viewModel->setPlayerMoveDirection({-1,0}, true);
            player->setDirection({-1,0});
            break;
        case Qt::Key_D:
            m_viewModel->setPlayerMoveDirection({1,0}, true);
            player->setDirection({1,0});
            break;
        case Qt::Key_Up:
            playerVM->shoot({0, -1});
            break;
        case Qt::Key_Down:
            playerVM->shoot({0, 1}); 
            break;
        case Qt::Key_Left:
            playerVM->shoot({-1, 0}); 
            break;
        case Qt::Key_Right:
            playerVM->shoot({1, 0}); 
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    m_viewModel->setPlayerMoveDirection({0,0}, false);
}

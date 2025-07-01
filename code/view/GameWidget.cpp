#include "view/GameWidget.h"
#include "GameWidget.h"

#define UI_LEFT 27
#define UI_UP 16
#define SCALE 5

GameWidget::GameWidget(GameViewModel *viewModel, QWidget *parent) 
    : QWidget(parent), m_viewModel(viewModel) {
    bool isLoaded = SpriteManager::instance().loadFromFile(":/assert/picture/sprite.json");
    if (!isLoaded) {
        qDebug() << "错误：加载 :/assert/picture/sprite.json 文件失败！";
        return;
    } else {
        qDebug() << "成功加载 :/assert/picture/sprite.json 文件。";
    }
    m_spriteSheet.load(":/assert/picture/sprite.png");
    if (m_spriteSheet.isNull()) {
        qDebug() << "错误：加载 :/assert/picture/sprite.png 文件失败！";
    }
    m_gameMap = new GameMap();
    if (!m_gameMap->loadFromFile(":/assert/picture/gamemap.json", "map_1", "1")) { 
        qWarning() << "GameWidget: 地图未能加载，地图将不会被绘制。";
    }
    player = new PlayerEntity();
    setFocusPolicy(Qt::StrongFocus); 
    m_maxTime = MAX_GAMETIME; 
    m_currentTime = MAX_GAMETIME; 
    m_timer = new QTimer(this); // 临时时钟
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    connect(m_viewModel, &GameViewModel::playerPositonChanged, this, &GameWidget::playerPositionChanged);
    connect(m_viewModel, &GameViewModel::playerLivesChanged, this, &GameWidget::playerLivesChanged);
    connect(m_viewModel->getEnemyManager(), &EnemyManager::enemyDestroyed, this, &GameWidget::die);
    m_timer->start(16);
    m_elapsedTimer.start();
}

GameWidget::~GameWidget() {
    delete player;
    qDeleteAll(m_monsters);
    m_monsters.clear();
    qDeleteAll(m_deadmonsters);
    m_deadmonsters.clear();
    delete m_gameMap;
}

void GameWidget::gameLoop() {
    timerEvent();
    double deltaTime = m_elapsedTimer.restart() / 1000.0;
    syncEnemies(); 
    if (player) {
        player->update(deltaTime);
    }
    m_currentTime =60-m_viewModel->getGameTime();
    for (auto& it: m_monsters) {
        it->update(deltaTime);
    }
    for (auto it = m_deadmonsters.begin(); it != m_deadmonsters.end(); ) {
        DeadMonsterEntity* deadMonster = it.value();
        int monsterId = it.key();

        deadMonster->update(deltaTime);
        if (deadMonster->ShouldbeRemove()) {
            delete deadMonster;
            it = m_deadmonsters.erase(it);

        } else {
            ++it;
        }
    }       
    this->update();
}

void GameWidget::playerPositionChanged() {
    player->setPosition(m_viewModel->getPlayerPosition());
    // qDebug() << m_viewModel->getPlayerPosition();
}

void GameWidget::playerLivesChanged() {
    if (player->isInvincible()) {
        return;
    }
    // qDebug() << "受伤了";
    player->setInvincible(true);
    player->setInvincibilityTime(100.0);
}

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPointF viewOffset(0, 0);
     if (m_gameMap && m_gameMap->getWidth() > 0) {
        double worldContentWidth = (m_gameMap->getWidth() * 16) * SCALE;
        double worldContentHeight = (m_gameMap->getHeight() * 16) * SCALE;
        double offsetX = (this->width() - worldContentWidth) / 2.0;
        double offsetY = (this->height() - worldContentHeight) / 2.0;
        viewOffset.setX(offsetX);
        viewOffset.setY(offsetY);
    }
    painter.fillRect(rect(), Qt::black); 

    paintMap(&painter, viewOffset);
    paintUi(&painter, viewOffset);
    for (auto it: m_deadmonsters) {
        it->paint(&painter, m_spriteSheet, viewOffset);
    }
    
    player->paint(&painter, m_spriteSheet, viewOffset);
    for (MonsterEntity* monster : m_monsters) {
        monster->paint(&painter, m_spriteSheet, viewOffset); 
    }
    if (m_viewModel && m_viewModel->getPlayer()) {
        const auto& bullets = m_viewModel->getPlayer()->getActiveBullets();
        QRect bulletSourceRect = SpriteManager::instance().getSpriteRect("player_bullet");
        if (!bulletSourceRect.isNull()) {
            for (const auto& bullet : bullets) {
                // qDebug() << "bullet";
                QPointF topLeft = (bullet.position - QPointF(bulletSourceRect.width()/2.0, bulletSourceRect.height()/2.0) + QPointF(10, 10)) * SCALE;
                // qDebug() << bullet.position;
                QSizeF scaledSize(bulletSourceRect.width() * SCALE, bulletSourceRect.height() * SCALE);
                QRectF destRect(topLeft, scaledSize);
                destRect.translate(viewOffset);
                painter.drawPixmap(destRect, m_spriteSheet, bulletSourceRect);
            }
        }
    }
}

void GameWidget::paintMap(QPainter *painter, const QPointF& viewOffset) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (m_gameMap && m_gameMap->getWidth() > 0) {
        for (int row = 0; row < m_gameMap->getHeight(); ++row) {
            for (int col = 0; col < m_gameMap->getWidth(); ++col) {
                int tileId = m_gameMap->getTileIdAt(row, col);
                QString spriteName = m_gameMap->getTileSpriteName(tileId);
                QRect sourceRect = SpriteManager::instance().getSpriteRect(spriteName);
                if (sourceRect.isNull()) continue;
                double destX = (col * sourceRect.width())* SCALE;
                double destY = (row * sourceRect.height())* SCALE;
                QRectF destRect(destX, destY, sourceRect.width() * SCALE, sourceRect.height() * SCALE);
                destRect.translate(viewOffset);
                painter->drawPixmap(destRect, m_spriteSheet, sourceRect);
            }
        }
    } else {
        painter->fillRect(rect(), Qt::darkCyan);
    }
}

void GameWidget::paintUi(QPainter *painter, const QPointF& viewOffset) {
    int healthCount = m_viewModel->getPlayerLives() - 1;
    int moneyCount = 5; 
    double ui_margin = 3.0;
    
    QRect circleRect = SpriteManager::instance().getSpriteRect("ui_circle");
    QRectF circleRectF(0, -(circleRect.height()+ui_margin/4)*SCALE, circleRect.width()*SCALE , circleRect.height()*SCALE);
    circleRectF.translate(viewOffset);
    painter->drawPixmap(circleRectF, m_spriteSheet, circleRect);

    QRect itemRect = SpriteManager::instance().getSpriteRect("ui_item_ground");
    QRectF itemRectF(-(itemRect.width() + ui_margin)*SCALE, 0, itemRect.width()*SCALE, itemRect.height()*SCALE);
    QPointF itemBottomLeft = itemRectF.bottomLeft();
    itemRectF.translate(viewOffset);
    painter->drawPixmap(itemRectF, m_spriteSheet, itemRect);

    QRect healthRect = SpriteManager::instance().getSpriteRect("ui_helth");
    QRectF healthRectF(itemBottomLeft.x(), (itemBottomLeft.y() + ui_margin*SCALE), healthRect.width()*SCALE, healthRect.height()*SCALE);
    QPointF healthBottomLeft = healthRectF.bottomLeft();
    healthRectF.translate(viewOffset);
    painter->drawPixmap(healthRectF, m_spriteSheet, healthRect);

    QRect moneyRect = SpriteManager::instance().getSpriteRect("ui_money");
    QRectF moneyRectF(healthBottomLeft.x(), (healthBottomLeft.y() + ui_margin*SCALE), moneyRect.width()*SCALE, moneyRect.height()*SCALE);
    moneyRectF.translate(viewOffset);
    painter->drawPixmap(moneyRectF, m_spriteSheet, moneyRect);

    if (m_maxTime > 0) { 
        double barWidth = 15*16*SCALE; 
        double barHeight = 4*SCALE; 
        QColor barBackgroundColor = Qt::darkGray;
        QColor barFillColor = Qt::green;

        QPointF barTopLeft(
            circleRectF.right() + (ui_margin)*SCALE, 
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
        healthRectF.right() + (ui_margin/2) * SCALE, 
        healthRectF.center().y() + Hfont.pointSize() / 2.0 
    );
    painter->drawText(healthtextPos, healthText);

    QString moneyText = QString("x%1").arg(0);
    QFont Mfont = painter->font();
    Mfont.setPointSize(21); 
    painter->setFont(Mfont);
    painter->setPen(Qt::white); 
    QPointF moneyTextPos(
        moneyRectF.right() + (ui_margin/2) * SCALE, 
        moneyRectF.center().y() + Mfont.pointSize() / 2.0 
    );
    painter->drawText(moneyTextPos, moneyText);
}

void GameWidget::keyPressEvent(QKeyEvent *event) {    
    keys[event->key()] = true;
    QWidget::keyPressEvent(event);
    return;
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {     
    keys[event->key()] = false;
    QWidget::keyReleaseEvent(event);
}

void GameWidget::timerEvent() {
    QPointF moveDirection(0, 0);
    if (keys[Qt::Key_W]) { moveDirection.ry() -= 1; }
    if (keys[Qt::Key_S]) { moveDirection.ry() += 1; }
    if (keys[Qt::Key_A]) { moveDirection.rx() -= 1; }
    if (keys[Qt::Key_D]) { moveDirection.rx() += 1; }

    QPointF shootDirection(0, 0);
    if (keys[Qt::Key_Up])    { shootDirection.ry() -= 1; }
    if (keys[Qt::Key_Down])  { shootDirection.ry() += 1; }
    if (keys[Qt::Key_Left])  { shootDirection.rx() -= 1; }
    if (keys[Qt::Key_Right]) { shootDirection.rx() += 1; }
    PlayerViewModel* playerVM = m_viewModel->getPlayer();
    if (!playerVM) return;

    playerVM->setMovingDirection(moveDirection);
    if (!shootDirection.isNull()) { 
        playerVM->shoot(shootDirection);
    }
    bool isMoving = !moveDirection.isNull();
    bool isShooting = !shootDirection.isNull();
    if (isShooting && isMoving) {
        if (shootDirection.x() < 0)       player->setState(PlayerState::ShootLeftWalk);
        else if (shootDirection.x() > 0)  player->setState(PlayerState::ShootRightWalk);
        else if (shootDirection.y() < 0)  player->setState(PlayerState::ShootUpWalk);
        else if (shootDirection.y() > 0)  player->setState(PlayerState::ShootDownWalk);
    } else if (isShooting) {
        if (shootDirection.x() < 0)       player->setState(PlayerState::ShootLeft);
        else if (shootDirection.x() > 0)  player->setState(PlayerState::ShootRight);
        else if (shootDirection.y() < 0)  player->setState(PlayerState::ShootUp);
        else if (shootDirection.y() > 0)  player->setState(PlayerState::ShootDown);
    } else if (isMoving) {
        if (moveDirection.x() < 0)       player->setState(PlayerState::WalkLeft);
        else if (moveDirection.x() > 0)  player->setState(PlayerState::WalkRight);
        else if (moveDirection.y() < 0)  player->setState(PlayerState::WalkUp);
        else if (moveDirection.y() > 0)  player->setState(PlayerState::WalkDown);
    } else {
        player->setState(PlayerState::Idle);
    }
}

void GameWidget::syncEnemies() {
    if (!m_viewModel || !m_viewModel->getEnemyManager()) return;
    const auto& enemyDataList = m_viewModel->getEnemyManager()->getEnemies();
    QSet<int> liveEnemyIds;
    for (const auto& data : enemyDataList) {
        liveEnemyIds.insert(data.id);
    }
    for (auto it = m_monsters.begin(); it != m_monsters.end();) {
        if (!liveEnemyIds.contains(it.key())) {
            delete it.value();
            it = m_monsters.erase(it);
        } else {
            ++it;
        }
    }
    for (const auto& data : enemyDataList) {
        MonsterEntity* monster = nullptr;
        if (!m_monsters.contains(data.id)) {
            monster = new MonsterEntity("orc");
            m_monsters[data.id] = monster;
        } else {
            monster = m_monsters[data.id];
        }
        monster->setPosition(data.position);
        monster->setVelocity(data.velocity);
    }
}

void GameWidget::die(int id) {
    qDebug() << "ID: " << id << "die";
    if (m_monsters.contains(id)) {
        DeadMonsterEntity* deadm = new DeadMonsterEntity(*m_monsters.value(id));
        m_deadmonsters.insert(id, deadm);
    }
}
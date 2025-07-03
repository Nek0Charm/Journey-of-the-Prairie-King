#include "view/GameWidget.h"
#include "GameWidget.h"

#define UI_LEFT 27
#define UI_UP 16
#define SCALE 3

GameWidget::GameWidget(QWidget *parent) 
    : QWidget(parent){
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
    m_gameMap = new GameMapView("map_1");
    if (!m_gameMap->loadFromFile(":/assert/picture/gamemap.json", "map_1", "1")) { 
        qWarning() << "GameWidget: 地图未能加载，地图将不会被绘制。";
    }
    player = new PlayerEntity();
    setFocusPolicy(Qt::StrongFocus); 
    m_maxTime = MAX_GAMETIME; 
    m_currentTime = MAX_GAMETIME; 
    m_timer = new QTimer(this); // 临时时钟
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    m_timer->start(16);
    m_elapsedTimer.start();
    
    // 初始化道具使用相关
    m_spaceKeyPressed = false;
}

GameWidget::~GameWidget() {
    delete player;
    qDeleteAll(m_monsters);
    m_monsters.clear();
    qDeleteAll(m_deadmonsters);
    m_deadmonsters.clear();
    qDeleteAll(m_items);
    m_items.clear();
    delete m_gameMap;
}

void GameWidget::gameLoop() {
    timerEvent();
    double deltaTime = m_elapsedTimer.restart() / 1000.0;
    syncEnemies(); 
    if (player) {
        player->update(deltaTime);
    }
    for (auto& it: m_monsters) {
        it->update(deltaTime);
    }
    syncItems();
    for (auto item : m_items) {
        item->update(deltaTime);
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
    if(m_gameMap) {
        m_gameMap->update(deltaTime);
    }  
    updateExplosion(deltaTime);
    updateSmoke(deltaTime); 
    this->update();
}

void GameWidget::playerPositionChanged(QPointF position) {
    player->setPosition(position);
    // qDebug() << m_viewModel->getPlayerPosition();
}

void GameWidget::playerLivesDown() {
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

    m_gameMap->paint(&painter, m_spriteSheet, viewOffset);
    paintUi(&painter, viewOffset);
    for (auto it: m_deadmonsters) {
        it->paint(&painter, m_spriteSheet, viewOffset);
    }
    for (auto it: m_items) {
        it->paint(&painter, m_spriteSheet, viewOffset);
    }
    
    player->paint(&painter, m_spriteSheet, viewOffset);
    for (MonsterEntity* monster : m_monsters) {
        monster->paint(&painter, m_spriteSheet, viewOffset); 
    }

    QRect bulletSourceRect = SpriteManager::instance().getSpriteRect("player_bullet");
    if (!bulletSourceRect.isNull()) {
        for (const auto& bullet : m_bullets) {
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

void GameWidget::paintUi(QPainter *painter, const QPointF& viewOffset) {
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
    if (m_hasPossessedItem) {
        QString itemSpriteName = ItemEntity::typeToString(static_cast<ItemType>(m_possessedItemType));
        QRect itemSourceRect = SpriteManager::instance().getSpriteRect(itemSpriteName);
        if (!itemSourceRect.isNull()) {
            double itemScaleRatio = 0.8; 
            double itemScaledWidth = itemRectF.width() * itemScaleRatio;
            double itemScaledHeight = itemRectF.height() * itemScaleRatio;
            QSizeF itemScaledSize(itemScaledWidth, itemScaledHeight);
            QPointF itemTopLeft = itemRectF.center() - QPointF(itemScaledWidth / 2.0, itemScaledHeight / 2.0);
            QRectF itemDestRect(itemTopLeft, itemScaledSize);
            painter->drawPixmap(itemDestRect, m_spriteSheet, itemSourceRect);
        }
    }

    QRect healthRect = SpriteManager::instance().getSpriteRect("ui_helth");
    QRectF healthRectF(itemBottomLeft.x()-ui_margin*SCALE, (itemBottomLeft.y() + ui_margin*SCALE), healthRect.width()*SCALE, healthRect.height()*SCALE);
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
            circleRectF.right() + (ui_margin/2)*SCALE, 
            circleRectF.center().y() - barHeight / 2.0 + 3 * SCALE
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

    QString healthText = QString("x%1").arg(m_healthCount-1);
    QFont Hfont = painter->font();
    Hfont.setPointSize(16); 
    painter->setFont(Hfont);
    painter->setPen(Qt::white);
    QPointF healthtextPos(
        healthRectF.right() + (ui_margin/10) * SCALE, 
        healthRectF.center().y() + Hfont.pointSize() / 2.0 
    );
    painter->drawText(healthtextPos, healthText);

    QString moneyText = QString("x%1").arg(m_moneyCount);
    QFont Mfont = painter->font();
    Mfont.setPointSize(16); 
    painter->setFont(Mfont);
    painter->setPen(Qt::white); 
    QPointF moneyTextPos(
        moneyRectF.right() + (ui_margin/10) * SCALE, 
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

    emit setMovingDirection(moveDirection, !moveDirection.isNull());
    if (!shootDirection.isNull()) { 
        emit shoot(shootDirection);
    }
    // 如果处于僵尸模式，直接设置僵尸动画
    if (m_isZombieMode) {
        player->setState(PlayerState::Zombie);
    } else {
        // 正常状态下的动画逻辑
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
    if (keys[Qt::Key_Space] && !m_spaceKeyPressed) {
        m_spaceKeyPressed = true;
        emit useItem();
    } else if (!keys[Qt::Key_Space]) {
        m_spaceKeyPressed = false;
    }
}

void GameWidget::syncEnemies() {
    QSet<int> liveEnemyIds;
    for (const auto& data : m_enemyDataList) {
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
    for (const auto& data : m_enemyDataList) {
        MonsterEntity* monster = nullptr;
        if (!m_monsters.contains(data.id)) {
            monster = new MonsterEntity(MonsterType::orc);
            m_monsters[data.id] = monster;
        } else {
            monster = m_monsters[data.id];
        }
        monster->setPosition(data.position);
        monster->setVelocity(data.velocity);
        
        // 根据玩家潜行状态设置敌人是否冻结
        monster->setFrozen(m_playerStealthMode);
    }
}

void GameWidget::syncItems() {
    QSet<int> activeItemIds;
    for (const auto& data : m_itemDataList) {
        activeItemIds.insert(data.id);
    }
    for (auto it = m_items.begin(); it != m_items.end();) {
        if (!activeItemIds.contains(it.key()) && it.value()->getState() != ItemState::Picked) {
            delete it.value();
            it = m_items.erase(it);
        } else {
            ++it;
        }
    }
    for (const auto& data: m_itemDataList) {
        ItemEntity* item = nullptr;
        if (!m_items.contains(data.id)) {
            item = new ItemEntity(data.type);
            m_items[data.id] = item;
            // qDebug() << "new_item";
        } else {
            item = m_items[data.id];
        }
        if (item->getState() != ItemState::Picked) {
            item->setPosition(data.position);
        }
    }
    
}

void GameWidget::die(int id) {
    // qDebug() << "ID: " << id << "die";
    if (m_monsters.contains(id)) {
        DeadMonsterEntity* deadm = new DeadMonsterEntity(*m_monsters.value(id));
        m_deadmonsters.insert(id, deadm);
    }
}

void GameWidget::updateGameTime(double gameTime) {
    m_currentTime = 60 - gameTime;
}

void GameWidget::updateBullets(QList<BulletData> bullets) {
    m_bullets = bullets;
}

void GameWidget::updateEnemies(QList<EnemyData> enemies) {
    m_enemyDataList = enemies;
}

void GameWidget::updateItems(QList<ItemData> items) {
    m_itemDataList = items;
}

void GameWidget::updatePlayerStealthMode(bool isStealth) {
    m_playerStealthMode = isStealth;
}

void GameWidget::updatePlayerHealth(int health) {
    if (m_healthCount > health) {
        playerLivesDown();
    }
    m_healthCount = health;
}

void GameWidget::updatePlayerMoney(int money) {
    m_moneyCount = money;
}

void GameWidget::updatePossessedItem(int itemType, bool hasItem) {
    m_possessedItemType = itemType;
    m_hasPossessedItem = hasItem;
}

void GameWidget::updateZombieMode(bool isZombieMode) {
    m_isZombieMode = isZombieMode;
}

void GameWidget::updateStealthMode(bool isStealth) {
    m_playerStealthMode = isStealth;
}

void GameWidget::updateItemEffect(int itemType) {
    switch (itemType) {
    case 5: // Boom
        startExplosionSequence(2.0);
        break;
    case 7: // Stealth
        m_isStealthMode = true;
        player->setInvincible(true);
        player->setInvincibilityTime(3);
        releaseSmoke(0.5);
        break;
    default:
        break;
    }
}

void GameWidget::updateSmoke(double deltaTime) {
    if (!m_isSmokeReleased) {
        return; 
    }
    m_smokeReleaseTimer -= deltaTime;
    if (m_smokeReleaseTimer <= 0) {
        m_isSmokeReleased = false;
        return;
    }
    m_nextSmokeReleaseTimer -= deltaTime;
    if (m_nextSmokeReleaseTimer <= 0) {
        double randX = QRandomGenerator::global()->bounded(16) + player->getPosition().x();
        double randY = QRandomGenerator::global()->bounded(16) + player->getPosition().y();
        m_gameMap->createExplosion(QPointF(randX, randY));
        m_nextSmokeReleaseTimer = (QRandomGenerator::global()->bounded(150) + 150) / 1000.0;
    }
}

void GameWidget::updateExplosion(double deltaTime) {
    if (!m_isExplosionSequenceActive) {
        return; 
    }
    m_explosionSequenceTimer -= deltaTime;
    if (m_explosionSequenceTimer <= 0) {
        m_isExplosionSequenceActive = false; 
        return;
    }
    m_nextExplosionSpawnTimer -= deltaTime;
    if (m_nextExplosionSpawnTimer <= 0) {
        double randX = QRandomGenerator::global()->bounded((m_gameMap->getWidth()-1) * 16);
        double randY = QRandomGenerator::global()->bounded((m_gameMap->getHeight()-1) * 16);
        m_gameMap->createExplosion(QPointF(randX, randY));
        m_nextExplosionSpawnTimer = (QRandomGenerator::global()->bounded(150) + 0) / 1000.0;
    }
}

void GameWidget::startExplosionSequence(double duration) {
    m_isExplosionSequenceActive = true;
    m_explosionSequenceTimer = duration;
    m_nextExplosionSpawnTimer = 0.0;
}

void GameWidget::releaseSmoke(double duration) {
    m_isSmokeReleased = true;
    m_smokeReleaseTimer = duration;
    m_nextSmokeReleaseTimer = 0.0;
}

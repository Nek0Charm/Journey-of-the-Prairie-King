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
    qDebug() << "开始创建GameMapView...";
    m_gameMap = new GameMapView("map_1");
    qDebug() << "GameMapView创建完成，地址:" << m_gameMap;
    if (!m_gameMap->loadFromFile(":/assert/picture/gamemap.json", "map_1", "1")) { 
        qWarning() << "GameWidget: 地图未能加载，地图将不会被绘制。";
    } else {
        qDebug() << "GameWidget: 地图加载成功，尺寸:" << m_gameMap->getWidth() << "x" << m_gameMap->getHeight();
    }
    player = new PlayerEntity();
    vendor = new VendorEntity();
    setFocusPolicy(Qt::StrongFocus); 
    m_maxTime = MAX_GAMETIME; 
    m_currentTime = MAX_GAMETIME; 
    m_timer = new QTimer(this); // 临时时钟
    connect(m_timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    connect(this, &GameWidget::vendorAppear, vendor, &VendorEntity::onVendorAppear);
    connect(this, &GameWidget::vendorDisappear, vendor, &VendorEntity::onVendorDisappear); 
    connect(this, &GameWidget::gameWin, vendor, &VendorEntity::onGameWin);
    connect(this, &GameWidget::gameWin, player, &PlayerEntity::onGameWin);
    m_timer->start(16);
    m_elapsedTimer.start();
    
    // 初始化道具使用相关
    m_spaceKeyPressed = false;
    
    // 初始化供应商物品列表（空列表，等待VendorManager更新）
    m_availableVendorItems = {};
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
    if (m_isTransitioning) {
        m_transitionTimer += deltaTime;
        if (m_transitionTimer >= m_transitionDuration) {
            m_isGamePaused = false;
            emit resumeGame();
            m_isTransitioning = false;
            delete m_gameMap;
            if (m_nextMap) {
                m_gameMap = m_nextMap;
            } else {
                qWarning() << "[gameLoop] m_nextMap为null，未切换地图，保留原地图。";
            }
            m_nextMap = nullptr;
        }
    } else {
        syncEnemies(); 
        if (player) player->update(deltaTime);
        if (vendor)  vendor->update (deltaTime, player->getPosition());
        for (auto& it: m_monsters) it->update(deltaTime);
        syncItems();
        for (auto item : m_items) item->update(deltaTime);
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
        if (m_lightningEffectTimer > 0) {
            m_lightningEffectTimer -= deltaTime;
            if (m_lightningEffectTimer < 0) {
                m_lightningEffectTimer = 0;
                m_lightningSegments.clear();
                m_isGamePaused = false;
                emit resumeGame();
            }
        }
        updateExplosion(deltaTime);
        updateSmoke(deltaTime); 
        if (m_pausedTime > 0) {
            m_pausedTime -= deltaTime;
            if (m_pausedTime <= 0) {
                m_pausedTime = 0;
                if (player->getState() != PlayerState::Lifting){
                    player->setInvincibilityTime(1.5);
                    player->setInvincible(true);
                } else {
                    delete m_purchasedItem;
                    m_purchasedItem = nullptr;
                    player->setState(PlayerState::Idle);
                }
                m_isGamePaused = false;
                emit resumeGame();
            }
        }
    }
    if (m_gameMap) m_gameMap->update(deltaTime);
    if (m_nextMap) m_nextMap->update(deltaTime);
    this->update();
}

void GameWidget::playerPositionChanged(QPointF position) {
    if (!m_isGamePaused) {
        player->setPosition(position);
    }
}

void GameWidget::startMapTransition(const QString &nextMapName, const QString &nextLayoutName) {
    if (m_isTransitioning) return;
    m_isGamePaused = true;
    emit pauseGame();
    m_nextMap = new GameMapView(nextMapName);
    if (!m_nextMap->loadFromFile(":/assert/picture/gamemap.json", nextMapName, nextLayoutName)) {
        qWarning() << "转场失败：无法加载下一关地图" << nextMapName;
        delete m_nextMap;
        m_nextMap = nullptr;
        return;
    }
    m_isTransitioning = true;
    m_transitionDuration = 2.0;
    m_transitionTimer = 0.0;
    double worldContentWidth = (m_gameMap->getWidth() * 16) * SCALE;
    double worldContentHeight = (m_gameMap->getHeight() * 16) * SCALE;
    m_transitionStartOffset.setX((this->width() - worldContentWidth) / 2.0);
    m_transitionStartOffset.setY((this->height() - worldContentHeight) / 2.0);
    m_transitionEndOffset.setX(m_transitionStartOffset.x());
    m_transitionEndOffset.setY(m_transitionStartOffset.y() - worldContentHeight);
}

void GameWidget::onMapChanged() {
    startMapTransition("map_1", "2");
}

void GameWidget::playerLivesDown() {
    if (player->isInvincible()) {
        return;
    }
    // qDebug() << "受伤了";
    player->setState(PlayerState::Dying);
    m_isGamePaused = true;
    emit pauseGame();
    m_pausedTime = 2.0;  
}

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPointF viewOffsetMap(0, 0);
    QPointF viewOffset(0, 0);
    QRectF gameWorldClipRect;
    if (m_gameMap && m_gameMap->getWidth() > 0) {
        double worldContentWidth = (m_gameMap->getWidth() * 16) * SCALE;
        double worldContentHeight = (m_gameMap->getHeight() * 16) * SCALE;
        double offsetX = (this->width() - worldContentWidth) / 2.0;
        double offsetY = (this->height() - worldContentHeight) / 2.0;
        viewOffset.setX(offsetX);
        viewOffset.setY(offsetY);
        viewOffsetMap = viewOffset;
        gameWorldClipRect.setRect(offsetX, offsetY, worldContentWidth, worldContentHeight);
    }
    if (m_isTransitioning) {
        double progress = m_transitionTimer / m_transitionDuration;
        if (progress > 1.0) progress = 1.0;
        double currentOffsetY = m_transitionStartOffset.y() * (1.0 - progress) + m_transitionEndOffset.y() * progress;
        viewOffsetMap.setX(m_transitionStartOffset.x());
        viewOffsetMap.setY(currentOffsetY);
    }
    painter.fillRect(rect(), Qt::black); 
    if (m_lightningEffectTimer > 0) {
        QRect lightningSourceRect_1 = SpriteManager::instance().getSpriteRect("lightning_2");
        QRect lightningSourceRect_2 = SpriteManager::instance().getSpriteRect("lightning_1");
        if (lightningSourceRect_1.isNull()) return;
        for (int i = 0; i < m_lightningSegments.size(); ++i) {
            const QPointF& segmentPos = m_lightningSegments[i];
            QRect currentSourceRect;
            if (i % 2 == 0) {
                currentSourceRect = lightningSourceRect_1;
            } else {
                currentSourceRect = lightningSourceRect_2.isNull() ? lightningSourceRect_1 : lightningSourceRect_2;
            }
            QPointF destinationAnchor = segmentPos * SCALE;
            QRectF destRect(destinationAnchor, currentSourceRect.size() * SCALE);
            destRect.translate(viewOffsetMap);
            painter.drawPixmap(destRect, m_spriteSheet, currentSourceRect);
        }
        player->paint(&painter, m_spriteSheet, viewOffsetMap);
        return;
    }
    painter.setClipRect(gameWorldClipRect);
    if (m_gameMap) m_gameMap->paint(&painter, m_spriteSheet, viewOffsetMap);
    if (m_isTransitioning && m_nextMap) {
        double mapHeight = (m_gameMap->getHeight() * 16) * SCALE;
        QPointF nextMapOffset = viewOffsetMap + QPointF(0, mapHeight);
        m_nextMap->paint(&painter, m_spriteSheet, nextMapOffset);
    }
    for (auto it: m_deadmonsters) it->paint(&painter, m_spriteSheet, viewOffsetMap);
    vendor->paint(&painter, m_spriteSheet, viewOffsetMap);
    for (auto it: m_items) it->paint(&painter, m_spriteSheet, viewOffsetMap);
    player->paint(&painter, m_spriteSheet, viewOffsetMap);
    if (player->getState() == PlayerState::Lifting) {
        if (m_purchasedItem) {
            m_purchasedItem->paint(&painter, m_spriteSheet, viewOffsetMap);
        } else {
            qWarning() << "GameWidget: m_purchasedItem is null during painting.";
        }
    }
    for (MonsterEntity* monster : m_monsters) monster->paint(&painter, m_spriteSheet, viewOffsetMap); 
    QRect bulletSourceRect = SpriteManager::instance().getSpriteRect("player_bullet_1");
    if (!bulletSourceRect.isNull()) {
        for (const auto& bullet : m_bullets) {
            // qDebug() << "bullet";
            QPointF topLeft = (bullet.position - QPointF(bulletSourceRect.width()/2.0, bulletSourceRect.height()/2.0) + QPointF(10, 10)) * SCALE;
            // qDebug() << bullet.position;
            QSizeF scaledSize(bulletSourceRect.width() * SCALE, bulletSourceRect.height() * SCALE);
            QRectF destRect(topLeft, scaledSize);
            destRect.translate(viewOffsetMap);
            painter.drawPixmap(destRect, m_spriteSheet, bulletSourceRect);
        }
    }
    painter.setClipping(false); 
    paintUi(&painter, viewOffset, gameWorldClipRect);
}

void GameWidget::paintUi(QPainter *painter, const QPointF& viewOffset, const QRectF& mapRect) {
    double ui_margin = 3.0;

    QRect itemRect = SpriteManager::instance().getSpriteRect("ui_item_ground");
    QRectF itemRectF(-(itemRect.width() + ui_margin)*SCALE, 0, itemRect.width()*SCALE, itemRect.height()*SCALE);
    QPointF itemBottomLeft = itemRectF.bottomLeft();
    itemRectF.translate(viewOffset);
    painter->drawPixmap(itemRectF, m_spriteSheet, itemRect);
    if (m_hasPossessedItem) {
        QString itemSpriteName = ItemEntity::typeToString(static_cast<int>(m_possessedItemType));
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

    if(!m_isTransitioning) {    
        QRect circleRect = SpriteManager::instance().getSpriteRect("ui_circle");
        QRectF circleRectF(0, -(circleRect.height()+ui_margin/4)*SCALE, circleRect.width()*SCALE , circleRect.height()*SCALE);
        circleRectF.translate(viewOffset);
        painter->drawPixmap(circleRectF, m_spriteSheet, circleRect);
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
    if (!ui_items.isEmpty()) {
        double marginFromMap = 3;
        QPointF anchorPoint = mapRect.bottomLeft();
        int index = 0;
        for (const auto& item : ui_items) {
            if (!item) continue;
            QString itemName = item->typeToString(item->getType());
            QRect itemSourceRect = SpriteManager::instance().getSpriteRect(itemName);
            if (!itemSourceRect.isNull()) {
                QSizeF itemScaledSize(itemSourceRect.width() * SCALE, itemSourceRect.height() * SCALE);
                QPointF itemTopLeft(
                    anchorPoint.x() - marginFromMap - itemScaledSize.width(),
                    anchorPoint.y() - itemScaledSize.height() - (index * (itemScaledSize.height() + marginFromMap))
                );
                QRectF itemDestRect(itemTopLeft, itemScaledSize);
                painter->drawPixmap(itemDestRect, m_spriteSheet, itemSourceRect);
                index++;
            }
        }
    }
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
    if (keys[Qt::Key_P] && !m_pauseKeyPressed) {
        m_pauseKeyPressed = true;
        m_isGamePaused = !m_isGamePaused;
        if (m_isGamePaused) {
            emit pauseGame();
        } else {
            emit resumeGame();
        }
    } else if (!keys[Qt::Key_P]) {
        m_pauseKeyPressed = false;
    }
    if (m_isGamePaused) return;
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
    if (keys[Qt::Key_E]) {
        emit vendorAppear();
    } else if (keys[Qt::Key_Q]) {
        emit vendorDisappear();
    }
    if (keys[Qt::Key_M]) {
        // 手动切换到下一个布局
        emit manualNextGame();
    }
    if (keys[Qt::Key_O]) {
        onGameWin();
    }
    // 供应商物品购买 - 根据实际可购买的物品响应按键
    static bool key1Pressed = false, key2Pressed = false, key3Pressed = false;
    
    if (vendor) {
        // qDebug() << "当前可购买的供应商物品:" << m_availableVendorItems;
        
        if (keys[Qt::Key_1] && !key1Pressed && m_availableVendorItems.size() >= 1) {
            key1Pressed = true;
            emit purchaseVendorItem(m_availableVendorItems[0]);  // 购买第一个可购买物品
        } else if (!keys[Qt::Key_1]) {
            key1Pressed = false;
        }
        
        if (keys[Qt::Key_2] && !key2Pressed && m_availableVendorItems.size() >= 2) {
            key2Pressed = true;
            emit purchaseVendorItem(m_availableVendorItems[1]);  // 购买第二个可购买物品
        } else if (!keys[Qt::Key_2]) {
            key2Pressed = false;
        }
        
        if (keys[Qt::Key_3] && !key3Pressed && m_availableVendorItems.size() >= 3) {
            key3Pressed = true;
            emit purchaseVendorItem(m_availableVendorItems[2]);  // 购买第三个可购买物品
        } else if (!keys[Qt::Key_3]) {
            key3Pressed = false;
        }
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
            monster = new MonsterEntity(enemyTypeToMonsterType(data.enemyType));
            m_monsters[data.id] = monster;
        } else {
            monster = m_monsters[data.id];
        }
        monster->setPosition(data.position);
        monster->setVelocity(data.velocity);
        
        if (data.enemyType == 1) { // Spikeball
            if (data.isDeployed) {
                monster->deploy();
            }
        }
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
            item->setLingerTimer(data.remainTime);
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
        startExplosionSequence(1.0);
        break;
    case 7: // Stealth
        m_isStealthMode = true;
        player->setInvincible(true);
        player->setInvincibilityTime(3);
        releaseSmoke(0.5);
        break;
    case 8: // Lightning
        triggerLightning(player->getPosition());
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

void GameWidget::purchase(int itemType) {
    emit pauseGame();
    m_pausedTime = 0.5;  
    m_isGamePaused = true;
    m_purchasedItem = new ItemEntity(itemType);
    if (itemType == 12 || itemType == 14 || itemType == 15 || itemType == 17 || itemType == 18) {
        ui_items.remove(itemType-1);
    }
    if (itemType != 13 && itemType != 19) {
        ui_items[itemType] = new ItemEntity(itemType);
    }
    m_purchasedItem->setPosition(player->getPosition() - QPointF(0, 15));
    player->setState(PlayerState::Lifting);
}

void GameWidget::onVendorAppear() {
    emit vendorAppear();
}

void GameWidget::onVendorDisappear() {
    emit vendorDisappear();
}

// 新增的供应商相关方法
void GameWidget::onVendorAppeared() {
    // 供应商出现时的处理逻辑
    
    // 触发供应商的显示动画
    if (vendor) {
        vendor->onVendorAppear();
        // 使用当前已设置的供应商物品列表
        vendor->setAvailableItems(m_availableVendorItems);
    }
}

void GameWidget::onVendorDisappeared() {
    // 供应商消失时的处理逻辑
    
    // 清空供应商物品列表
    m_availableVendorItems.clear();
    
    // 隐藏供应商实体
    if (vendor) {
        vendor->onVendorDisappear();  // 设置供应商状态为Disappearing
        vendor->setAvailableItems(QList<int>());
    }
}

void GameWidget::onVendorItemPurchased(int itemType) {
    purchase(itemType);
}

void GameWidget::onEnemyHitByBullet(int enemyId) {
    qDebug() << "敌人ID:" << enemyId << "被子弹击中";
    if (m_monsters.contains(enemyId)) {
        MonsterEntity* monster = m_monsters[enemyId];
        if (monster) {
            monster->onHit();
            qDebug() << "敌人状态已更新为Hit";
        } else {
            qDebug() << "警告：未找到对应的MonsterEntity";
        }
    } else {
        qDebug() << "警告：敌人ID" << enemyId << "不在当前敌人列表中";
    }
}

void GameWidget::updateVendorItems() {
    // 更新供应商的可购买物品列表
    if (vendor) {
        qDebug() << "正在更新VendorEntity的物品列表，当前列表:" << m_availableVendorItems;
        vendor->setAvailableItems(m_availableVendorItems);
        qDebug() << "VendorEntity物品列表更新完成，当前列表:" << vendor->getAvailableItems();
    } else {
        qDebug() << "警告：vendor对象为空，无法更新物品列表";
    }
}

void GameWidget::setAvailableVendorItems(const QList<int>& items) {
    m_availableVendorItems = items;
    // qDebug() << "设置可购买的供应商物品:" << m_availableVendorItems;
    updateVendorItems();  // 立即更新供应商显示
}

void GameWidget::onGameWin() {
    // 清除所有游戏元素，确保游戏胜利画面干净
    qDebug() << "游戏胜利，清除所有游戏元素";
    
    // 清除所有敌人
    qDeleteAll(m_monsters);
    m_monsters.clear();
    qDeleteAll(m_deadmonsters);
    m_deadmonsters.clear();
    
    // 清除所有道具
    qDeleteAll(m_items);
    m_items.clear();
    
    // 清除所有子弹数据
    m_bullets.clear();
    
    // 清除所有敌人数据
    m_enemyDataList.clear();
    
    // 清除所有道具数据
    m_itemDataList.clear();
    
    // 加载游戏胜利地图
    m_gameMap->loadFromFile(":/assert/picture/gamemap.json", "end", "1");
    m_gameMap->setMapTitle("end");
    m_isGamePaused = true;
    emit gameWin(); 
}

// 辅助函数：将EnemyData的enemyType转换为MonsterType
MonsterType GameWidget::enemyTypeToMonsterType(int enemyType) {
    switch (enemyType) {
        case 0: // 普通兽人
            return MonsterType::orc;
        case 1: // Spikeball
            return MonsterType::spikeball;
        case 2: // Ogre
            return MonsterType::ogre;
        default:
            return MonsterType::orc; // 默认返回兽人
    }
}

void GameWidget::triggerLightning(const QPointF &startPosition) {
    emit pauseGame();
    m_isGamePaused = true;
    player->setState(PlayerState::Lightning);
    m_lightningSegments.clear();
    m_lightningEffectTimer = 1.0;
    QRect segmentSourceRect = SpriteManager::instance().getSpriteRect("lightning_1");
    if (segmentSourceRect.isNull()) return;
    int segmentHeight = segmentSourceRect.height();
    for (int i = 1; i < 20; ++i) {
        QPointF segmentPos = startPosition - QPointF(0, i * segmentHeight);
        m_lightningSegments.append(segmentPos);
    }
}



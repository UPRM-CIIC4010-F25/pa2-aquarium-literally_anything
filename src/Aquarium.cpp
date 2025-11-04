#include "Aquarium.h"
#include <cstdlib>
#include <cmath>

void Creature::setDirection(float dx, float dy) { m_dx = dx; m_dy = dy; }
void Creature::setX(float x) { m_x = x; }
void Creature::setY(float y) { m_y = y; }


std::string AquariumCreatureTypeToString(AquariumCreatureType t) {
    switch (t) {
        case AquariumCreatureType::BiggerFish:  return "BiggerFish";
        case AquariumCreatureType::NPCreature:  return "NPCreature";
        case AquariumCreatureType::FastFish:    return "FastFish";
        case AquariumCreatureType::ArmoredFish: return "ArmoredFish";
        default:                                return "UnknownFish";
    }
}


PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
    : Creature(x, y, speed, 10.0f, 1, sprite) 
{
    m_flashSprite = std::make_shared<GameSprite>("white-fish.png", 70, 70);
}

void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
    m_flipped = (m_dx < 0);
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) --m_damage_debounce;
}

void PlayerCreature::update() {
    reduceDamageDebounce();
    move();
    if (m_flashFrames > 0) --m_flashFrames;
}

void PlayerCreature::draw() const {
    if (!m_sprite) return;

    m_sprite->setFlipped(m_flipped);

    if (m_flashFrames > 0 && m_flashSprite) {
        m_flashSprite->setFlipped(m_flipped);
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        m_flashSprite->draw(m_x, m_y);
        ofDisableBlendMode();
        ofPopStyle();
    } else {
        m_sprite->draw(m_x, m_y);
    }
}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) --m_lives;
        m_damage_debounce = debounce;
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives;
        startFlash();
    }
}

void PlayerCreature::increasePower(int value) {
    m_power += value;
    m_collisionRadius += 3.0f;
    m_speed += 1.0f;
}

void PlayerCreature::startFlash() {
    m_flashFrames = 30;
}


NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
    : Creature(x, y, speed, 30.0f, 1, sprite)
{
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();
    m_creatureType = AquariumCreatureType::NPCreature;
    m_value = 1;
}

void NPCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if (m_sprite) m_sprite->setFlipped(m_dx < 0);
    bounce();
}

void NPCreature::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}

// ---- BiggerFish
BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
    : NPCreature(x, y, speed, sprite)
{
    m_value = 5;
    m_creatureType = AquariumCreatureType::BiggerFish;
    setCollisionRadius(60.0f);
}

void BiggerFish::move() {
    m_x += m_dx * (m_speed * 0.5f);
    m_y += m_dy * (m_speed * 0.5f);
    if (m_sprite) m_sprite->setFlipped(m_dx < 0);
    bounce();
}

void BiggerFish::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}

// ---- FastFish
FastFish::FastFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
    : NPCreature(x, y, speed, sprite)
{
    m_creatureType = AquariumCreatureType::FastFish;
    m_speed = std::max(1, speed * 2);
}

void FastFish::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if (m_sprite) m_sprite->setFlipped(m_dx < 0);
    bounce();
}

void FastFish::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}

// ---- ArmoredFish
ArmoredFish::ArmoredFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
    : NPCreature(x, y, speed, sprite)
{
    m_creatureType = AquariumCreatureType::ArmoredFish;
    m_speed = std::max(1, int(speed * 0.5f));
    setCollisionRadius(getCollisionRadius() + 10.0f);
}

void ArmoredFish::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if (m_sprite) m_sprite->setFlipped(m_dx < 0);
    bounce();
}

void ArmoredFish::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}


AquariumSpriteManager::AquariumSpriteManager() {
    m_npc_fish     = std::make_shared<GameSprite>("base-fish.png",    70, 70);
    m_big_fish     = std::make_shared<GameSprite>("bigger-fish.png", 120,120);
    m_fast_fish    = std::make_shared<GameSprite>("fast-fish.png",    70, 70);
    m_armored_fish = std::make_shared<GameSprite>("armored-fish.png", 90, 90);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t) {
    switch (t) {
        case AquariumCreatureType::NPCreature:  return std::make_shared<GameSprite>(*m_npc_fish);
        case AquariumCreatureType::BiggerFish:  return std::make_shared<GameSprite>(*m_big_fish);
        case AquariumCreatureType::FastFish:    return std::make_shared<GameSprite>(*m_fast_fish);
        case AquariumCreatureType::ArmoredFish: return std::make_shared<GameSprite>(*m_armored_fish);
        default:                                return nullptr;
    }
}


void PowerUp::move() {
    m_y += 1.0f;
    if (m_y > ofGetHeight()) m_y = 0;
}

void PowerUp::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}


void AquariumLevel::populationReset() {
    for (auto &node : m_levelPopulation) {
        node->currentPopulation = 0;
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power) {
    for (auto &node : m_levelPopulation) {
        if (node->creatureType == creatureType) {
            if (node->currentPopulation > 0) {
                node->currentPopulation -= 1;
                m_level_score += power;
            }
            return;
        }
    }
}

bool AquariumLevel::isCompleted() {
    return m_level_score >= m_targetScore;
}

// Level repopulate helpers
static std::vector<AquariumCreatureType> RepopulateFromNodes(std::vector<std::shared_ptr<AquariumLevelPopulationNode>> &nodes) {
    std::vector<AquariumCreatureType> out;
    for (auto &node : nodes) {
        int need = node->population - node->currentPopulation;
        if (need > 0) {
            out.insert(out.end(), need, node->creatureType);
            node->currentPopulation += need;
        }
    }
    return out;
}

// ---- Level_0..Level_4
std::vector<AquariumCreatureType> Level_0::Repopulate() { return RepopulateFromNodes(m_levelPopulation); }
std::vector<AquariumCreatureType> Level_1::Repopulate() { return RepopulateFromNodes(m_levelPopulation); }
std::vector<AquariumCreatureType> Level_2::Repopulate() { return RepopulateFromNodes(m_levelPopulation); }
std::vector<AquariumCreatureType> Level_3::Repopulate() { return RepopulateFromNodes(m_levelPopulation); }
std::vector<AquariumCreatureType> Level_4::Repopulate() { return RepopulateFromNodes(m_levelPopulation); }


Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height), m_sprite_manager(std::move(spriteManager)) {}

void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(std::move(creature));
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level) {
    if (level) m_aquariumlevels.push_back(std::move(level));
}

void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        if (!m_aquariumlevels.empty()) {
            int idx = currentLevel % static_cast<int>(m_aquariumlevels.size());
            if (auto npc = std::dynamic_pointer_cast<NPCreature>(*it)) {
                m_aquariumlevels[idx]->ConsumePopulation(npc->GetType(), npc->getValue());
            }
        }
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

void Aquarium::update() {
    // move creatures and powerups
    for (auto &c : m_creatures) c->move();
    for (auto &pu : m_powerUps) pu->move();

    // handle repopulation & level progression
    Repopulate();

    // simple collision resolution between NPCs
    for (size_t i = 0; i < m_creatures.size(); ++i) {
        for (size_t j = i + 1; j < m_creatures.size(); ++j) {
            auto a = m_creatures[i];
            auto b = m_creatures[j];
            if (checkCollision(a, b)) {
                a->setDirection(-a->getDx(), -a->getDy());
                b->setDirection(-b->getDx(), -b->getDy());

                float dx = a->getX() - b->getX();
                float dy = a->getY() - b->getY();
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0.0f) {
                    float overlap = (a->getCollisionRadius() + b->getCollisionRadius() - dist) / 2.0f;
                    a->setX(a->getX() + dx / dist * overlap);
                    a->setY(a->getY() + dy / dist * overlap);
                    b->setX(b->getX() - dx / dist * overlap);
                    b->setY(b->getY() - dy / dist * overlap);
                }
            }
        }
    }
}

void Aquarium::draw() const {
    for (const auto &c : m_creatures) c->draw();
    for (const auto &pu : m_powerUps) pu->draw();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int i) {
    if (i < 0 || static_cast<size_t>(i) >= m_creatures.size()) return nullptr;
    return m_creatures[i];
}

void Aquarium::Repopulate() {
    if (m_aquariumlevels.empty()) return;

    int idx = currentLevel % static_cast<int>(m_aquariumlevels.size());
    auto level = m_aquariumlevels[idx];

    if (level->isCompleted()) {
        level->levelReset();
        ++currentLevel;
        idx = currentLevel % static_cast<int>(m_aquariumlevels.size());
        level = m_aquariumlevels[idx];
        clearCreatures();
    }

    auto toSpawn = level->Repopulate();
    for (auto t : toSpawn) {
        SpawnCreature(t);
    }
}

void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % std::max(1, getWidth());
    int y = rand() % std::max(1, getHeight());
    int speed = 1 + rand() % 25;

    switch (type) {
        case AquariumCreatureType::NPCreature:
            addCreature(std::make_shared<NPCreature>(x, y, speed, m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            addCreature(std::make_shared<BiggerFish>(x, y, speed, m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::FastFish:
            addCreature(std::make_shared<FastFish>(x, y, speed, m_sprite_manager->GetSprite(AquariumCreatureType::FastFish)));
            break;
        case AquariumCreatureType::ArmoredFish:
            addCreature(std::make_shared<ArmoredFish>(x, y, speed, m_sprite_manager->GetSprite(AquariumCreatureType::ArmoredFish)));
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }
}

void Aquarium::SpawnPowerUp(PowerUp::Type type) {
    int x = rand() % std::max(1, getWidth());
    int y = rand() % std::max(1, getHeight() / 2);

    std::shared_ptr<GameSprite> sprite;
    switch (type) {
        case PowerUp::Type::SPEED: sprite = std::make_shared<GameSprite>("speed_powerup.png", 40, 40); break;
        case PowerUp::Type::POWER: sprite = std::make_shared<GameSprite>("power_powerup.png", 40, 40); break;
        case PowerUp::Type::SIZE:  sprite = std::make_shared<GameSprite>("size_powerup.png",  40, 40); break;
    }
    m_powerUps.push_back(std::make_shared<PowerUp>(x, y, type, sprite));
}

void Aquarium::removePowerUp(std::shared_ptr<PowerUp> powerUp) {
    auto it = std::find(m_powerUps.begin(), m_powerUps.end(), powerUp);
    if (it != m_powerUps.end()) m_powerUps.erase(it);
}


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium,
                                                    std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;

    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        auto npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
}

std::shared_ptr<PowerUp> DetectPowerUpCollision(std::shared_ptr<Aquarium> aquarium,
                                                std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;

    const auto &pus = aquarium->GetPowerUps();
    for (const auto &pu : pus) {
        if (checkCollision(std::static_pointer_cast<Creature>(player), pu)) {
            return pu;
        }
    }
    return nullptr;
}


AquariumGameScene::AquariumGameScene(std::shared_ptr<PlayerCreature> player,
                                     std::shared_ptr<Aquarium> aquarium,
                                     std::string name)
    : m_player(std::move(player)), m_aquarium(std::move(aquarium)), m_name(std::move(name))
{
    if (!m_ambientSound.load("sounds/underwater_loop.mp3")) {
        ofLogError() << "Failed to load ambient sound: sounds/underwater_loop.mp3";
    } else {
        m_ambientSound.setLoop(true);
        m_ambientSound.setVolume(0.5f);
        m_ambientSound.play();
    }
}

void AquariumGameScene::Update() {
    m_player->update();

    if (updateControl.tick()) {
        // Player vs NPC collision
        auto event = DetectAquariumCollisions(m_aquarium, m_player);
        if (event && event->isCollisionEvent()) {
            if (event->creatureB) {
                if (m_player->getPower() < event->creatureB->getValue()) {
                    m_player->loseLife(3 * 60);
                    if (m_player->getLives() <= 0) {
                        m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, m_player, nullptr);
                        return;
                    }
                } else {
                    m_aquarium->removeCreature(event->creatureB);
                    m_player->addToScore(1, event->creatureB->getValue());
                    if (m_player->getScore() % 25 == 0) m_player->increasePower(1);
                }
            }
        }

        // Player vs PowerUp
        auto powerUp = DetectPowerUpCollision(m_aquarium, m_player);
        if (powerUp) {
            m_player->increasePower(1);
            m_player->startFlash();
            m_aquarium->removePowerUp(powerUp);
        }

        // Update world
        m_aquarium->update();
    }
}

void AquariumGameScene::Draw() {
    m_player->draw();
    m_aquarium->draw();
    paintAquariumHUD();
}

void AquariumGameScene::paintAquariumHUD() {
    float panelX = ofGetWindowWidth() - 150.0f;
    ofDrawBitmapString("Score: " + std::to_string(m_player->getScore()), panelX, 20);
    ofDrawBitmapString("Power: " + std::to_string(m_player->getPower()), panelX, 30);
    ofDrawBitmapString("Lives: " + std::to_string(m_player->getLives()), panelX, 40);

    for (int i = 0; i < m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelX + i * 20.0f, 50.0f, 5.0f);
    }
    ofSetColor(ofColor::white);
    ofDrawBitmapStringHighlight("Power: " + std::to_string(m_player->getPower()), 20, 60,
                                ofColor(0,0,0,120), ofColor::yellow);
}

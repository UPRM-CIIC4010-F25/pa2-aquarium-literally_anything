#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <string>
#include "Core.h"

// ---------------- ENUM ----------------
enum class AquariumCreatureType {
    NPCreature,
    BiggerFish,
    FastFish,
    ArmoredFish
};

std::string AquariumCreatureTypeToString(AquariumCreatureType t);

// ---------------- LEVEL POPULATION NODE ----------------
class AquariumLevelPopulationNode {
public:
    AquariumLevelPopulationNode() = default;
    AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population)
        : creatureType(creature_type), population(population), currentPopulation(0) {}

    AquariumCreatureType creatureType;
    int population;
    int currentPopulation;
};

// Forward declarations
class AquariumLevel;
class PowerUp;
class AquariumSpriteManager;

// ---------------- AQUARIUM LEVEL (ABSTRACT) ----------------
class AquariumLevel : public GameLevel {
public:
    AquariumLevel(int levelNumber, int targetScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore) {}

    void ConsumePopulation(AquariumCreatureType creature, int power);
    bool isCompleted() override;
    void populationReset();
    void levelReset() { m_level_score = 0; this->populationReset(); }
    virtual std::vector<AquariumCreatureType> Repopulate();


protected:
    std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
    int m_level_score;
    int m_targetScore;
};

// ---------------- PLAYER CREATURE ----------------
class PlayerCreature : public Creature {
public:
    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);

    // Direction helpers
    bool isXDirectionActive() const { return m_dx != 0; }
    bool isYDirectionActive() const { return m_dy != 0; }
    void setFlipped(bool f) { m_flipped = f; }

    void move() override;
    void draw() const override;
    void update();
    void setDirection(float dx, float dy);
    void changeSpeed(int speed);

    // Getters
    int getScore() const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    float getDx() const { return m_dx; }
    float getDy() const { return m_dy; }

    // Gameplay
    void addToScore(int amount, int weight = 1) { m_score += amount * weight; }
    void setLives(int lives) { m_lives = lives; }
    void loseLife(int debounce);
    void increasePower(int value);
    void reduceDamageDebounce();
    void startFlash();

private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1;
    int m_damage_debounce = 0;
    int m_flashFrames = 0;

    bool m_flipped = false;
    std::shared_ptr<GameSprite> m_flashSprite;
};

// ---------------- NPC CREATURE BASE ----------------
class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() { return m_creatureType; }
    int getValue() const { return m_value; }
    void move() override;
    void draw() const override;

protected:
    AquariumCreatureType m_creatureType = AquariumCreatureType::NPCreature;
    int m_value = 1;
};

// ---------------- BIGGER FISH ----------------
class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

// ---------------- FAST FISH ----------------
class FastFish : public NPCreature {
public:
    FastFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

// ---------------- ARMORED FISH ----------------
class ArmoredFish : public NPCreature {
public:
    ArmoredFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

// ---------------- SPRITE MANAGER ----------------
class AquariumSpriteManager {
public:
    AquariumSpriteManager();
    ~AquariumSpriteManager() = default;

    std::shared_ptr<GameSprite> GetSprite(AquariumCreatureType t);

private:
    std::shared_ptr<GameSprite> m_npc_fish;
    std::shared_ptr<GameSprite> m_big_fish;
    std::shared_ptr<GameSprite> m_fast_fish;
    std::shared_ptr<GameSprite> m_armored_fish;
};

// ---------------- POWERUP ----------------
class PowerUp : public Creature {
public:
    enum class Type { SPEED, POWER, SIZE };

    PowerUp(float x, float y, Type type, std::shared_ptr<GameSprite> sprite)
        : Creature(x, y, 0, 20.0f, 0, sprite), m_type(type) {}

    void move() override;
    void draw() const override;
    Type getType() const { return m_type; }

private:
    Type m_type;
};

// ---------------- AQUARIUM ----------------
class Aquarium {
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update();
    void draw() const;

    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }

    void Repopulate();
    void SpawnCreature(AquariumCreatureType type);
    void SpawnPowerUp(PowerUp::Type type);
    void removePowerUp(std::shared_ptr<PowerUp> powerUp);

    std::shared_ptr<Creature> getCreatureAt(int i);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    const std::vector<std::shared_ptr<PowerUp>>& GetPowerUps() const { return m_powerUps; }

private:
    int m_maxPopulation = 0;
    int m_width, m_height;
    int currentLevel = 0;

    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::vector<std::shared_ptr<PowerUp>> m_powerUps;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;
};

// ---------------- COLLISION FUNCTIONS ----------------
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);
std::shared_ptr<PowerUp> DetectPowerUpCollision(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);

// ---------------- GAME SCENE ----------------
class AquariumGameScene : public GameScene {
public:
    AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, std::string name);

    std::shared_ptr<GameEvent> GetLastEvent() { return m_lastEvent; }
    std::shared_ptr<PlayerCreature> GetPlayer() { return m_player; }
    std::shared_ptr<Aquarium> GetAquarium() { return m_aquarium; }
    std::string GetName() override { return m_name; }

    void Update() override;
    void Draw() override;

private:
    void paintAquariumHUD();

    std::shared_ptr<PlayerCreature> m_player;
    std::shared_ptr<Aquarium> m_aquarium;
    std::shared_ptr<GameEvent> m_lastEvent;

    std::string m_name;
    AwaitFrames updateControl{5};
    ofSoundPlayer m_ambientSound;
};

// ---------------- LEVELS ----------------
class Level_0 : public AquariumLevel {
public:
    Level_0(int n, int target) : AquariumLevel(n, target) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::NPCreature, 10));
    }
    std::vector<AquariumCreatureType> Repopulate() override;
};

class Level_1 : public AquariumLevel {
public:
    Level_1(int n, int target) : AquariumLevel(n, target) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::NPCreature, 20));
    }
    std::vector<AquariumCreatureType> Repopulate() override;
};

class Level_2 : public AquariumLevel {
public:
    Level_2(int n, int target) : AquariumLevel(n, target) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::NPCreature, 30));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::BiggerFish, 5));
    }
    std::vector<AquariumCreatureType> Repopulate() override;
};

class Level_3 : public AquariumLevel {
public:
    Level_3(int n, int target) : AquariumLevel(n, target) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::NPCreature, 25));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::BiggerFish, 10));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::FastFish, 5));
    }
    std::vector<AquariumCreatureType> Repopulate() override;
};

class Level_4 : public AquariumLevel {
public:
    Level_4(int n, int target) : AquariumLevel(n, target) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::NPCreature, 30));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::BiggerFish, 15));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::FastFish, 10));
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(
            AquariumCreatureType::ArmoredFish, 5));
    }
    std::vector<AquariumCreatureType> Repopulate() override;
};

#include <SFML/Graphics.hpp>
#include "EntityManager.hpp"
#include "Components.hpp"
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand() with time

// Enum representing the current game state
enum class GameState {
    Playing,
    GameOver
};

// Main Game Class
class Game {
public:
    Game();                // Constructor
    ~Game();               // Destructor

    void run();            // Main game loop

private:
    // === Game State ===
    GameState gameState = GameState::Playing; // Tracks the current state of the game

    // === Input Handling ===
    void handleInput();                        // Handles player input
    void fireBullet(bool isSupermove = false); // Fires a bullet (optional supermove mode)
    void activateSupermove();                  // Fires bullets in all directions

    // === Update Logic ===
    void update(float dt);                    // Updates the game state
    void updateSurvivalPoints(float dt);      // Tracks survival time for scoring
    void updatePlayerInvincibility(float dt); // Updates invincibility frames for the player
    void updateBulletCooldown(float dt);      // Manages bullet firing cooldown
    void updateSupermoveCooldown(float dt);   // Manages supermove cooldown
    void updateBullets(float dt);             // Updates active bullets
    void updateFragments(float dt);           // Updates fragment entities
    void updateCollisions();                  // Handles all collisions in the game

    void updatePlayer(float dt);              // Updates player-specific logic

    // === Rendering ===
    void render();                 // Draws everything to the screen
    void initializeHUD();          // Sets up HUD elements
    void updateHUD();              // Updates HUD values
    void initializeGameOverText(); // Prepares the "Game Over" text

    // === Spawning ===
    void spawnEnemies(float dt);   // Spawns enemy entities

    // === Explosions ===
    void explodeEnemy(std::shared_ptr<Entity> enemy); // Handles enemy destruction visuals

    // === Game Logic ===
    void processPlayerMovement(float dt); // Handles player movement logic
    void processEnemyMovement(float dt);  // Handles enemy movement logic

    // === Core Components ===
    sf::RenderWindow window;        // Main game window
    EntityManager entityManager;    // Manages all entities in the game

    // === Player Attributes ===
    float playerSpeed = 200.0f;         // Movement speed
    float playerRadius = 45.0f;         // Radius for player collision
    float playerRotationSpeed = 360.0f; // Player rotation speed (degrees per second)
    int playerLives = 3;                // Initial number of lives
    float playerInvincibilityTime = 2.0f;     // Player is invincible after spawing to avoid death on spawn

    // === Enemy Attributes ===
    float enemySpawnTimer = 0.0f;       // Tracks time for spawning enemies
    float enemySpawnInterval = 0.7f;    // Interval between enemy spawns
    float enemySpeed = 130.0f;          // Enemy movement speed
    float enemyRadius = 35.0f;          // Enemy collision radius
    float enemyRotationSpeed = 360.0f;  // Enemy rotation speed (degrees per second)
    size_t maxEnemyPerFrame = 15;       // Max enemies to spawn per frame
    float spawnProtectionTime = 1.0f; // Duration for spawn protection

    // === Bullet Attributes ===
    float superBulletSpeed = 500.0f;    // Fixed super bullet speed
    float bulletSpeed = 2.3f;           // Fixed bullet speed
    float bulletCooldown = 0.1f;       // Time between bullet shots
    float bulletCooldownTimer = 0.0f;  // Tracks cooldown time
    float bulletLifeTime = 1.0f;       // Lifespan of bullets (seconds)

    // === Fragment Attributes ===
    float fragmentSpeed = 200.0f;         // Speed of fragments after explosions
    float fragmentLifeTime = 1.0f;      // Lifespan of fragments

    // === Supermove Attributes ===
    float supermoveCooldown = 4.0f;     // Time between supermove uses
    float supermoveTimer = 0.0f;        // Tracks supermove cooldown
    bool supermoveReady = true;         // Indicates if supermove is ready

    // === HUD Elements ===
    sf::Font font;                      // Font used for all HUD text
    sf::Text livesText;                 // Displays the player's remaining lives
    sf::Text bestScoreText;             // Displays the best score for the player's current shape
    sf::Text pointsText;                // Displays the player's current score
    sf::Text supermoveDisplay;          // Displays the supermove status (READY or cooldown time)
    sf::Text gameOverText;              // Displays the "Game Over" message when the game ends

    // === Score and Survival ===
    int totalPoints = 0;                // Tracks the player's current total score
    float survivalTimer = 0.0f;         // Accumulates survival time for awarding points

    std::map<std::string, int> bestScores; // Stores the best scores for each shape (e.g., "triangle" -> 3000)

    // Handles the player's death, updating the best score if applicable
    void handlePlayerDeath(int currentScore, int shapeSides);
    };

// === Utility Function ===
// Returns a random number in the range [min, max]
template <typename T>
T getRandom(T min, T max) {
    static_assert(std::is_arithmetic<T>::value, "Template type must be numeric");

    if constexpr (std::is_integral<T>::value) {
        // For integers
        return min + (rand() % (max - min + 1));
    } else {
        // For floating-point numbers
        return min + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX) / (max - min));
    }
}

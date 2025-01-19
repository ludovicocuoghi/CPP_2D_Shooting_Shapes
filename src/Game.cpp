#include "ScoreManager.hpp"
#include "Game.h"
#include <iostream>

sf::Color getRandomBrightColor() {
    int r, g, b;

    do {
        r = getRandom<int>(128, 255); // Red component (bright range)
        g = getRandom<int>(128, 255); // Green component (bright range)
        b = getRandom<int>(128, 255); // Blue component (bright range)
    } while (r + g + b < 400); // Ensure it's not too dark overall 

    return sf::Color(r, g, b);
}

Game::Game()
    : window(sf::VideoMode(1200, 700), "ECS Game") {
    window.setFramerateLimit(60);

    // Seed the random number generator once
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Load best scores from file
    loadBestScores("shape_scores.txt", bestScores);

    // Initialize the HUD 
    initializeHUD();
    initializeGameOverText();

    // Center the player in the screen
    auto player = entityManager.addEntity("player");
    float centerX = window.getSize().x / 2.0f;
    float centerY = window.getSize().y / 2.0f;
    player->add<CTransform>(Vec2<float>(centerX, centerY), Vec2<float>(0.0f, 0.0f)); // Centered position
    player->add<CInput>();

    // Set random player color
    sf::Color PlayerColor = getRandomBrightColor();

    // Set random player number of sides
    int playerShapeSides = getRandom<int>(3, 8);

    // Add Components to player
    player->add<CShape>(playerShapeSides, playerRadius, PlayerColor);
    player->add<CRotation>(0.0f, playerRotationSpeed);
    player->add<CCollision>(playerRadius, true, false);
    player->add<CLives>(playerLives);
    player->add<CState>();


    // Initialize supermove timer based on player shape sides
    supermoveCooldown = playerShapeSides; // Cooldown duration is equal to the number of sides
    supermoveTimer = 0.0f;     // Timer starts at 0
}

Game::~Game() {
}

void Game::run() {
    sf::Clock deltaClock; // SFML clock for delta time

    while (window.isOpen()) {
        // Handle input
        handleInput();

        // Calculate delta time
        sf::Time deltaTime = deltaClock.restart();
        float dt = deltaTime.asSeconds();

        // Update game logic
        update(dt);

        // Render the game
        render();
    }
}

// Input Handling

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close the window if the close event is triggered
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Handle mouse button inputs
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                fireBullet(); // Fire a normal bullet
            }
        }

        // Handle keypress inputs
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space && supermoveReady) {
                activateSupermove(); // Trigger supermove
            }
        }
    }

    // Update the player's input component for movement (WASD keys)
    for (auto& entity : entityManager.getEntities("player")) {
        auto& input = entity->get<CInput>();

        // Record input states
        input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    }
}

void Game::fireBullet(bool isSupermove) {
    // Check if the bullet is on cooldown (only for normal bullets)
    if (!isSupermove && bulletCooldownTimer > 0.0f) {
        return;
    }

    // Get the player's position
    auto& player = entityManager.getEntities("player").front();
    auto& playerTransform = player->get<CTransform>();

    // Calculate the direction toward the mouse position
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);

    Vec2<float> direction(
        worldMousePosition.x - playerTransform.position.x,
        worldMousePosition.y - playerTransform.position.y
    );

    // Define bullet speed based on the type (supermove or normal)
    float speed = isSupermove ? superBulletSpeed : bulletSpeed; // Supermove: faster, Normal: slower

    // Create a bullet entity
    auto bullet = entityManager.addEntity("bullet");
    bullet->add<CTransform>(
        playerTransform.position, // Spawn bullet at player's position
        direction * speed         // Apply the fixed speed multiplier
    );

    bullet->add<CShape>(
        20,                          // Use high number of sides to approximate circular shape
        playerRadius / 6.0f,         // Use smaller radius for bullets
        isSupermove ? sf::Color::Red : sf::Color::White // Color based on type (super or not)
    );
    bullet->add<CLifeSpan>(bulletLifeTime);  // Bullets live for bulletLifeTime second

    // Reset bullet cooldown timer for normal bullets
    if (!isSupermove) {
        bulletCooldownTimer = bulletCooldown;
    }
}

void Game::activateSupermove() {
    if (!supermoveReady) {
        return;
    }

    // Get the player's position and shape
    auto& player = entityManager.getEntities("player").front();
    auto& playerTransform = player->get<CTransform>();
    auto& playerShape = player->get<CShape>();

    // Fire bullets in directions based on the number of sides of the player's shape
    float angleIncrement = 360.0f / playerShape.sides; // Divide 360° by the number of sides
    for (int i = 0; i < playerShape.sides; ++i) {
        float angle = angleIncrement * i;

        // Calculate direction vector for this bullet
        float radian = angle * (3.14159265f / 180.0f); // Convert to radians
        Vec2<float> direction(std::cos(radian), std::sin(radian));
        direction.normalize(); // Ensure consistent speed

        // Create a bullet entity
        auto bullet = entityManager.addEntity("bullet");
        bullet->add<CTransform>(
            playerTransform.position, // Spawn bullet at player's position
            direction * 500.0f        // Fixed bullet speed for supermove
        );
        bullet->add<CShape>(
            20,                          
            playerRadius / 2.0f,         
            sf::Color::Red               // Red color for supermove bullets
        );
        bullet->add<CLifeSpan>(bulletLifeTime * 1.5); //Make the super bullets last longer than normal bullets
    }

    // Set supermove on cooldown
    supermoveReady = false;
    supermoveTimer = supermoveCooldown;
}

void Game::update(float dt) {
    if (gameState == GameState::GameOver) {
        return;
    }

    updateSurvivalPoints(dt);
    updateBulletCooldown(dt);
    updatePlayerInvincibility(dt);
    updateSupermoveCooldown(dt);

    entityManager.update(); // Update ECS
    spawnEnemies(dt);       // Spawn enemies
    updateBullets(dt);      // Update bullets
    updateFragments(dt);    // Update fragments

    // Update enemy spawn times
    for (auto& enemy : entityManager.getEntities("enemy")) {
        auto& spawnTime = enemy->get<CSpawnTime>();
        spawnTime.timeSinceSpawn += dt;
    }

    updatePlayer(dt);
    processEnemyMovement(dt);

    updateCollisions();  // Handle collisions
    updateHUD();         // Update HUD
}

void Game::updateSurvivalPoints(float dt) {
    survivalTimer += dt;
    if (survivalTimer >= 1.0f) { // Every second
        totalPoints += 100;      // Award points
        survivalTimer = 0.0f;    // Reset timer
    }
}

void Game::updatePlayerInvincibility(float dt) {
    for (auto& player : entityManager.getEntities("player")) {
        auto& state = player->get<CState>();

        if (state.isInvincible) {
            state.invincibilityTimer -= dt; // Decrease the timer
            if (state.invincibilityTimer <= 0.0f) {
                state.isInvincible = false; // End invincibility
                state.invincibilityTimer = 0.0f; // Ensure timer is reset
            }
        }
    }
}

void Game::updateBulletCooldown(float dt) {
    if (bulletCooldownTimer > 0.0f) {
        bulletCooldownTimer -= dt; // Decrease cooldown
        if (bulletCooldownTimer < 0.0f) {
            bulletCooldownTimer = 0.0f; // Clamp to zero
        }
    }
}

void Game::updateSupermoveCooldown(float dt) {
    if (!supermoveReady) {
        supermoveTimer -= dt;
        if (supermoveTimer <= 0.0f) {
            supermoveReady = true; // Supermove is ready again
            supermoveTimer = 0.0f; // Reset timer
        }
    }
}

void Game::updateBullets(float dt) {
    for (auto& bullet : entityManager.getEntities("bullet")) {
        auto& transform = bullet->get<CTransform>();
        auto& lifespan = bullet->get<CLifeSpan>();

        // Update bullet position
        transform.position += transform.velocity * dt;

        // Reduce lifespan
        lifespan.remainingTime -= dt;

        // Destroy bullet if its lifespan is over
        if (lifespan.remainingTime <= 0.0f) {
            bullet->destroy();
        }
    }
}

void Game::updateFragments(float dt) {
    for (auto& fragment : entityManager.getEntities("fragment")) {
        auto& transform = fragment->get<CTransform>();
        auto& shape = fragment->get<CShape>();
        auto& lifespan = fragment->get<CLifeSpan>();
        auto& rotation = fragment->get<CRotation>();

        // Update position using velocity
        transform.position += transform.velocity * dt;
        // Update rotation
        rotation.angle += rotation.speed * dt; // Increment rotation angle
        if (rotation.angle >= 360.0f) rotation.angle -= 360.0f; // Wrap within [0, 360)

        // Decrease lifespan
        lifespan.remainingTime -= dt;

        // Fade out (reduce alpha over time)
        sf::Color color = shape.color;
        float alpha = (lifespan.remainingTime / lifespan.totalTime) * 255.0f; // Scale alpha
        color.a = static_cast<int>(std::max(0.0f, alpha));
        shape.color = color;

        // Destroy fragment if lifespan is over
        if (lifespan.remainingTime <= 0.0f) {
            fragment->destroy();
        }
    }
}

void Game::updateCollisions() {
    auto& players = entityManager.getEntities("player");
    auto& enemies = entityManager.getEntities("enemy");

    for (auto& player : players) {
        auto& playerTransform = player->get<CTransform>();
        auto& playerCollision = player->get<CCollision>();
        auto& playerState = player->get<CState>();
        auto& playerShape = player->get<CShape>();


        for (auto& enemy : entityManager.getEntities("enemy")) {
            auto& enemyTransform = enemy->get<CTransform>();
            auto& enemyCollision = enemy->get<CCollision>();
            auto& spawnTime = enemy->get<CSpawnTime>();

            // Skip collision if the enemy has just spawned
            if (spawnTime.timeSinceSpawn < spawnProtectionTime) {
                continue;
            }

            // Calculate distance
            float dx = playerTransform.position.x - enemyTransform.position.x;
            float dy = playerTransform.position.y - enemyTransform.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Check if the distance is less than or equal to the sum of their radii
            float collisionThreshold = playerCollision.radius + enemyCollision.radius;

            if (distance <= collisionThreshold) {
                // Skip collision logic if the player is invincible
                if (playerState.isInvincible) {
                    continue;
                }

                // Reduce lives
                playerLives--;

                //If Lives <=0 
                if (playerLives <= 0) {
                    handlePlayerDeath(totalPoints, playerShape.sides);
                }

                // Reset player position and enable invincibility
                playerTransform.position = {
                    static_cast<float>(window.getSize().x) / 2.0f,
                    static_cast<float>(window.getSize().y) / 2.0f
                };
                playerState.isInvincible = true; // Make the player invincible
                playerState.invincibilityTimer = playerInvincibilityTime; // Set invincibility duration
            }
        }
    }
    // Handle enemy-enemy collisions
    for (size_t i = 0; i < entityManager.getEntities("enemy").size(); ++i) {
        auto& enemy1 = enemies[i];
        auto& transform1 = enemy1->get<CTransform>();
        auto& collision1 = enemy1->get<CCollision>();

        for (size_t j = i + 1; j < enemies.size(); ++j) {
            auto& enemy2 = enemies[j];
            auto& transform2 = enemy2->get<CTransform>();
            auto& collision2 = enemy2->get<CCollision>();

            // Calculate distance
            float dx = transform1.position.x - transform2.position.x;
            float dy = transform1.position.y - transform2.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Check for collision
            float collisionThreshold = collision1.radius + collision2.radius;
            if (distance <= collisionThreshold) {
                // Separate the enemies to prevent sticking
                float overlap = collisionThreshold - distance + 0.1f; // Add a small buffer
                float nx = dx / distance; // Normalized x direction
                float ny = dy / distance; // Normalized y direction

                // Push enemies apart based on overlap
                transform1.position.x += nx * (overlap / 2.0f);
                transform1.position.y += ny * (overlap / 2.0f);
                transform2.position.x -= nx * (overlap / 2.0f);
                transform2.position.y -= ny * (overlap / 2.0f);

                // Adjust velocities for bounce
                transform1.velocity.x = -transform1.velocity.x * 1.3f; // Add a slight speed boost
                transform1.velocity.y = -transform1.velocity.y * 1.3f;

                transform2.velocity.x = -transform2.velocity.x * 1.3f;
                transform2.velocity.y = -transform2.velocity.y * 1.3f;
            } else {
                // Reset velocity to original speed when not in collision
                // Normalize and scale velocity to ensure consistent speed
                float magnitude1 = std::sqrt(transform1.velocity.x * transform1.velocity.x + transform1.velocity.y * transform1.velocity.y);
                if (magnitude1 > 0) {
                    transform1.velocity.x = (transform1.velocity.x / magnitude1) * enemySpeed;
                    transform1.velocity.y = (transform1.velocity.y / magnitude1) * enemySpeed;
                }

                float magnitude2 = std::sqrt(transform2.velocity.x * transform2.velocity.x + transform2.velocity.y * transform2.velocity.y);
                if (magnitude2 > 0) {
                    transform2.velocity.x = (transform2.velocity.x / magnitude2) * enemySpeed;
                    transform2.velocity.y = (transform2.velocity.y / magnitude2) * enemySpeed;
                }
            }
        }
        // Manage enemy-bullet collision        
        for (auto& bullet : entityManager.getEntities("bullet")) {
            auto& bulletTransform = bullet->get<CTransform>();

            for (auto& enemy : entityManager.getEntities("enemy")) {
                auto& enemyTransform = enemy->get<CTransform>();
                auto& enemyShape = enemy->get<CShape>();

                // Check if the bullet hits the enemy
                float dx = bulletTransform.position.x - enemyTransform.position.x;
                float dy = bulletTransform.position.y - enemyTransform.position.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                float collisionThreshold = enemyShape.radius; // Bullet is small
                if (distance <= collisionThreshold) {
                    bullet->destroy(); // Destroy the bullet
                    explodeEnemy(enemy); // Trigger enemy explosion
                    totalPoints += enemyShape.sides*100; // reward 100 point per side
                    break; // Exit the loop since the bullet is destroyed
                }
            }
        }        
    }

}

void Game::updatePlayer(float dt) {
    auto& player = entityManager.getEntities("player").front(); // Get the player entity
    auto& transform = player->get<CTransform>();
    auto& shape = player->get<CShape>();
    auto& state = player->get<CState>();
    auto& input = player->get<CInput>();
    auto& rotation = player->get<CRotation>(); // Get rotation component

    // Movement logic based on the input component
    Vec2<float> velocity(0.0f, 0.0f);
    if (input.up) velocity.y -= playerSpeed;
    if (input.down) velocity.y += playerSpeed;
    if (input.left) velocity.x -= playerSpeed;
    if (input.right) velocity.x += playerSpeed;

    transform.position += velocity * dt;

    // Keep the player within screen boundaries
    transform.position.x = std::clamp(transform.position.x, shape.radius, static_cast<float>(window.getSize().x) - shape.radius);
    transform.position.y = std::clamp(transform.position.y, shape.radius, static_cast<float>(window.getSize().y) - shape.radius);

    // Update rotation logic
    rotation.angle += rotation.speed * dt; // Increment rotation angle
    if (rotation.angle >= 360.0f) {
        rotation.angle -= 360.0f; // Wrap around to keep within [0, 360)
    }

    // Handle invincibility
    if (state.isInvincible) {
        state.invincibilityTimer -= dt;
        if (state.invincibilityTimer <= 0.0f) {
            state.isInvincible = false;
            state.invincibilityTimer = 0.0f;
        }
    }
}

// Rendering

void Game::render() {
    window.clear(sf::Color::Black);
    // Draw entities if the game is still playing
    if (gameState == GameState::Playing) {

        // Render bullets
        for (auto& bullet : entityManager.getEntities("bullet")) {
            auto& transform = bullet->get<CTransform>();
            auto& shape = bullet->get<CShape>();

            sf::CircleShape bulletShape(shape.radius, shape.sides); // Match bullet shape
            bulletShape.setOrigin(shape.radius, shape.radius);      // Center the shape
            bulletShape.setFillColor(sf::Color::White);             // White inner color
            bulletShape.setOutlineThickness(2.0f);                 // Set outline thickness
            bulletShape.setOutlineColor(shape.color);              // Match player outline color
            bulletShape.setPosition(transform.position.x, transform.position.y);

            window.draw(bulletShape);
        }

        // Render the player
        for (auto& entity : entityManager.getEntities("player")) {
            auto& transform = entity->get<CTransform>();
            auto& shape = entity->get<CShape>();
            auto& rotation = entity->get<CRotation>();

            // Blinking logic: Alternate visibility during invincibility
            auto& playerState = entity->get<CState>();
            bool renderPlayer = true; // Default: always render
            if (playerState.isInvincible) {
                [[maybe_unused]] int blinkInterval = 200; // Milliseconds
                [[maybe_unused]] int currentTime = static_cast<int>(playerState.invincibilityTimer * 1000); // Convert to ms
                renderPlayer = (currentTime / blinkInterval) % 2 == 0; // Toggle visibility
            }

            // Render the player if visible
            if (renderPlayer) {

                sf::CircleShape circle(shape.radius);
                circle.setOrigin(shape.radius, shape.radius);       
                circle.setFillColor(sf::Color::Black);            // Black fill
                circle.setOutlineThickness(1.0f);                 // Thin outline for the inner circle
                circle.setOutlineColor(shape.color);         // White outline
                circle.setPosition(transform.position.x ,
                                transform.position.y);
                window.draw(circle);

                // Render the outer shape
                sf::CircleShape polygon(shape.radius, shape.sides); // Set radius and sides

                polygon.setOrigin(shape.radius, shape.radius);    
                polygon.setOutlineThickness(1.0f);                
                polygon.setOutlineColor(sf::Color::White);
                polygon.setRotation(rotation.angle); 
                polygon.setPosition(transform.position.x,
                                    transform.position.y);
                window.draw(polygon);
            }
        }
        // Render the enemies
        for (auto& entity : entityManager.getEntities("enemy")) {
            auto& transform = entity->get<CTransform>();
            auto& shape = entity->get<CShape>();
            auto& rotation = entity->get<CRotation>();

            sf::CircleShape polygon(shape.radius, shape.sides);
            polygon.setOrigin(shape.radius, shape.radius);      
            polygon.setFillColor(sf::Color::Black);            
            polygon.setOutlineThickness(4.0f);                
            polygon.setOutlineColor(shape.color);            
            polygon.setRotation(rotation.angle); 
            polygon.setPosition(transform.position.x, 
                                transform.position.y); // Adjust for origin

            
            window.draw(polygon);
        }
        // Render fragments
        for (auto& fragment : entityManager.getEntities("fragment")) {
            auto& transform = fragment->get<CTransform>();
            auto& shape = fragment->get<CShape>();
            auto& rotation = fragment->get<CRotation>();

            sf::CircleShape fragmentShape(shape.radius, shape.sides); // Same shape type as enemy
            fragmentShape.setOrigin(shape.radius, shape.radius);
            fragmentShape.setFillColor(sf::Color::Transparent); // Transparent fill
            fragmentShape.setOutlineThickness(3.0f);
            fragmentShape.setRotation(rotation.angle); 
            fragmentShape.setOutlineColor(shape.color);
            fragmentShape.setPosition(transform.position.x, transform.position.y);
            window.draw(fragmentShape);
        }
        for (auto& clone : entityManager.getEntities("clone")) {
            auto& transform = clone->get<CTransform>();
            auto& shape = clone->get<CShape>();

            sf::CircleShape cloneShape(shape.radius, shape.sides);
            cloneShape.setOrigin(shape.radius, shape.radius);
            cloneShape.setFillColor(sf::Color::Transparent); // Transparent fill
            cloneShape.setOutlineThickness(2.0f);
            cloneShape.setOutlineColor(shape.color); // Updated color with alpha
            cloneShape.setPosition(transform.position.x, transform.position.y);

            window.draw(cloneShape);
        }

        // Render HUD
        sf::Text supermoveDisplay;
        supermoveDisplay.setFont(font); // Ensure the font is loaded
        supermoveDisplay.setCharacterSize(20); // Set font size
        supermoveDisplay.setFillColor(sf::Color::White); // Text color

        // Set the text based on supermove readiness
        if (supermoveReady) {
            supermoveDisplay.setString("Supermove: READY");
        } else {
            supermoveDisplay.setString("Supermove: Available in " + std::to_string(static_cast<int>(std::ceil(supermoveTimer))) + "s");
        }

        // Position the text in the bottom-right corner
        sf::FloatRect textBounds = supermoveDisplay.getLocalBounds();
        supermoveDisplay.setPosition(
            static_cast<float>(window.getSize().x) - textBounds.width - 20.0f, // Align right
            static_cast<float>(window.getSize().y) - textBounds.height - 20.0f // Align bottom
        );
        window.draw(supermoveDisplay);
    }

    window.draw(livesText);
    window.draw(pointsText);
    window.draw(bestScoreText);

    // Draw Game Over message if the game is over
   if (gameState == GameState::GameOver) {
        window.draw(gameOverText);
    }
    window.display();
}

void Game::initializeHUD() {
    // Load a font
    if (!font.loadFromFile("src/font/font.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    // Initialize Score text
    bestScoreText.setFont(font);
    bestScoreText.setCharacterSize(20);
    bestScoreText.setFillColor(sf::Color::White);

    // Initialize lives text
    livesText.setFont(font);
    livesText.setCharacterSize(20); 
    livesText.setFillColor(sf::Color::White); 
    livesText.setPosition(10, window.getSize().y - 40); // Bottom left position
    livesText.setString("Lives Remaining: " + std::to_string(playerLives));

    // Initialize points text
    pointsText.setFont(font);
    pointsText.setCharacterSize(20);
    pointsText.setFillColor(sf::Color::White); 
    pointsText.setPosition(10, 10); // Top left position
    pointsText.setString("Points: " + std::to_string(totalPoints));

    // Initialize supermoveDisplay
    supermoveDisplay.setFont(font);
    supermoveDisplay.setCharacterSize(20);
    supermoveDisplay.setFillColor(sf::Color::White);
}

void Game::updateHUD() {
    // === Supermove Status ===
    if (supermoveReady) {
        supermoveDisplay.setString("Supermove: READY");
    } else {
        supermoveDisplay.setString("Supermove: Available in " + std::to_string(static_cast<int>(std::ceil(supermoveTimer))) + "s");
    }
    supermoveDisplay.setPosition(static_cast<float>(window.getSize().x) - 250.0f, static_cast<float>(window.getSize().y) - 50.0f);

    // === Player Lives Display ===
    livesText.setString("Lives Remaining: " + std::to_string(playerLives));
    livesText.setPosition(20.0f, static_cast<float>(window.getSize().y) - 50.0f);

    // === Points Display ===
    pointsText.setString("Points: " + std::to_string(totalPoints));
    pointsText.setPosition(20.0f, 20.0f);

    // === Best Score Display ===
    if (!entityManager.getEntities("player").empty()) {
        auto& player = entityManager.getEntities("player").front();
        int shapeSides = player->get<CShape>().sides;
        std::string shapeName = getShapeName(shapeSides);

        // Check if the shape has a recorded best score
        int bestScore = bestScores.contains(shapeName) ? bestScores[shapeName] : 0;
        bestScoreText.setString("Best Score for " + shapeName + ": " + std::to_string(bestScore));
        bestScoreText.setPosition(20.0f, 50.0f);
    }
}

void Game::initializeGameOverText() {
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50); // Large font size for the message
    gameOverText.setFillColor(sf::Color::Red); 
    gameOverText.setString("Game Over"); // Text content

    // Center the text in the middle of the screen
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    gameOverText.setPosition(static_cast<float>(window.getSize().x) / 2,
                             static_cast<float>(window.getSize().y) / 2);
}

// Spawning

void Game::spawnEnemies(float dt) {
    enemySpawnTimer += dt;
    if (enemySpawnTimer >= enemySpawnInterval) {
        if (entityManager.countEntities("enemy") < maxEnemyPerFrame) {
            auto enemy = entityManager.addEntity("enemy");

            float x = getRandom<float>(enemyRadius, static_cast<float>(window.getSize().x) - enemyRadius);
            float y = getRandom<float>(enemyRadius, static_cast<float>(window.getSize().y) - enemyRadius);

            float angle = getRandom<float>(0.0f, 360.0f);
            float radian = angle * (3.14159265f / 180.0f);
            Vec2<float> velocity = Vec2<float>(std::cos(radian), std::sin(radian)) * enemySpeed;
            enemy->add<CTransform>(Vec2<float>(x, y), velocity);


            sf::Color EnemyColor = getRandomBrightColor();
            enemy->add<CShape>(getRandom<int>(3, 8), enemyRadius, sf::Color(EnemyColor));
            enemy->add<CRotation>(0.0f, enemyRotationSpeed);
            enemy->add<CCollision>(enemyRadius, true, true);

            // Add the spawn time component
            enemy->add<CSpawnTime>();
            enemySpawnTimer = 0.0f;
        }
    }
}

// Explosions

void Game::explodeEnemy(std::shared_ptr<Entity> enemy) {
    auto& enemyTransform = enemy->get<CTransform>();
    auto& enemyShape = enemy->get<CShape>();

    // Get the center position and radius of the enemy
    Vec2<float> center = enemyTransform.position;
    float radius = enemyShape.radius;
    int sides = enemyShape.sides;
    sf::Color color = enemyShape.color;

    // Destroy the original enemy
    enemy->destroy();

    // Generate fragments
    for (int i = 0; i < sides; i++) {
        // Calculate angle for each fragment
        float angle = (2.0f * M_PI / sides) * i;

        // Calculate direction vector for the fragment
        Vec2<float> direction(std::cos(angle), std::sin(angle));

        // Spawn a new fragment
        auto fragment = entityManager.addEntity("fragment");
        Vec2<float> fragmentVelocity = direction * fragmentSpeed; // Assign outward velocity

        // Fragments are smaller shapes with reduced radius
        fragment->add<CTransform>(center, fragmentVelocity);
        fragment->add<CShape>(sides, radius / 2.0f, color);  // Smaller radius
        fragment->add<CLifeSpan>(fragmentLifeTime);
        fragment->add<CRotation>(0.0f, enemyRotationSpeed); 
    }
}

// Game-Specific Logic

void Game::processPlayerMovement(float dt) {
    for (auto& entity : entityManager.getEntities("player")) {
        auto& transform = entity->get<CTransform>();
        auto& input = entity->get<CInput>();
        auto& rotation = entity->get<CRotation>();
        auto& shape = entity->get<CShape>();

        // Calculate intended velocity based on input
        Vec2<float> intendedVelocity(0.0f, 0.0f);
        if (input.up) intendedVelocity.y -= playerSpeed;
        if (input.down) intendedVelocity.y += playerSpeed;
        if (input.left) intendedVelocity.x -= playerSpeed;
        if (input.right) intendedVelocity.x += playerSpeed;

        // Predict next position
        Vec2<float> nextPosition = transform.position + intendedVelocity * dt;

        // Check boundaries for the next position
        if (nextPosition.x - shape.radius < 0) {
            nextPosition.x = shape.radius; // Stop at the left boundary
        } else if (nextPosition.x + shape.radius > window.getSize().x) {
            nextPosition.x = window.getSize().x - shape.radius; // Stop at the right boundary
        }

        if (nextPosition.y - shape.radius < 0) {
            nextPosition.y = shape.radius; // Stop at the top boundary
        } else if (nextPosition.y + shape.radius > window.getSize().y) {
            nextPosition.y = window.getSize().y - shape.radius; // Stop at the bottom boundary
        }

        // Update position and velocity
        transform.position = nextPosition;
        transform.velocity = intendedVelocity;

        // Update rotation
        rotation.angle += rotation.speed * dt; // Increment rotation angle
        if (rotation.angle >= 360.0f) rotation.angle -= 360.0f; // Wrap within [0, 360)
    }
}

void Game::processEnemyMovement(float dt) {
    for (auto& entity : entityManager.getEntities("enemy")) {
        auto& transform = entity->get<CTransform>();
        auto& shape = entity->get<CShape>();
        auto& rotation = entity->get<CRotation>();

        // Update position using velocity
        transform.position += transform.velocity * dt;

        // Reverse direction if the enemy hits a boundary (considering radius)
        if (transform.position.x - shape.radius <= 0 || transform.position.x + shape.radius >= window.getSize().x) {
            transform.velocity.x = -transform.velocity.x; // Reverse X direction
        }
        if (transform.position.y - shape.radius <= 0 || transform.position.y + shape.radius >= window.getSize().y) {
            transform.velocity.y = -transform.velocity.y; // Reverse Y direction
        }

        // Update rotation
        rotation.angle += rotation.speed * dt; // Increment rotation angle
        if (rotation.angle >= 360.0f) {
            rotation.angle -= 360.0f; // Wrap within [0, 360)
        }
    }
}

void Game::handlePlayerDeath(int currentScore, int shapeSides) {
    std::string shapeName = getShapeName(shapeSides);
    std::cout << "Shape: " << shapeName << "\n";

    if (bestScores[shapeName] < currentScore) {
        bestScores[shapeName] = currentScore;
        std::cout << "New high score for " << shapeName << ": " << currentScore << "\n";

        // Save the updated best scores to the file
        saveBestScores("shape_scores.txt", bestScores);
    } else {
        std::cout << "Game Over! Current score: " << currentScore << "\n";
    }

    gameState = GameState::GameOver;
}

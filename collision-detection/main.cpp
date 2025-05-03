#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <vector>
#include <cmath>

const float SCALE = 30.0f;
const float PLAYER_SPEED = 1.0f;
const float COLLISION_DISTANCE = 60.0f; // Distance to check for collisions

struct Box {
    b2BodyId bodyId;
    sf::RectangleShape shape;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML & Box2C Collision");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        std::cerr << "Could not load font!\n";
        return -1;
    }

    // Create text
    sf::Text text("Click to spawn a box!", font, 20);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10);
    unsigned int collision_count = 0;

    // Create Box2C world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 3.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Create player (green box)
    b2BodyDef playerDef = b2DefaultBodyDef();
    playerDef.type = b2_dynamicBody;
    playerDef.position = (b2Vec2){400.0f / SCALE, 500.0f / SCALE};
    playerDef.fixedRotation = true;
    b2BodyId playerId = b2CreateBody(worldId, &playerDef);

    b2Polygon playerBox = b2MakeBox(40.0f / SCALE, 40.0f / SCALE);
    b2ShapeDef playerShapeDef = b2DefaultShapeDef();
    playerShapeDef.density = 1.0f;
    playerShapeDef.friction = 0.3f;
    b2CreatePolygonShape(playerId, &playerShapeDef, &playerBox);

    sf::RectangleShape playerRect(sf::Vector2f(80, 80));
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setOrigin(40, 40);

    // Create ground
    b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.position = (b2Vec2){400.0f / SCALE, 570.0f / SCALE};
    b2BodyId groundId = b2CreateBody(worldId, &groundDef);

    b2Polygon groundBox = b2MakeBox(400.0f / SCALE, 10.0f / SCALE);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    sf::RectangleShape groundRect(sf::Vector2f(800, 20));
    groundRect.setFillColor(sf::Color::Red);
    groundRect.setOrigin(400, 10);
    groundRect.setPosition(400, 570);

    // Create falling boxes
    std::vector<Box> boxes;
    for (int i = 0; i < 5; i++) {
        b2BodyDef boxDef = b2DefaultBodyDef();
        boxDef.type = b2_dynamicBody;
        boxDef.position = (b2Vec2){(200 + i * 100) / SCALE, 100.0f / SCALE};
        b2BodyId boxId = b2CreateBody(worldId, &boxDef);

        b2Polygon boxShape = b2MakeBox(30.0f / SCALE, 30.0f / SCALE);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        b2CreatePolygonShape(boxId, &shapeDef, &boxShape);

        sf::RectangleShape shape(sf::Vector2f(60, 60));
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin(30, 30);
        shape.setPosition((200 + i * 100), 100);

        boxes.push_back({boxId, shape});
    }

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Move player
        b2Vec2 velocity = b2Body_GetLinearVelocity(playerId);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -PLAYER_SPEED;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = PLAYER_SPEED;
        } else {
            velocity.x = 0;
        }
        b2Body_SetLinearVelocity(playerId, velocity);

        // Step the world
        float timeStep = 1.0f / 60.0f;
        b2World_Step(worldId, timeStep, 4);

        // Update player position
        b2Vec2 playerPos = b2Body_GetPosition(playerId);
        playerRect.setPosition(playerPos.x * SCALE, playerPos.y * SCALE);

        // Check for collisions manually
        for (auto& box : boxes) {
            playerRect.getGlobalBounds().intersects(box.shape.getGlobalBounds());
            collision_count += 1;
        }

        text.setString("Collision Count: " + std::to_string(collision_count));

        // Render
        window.clear();
        window.draw(groundRect);
        window.draw(playerRect);

        // Draw falling boxes
        for (auto& box : boxes) {
            b2Vec2 pos = b2Body_GetPosition(box.bodyId);
            box.shape.setPosition(pos.x * SCALE, pos.y * SCALE);
            window.draw(box.shape);
        }
        window.draw(text);
        window.display();
    }

    b2DestroyWorld(worldId);
    return 0;
}

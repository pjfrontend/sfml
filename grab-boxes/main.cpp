#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <vector>
#include <cmath>

const float SCALE = 30.0f;
const float PLAYER_SPEED = 2.0f; // Reduced speed
const float GRAB_RADIUS = 50.0f;

struct Box {
    b2BodyId bodyId;
    sf::RectangleShape shape;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML & Box2D - Grab Mechanic");

    // Reduce gravity
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 3.0f}; // Reduced gravity
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Ground setup
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){400.0f / SCALE, 570.0f / SCALE};
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(400.0f / SCALE, 10.0f / SCALE);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    sf::RectangleShape groundRect(sf::Vector2f(800, 20));
    groundRect.setFillColor(sf::Color::Green);
    groundRect.setOrigin(400, 10);
    groundRect.setPosition(400, 570);

    // Player (green square)
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

    // Add damping to slow movement over time
    b2Body_SetLinearDamping(playerId, 3.0f); 

    sf::RectangleShape playerRect(sf::Vector2f(80, 80));
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setOrigin(40, 40);

    std::vector<Box> boxes;

    int subStepCount = 4;
    sf::Clock clock;
    bool isGrabbing = false;
    Box* grabbedBox = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Handle player movement
        b2Vec2 velocity = b2Body_GetLinearVelocity(playerId);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -PLAYER_SPEED;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = PLAYER_SPEED;
        } else {
            velocity.x = 0;
        }
        b2Body_SetLinearVelocity(playerId, velocity);

        // Handle grabbing/releasing
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isGrabbing) {
            for (auto& box : boxes) {
                b2Vec2 playerPos = b2Body_GetPosition(playerId);
                b2Vec2 boxPos = b2Body_GetPosition(box.bodyId);
                float dist = std::hypot((playerPos.x - boxPos.x) * SCALE, (playerPos.y - boxPos.y) * SCALE);

                if (dist < GRAB_RADIUS) {
                    grabbedBox = &box;
                    isGrabbing = true;
                    break;
                }
            }
        } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isGrabbing) {
            grabbedBox = nullptr;
            isGrabbing = false;
        }

        // Spawn falling boxes at intervals
        if (clock.getElapsedTime().asSeconds() > 1.5f) { // Increased interval to slow spawning
            float spawnX = static_cast<float>(rand() % 800);
            b2BodyDef boxDef = b2DefaultBodyDef();
            boxDef.type = b2_dynamicBody;
            boxDef.position = (b2Vec2){spawnX / SCALE, 0.0f};
            b2BodyId boxId = b2CreateBody(worldId, &boxDef);

            b2Polygon boxShape = b2MakeBox(30.0f / SCALE, 30.0f / SCALE);
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.friction = 0.3f;
            b2CreatePolygonShape(boxId, &shapeDef, &boxShape);

            b2Body_SetLinearDamping(boxId, 2.0f); // Slows falling objects

            sf::RectangleShape shape(sf::Vector2f(60, 60));
            shape.setFillColor(sf::Color::Blue);
            shape.setOrigin(30, 30);
            shape.setPosition(spawnX, 0);

            boxes.push_back({boxId, shape});
            clock.restart();
        }

        // Box2D physics step
        float timeStep = 1.0f / 60.0f;
        b2World_Step(worldId, timeStep, subStepCount);

        // Update player position
        b2Vec2 playerPos = b2Body_GetPosition(playerId);
        playerRect.setPosition(playerPos.x * SCALE, playerPos.y * SCALE);

        // Update all falling boxes
        for (auto& box : boxes) {
            b2Vec2 pos = b2Body_GetPosition(box.bodyId);
            b2Rot rotation = b2Body_GetRotation(box.bodyId);
            box.shape.setPosition(pos.x * SCALE, pos.y * SCALE);
            box.shape.setRotation(b2Rot_GetAngle(rotation) * 180.0f / b2_pi);
        }

        // Move grabbed box with player
        if (isGrabbing && grabbedBox) {
            b2Vec2 playerPos = b2Body_GetPosition(playerId);
            b2Body_SetTransform(grabbedBox->bodyId, (b2Vec2){playerPos.x, playerPos.y - (50.0f / SCALE)}, {0});
        }

        // Render scene
        window.clear();
        window.draw(groundRect);
        window.draw(playerRect);
        for (const auto& box : boxes) {
            window.draw(box.shape);
        }
        window.display();
    }

    b2DestroyWorld(worldId);
    return 0;
}

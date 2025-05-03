#include <SFML/Graphics.hpp>
#include <vector>

sf::Vector2f normalise_vector(float w, float h, float dist)
{
    return sf::Vector2f(w/dist, h/dist);
}

void circle_constraint(sf::CircleShape &anchor, sf::CircleShape &target, float constraint)
{
    float width = target.getPosition().x - anchor.getPosition().x;
    float height = target.getPosition().y - anchor.getPosition().y;
    float distance = std::sqrt(width * width + height * height);
    if(distance < constraint){
        return;
    }

    // get unit vector
    sf::Vector2f unit = normalise_vector(width, height, distance);

    // multiply unit vector by constraint
    sf::Vector2f projection = sf::Vector2f(unit.x * constraint, unit.y * constraint) + anchor.getPosition();

    // constrain follower
    target.setPosition(projection);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Circle Constraints");

    std::vector<sf::CircleShape> segments;
    for (size_t i = 0; i < 3; i++)
    {
        sf::CircleShape shape(50.f);
        shape.setFillColor( i == 0 ? sf::Color::Green : sf::Color::Red);
        shape.setPosition(150,150);
        segments.push_back(shape);
    }
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // UPDATE
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            segments[0].move(-0.1f, 0.f);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            segments[0].move(0.1f, 0.f);
        }

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            segments[0].move(0.f, -0.1f);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            segments[0].move(0.f, 0.1f);
        }

        for (size_t i = 1; i < segments.size(); i++)
        {
            circle_constraint(segments[i-1], segments[i], 100.f);
        }

        // DRAW
        window.clear();
        for (size_t i = 0; i < segments.size(); i++)
        {
            window.draw(segments[(segments.size() -1)-i]);
        }
        window.display();
    }

    return 0;
}
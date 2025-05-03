#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int SQUARE_SIZE = 10;
const int NUM_BLUE_SQUARES = 500;
const int GRID_SIZE = 20; // Size of each cell in the flow field grid

// Directions for grid movement (up, down, left, right)
const sf::Vector2i directions[] = {
    sf::Vector2i(0, -1),  // Up
    sf::Vector2i(1, 0),   // Right
    sf::Vector2i(0, 1),   // Down
    sf::Vector2i(-1, 0)   // Left
};

// Structure to store each grid cell's flow vector
struct FlowFieldCell {
    sf::Vector2f direction;
    bool visited = false;
    bool isObstacle = false; // Flag to mark obstacle cells
};

// Flow Field Class
class FlowField {
public:
    FlowField(int width, int height, int cellSize)
        : width(width), height(height), cellSize(cellSize) {
        // Calculate number of cells in each dimension
        gridWidth = width / cellSize;
        gridHeight = height / cellSize;
        flowField.resize(gridWidth, std::vector<FlowFieldCell>(gridHeight));
    }

    // Generate the flow field based on a given goal position
    void generateFlowField(sf::Vector2f goal) {
        // Reset all cells
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                flowField[x][y].visited = false;
                flowField[x][y].direction = sf::Vector2f(0, 0);
            }
        }

        // BFS queue
        std::queue<sf::Vector2i> queue;
        sf::Vector2i goalCell(goal.x / cellSize, goal.y / cellSize);

        // Start BFS from the goal position
        queue.push(goalCell);
        flowField[goalCell.x][goalCell.y].visited = true;

        // BFS propagation
        while (!queue.empty()) {
            sf::Vector2i current = queue.front();
            queue.pop();

            // For each direction (up, right, down, left)
            for (const sf::Vector2i& dir : directions) {
                sf::Vector2i neighbor = current + dir;

                // Check if the neighbor is within bounds and not an obstacle
                if (neighbor.x >= 0 && neighbor.x < gridWidth &&
                    neighbor.y >= 0 && neighbor.y < gridHeight &&
                    !flowField[neighbor.x][neighbor.y].visited &&
                    !flowField[neighbor.x][neighbor.y].isObstacle) {

                    // Set the direction to move from the current cell to the neighbor
                    flowField[neighbor.x][neighbor.y].direction = sf::Vector2f(dir.x, dir.y);
                    flowField[neighbor.x][neighbor.y].visited = true;

                    // Add the neighbor to the queue
                    queue.push(neighbor);
                }
            }
        }
    }

    // Set a cell as an obstacle
    void setObstacle(int x, int y) {
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            flowField[x][y].isObstacle = true;
        }
    }

    // Get the flow direction at a specific position
    sf::Vector2f getFlowDirection(sf::Vector2f position) {
        int cellX = position.x / cellSize;
        int cellY = position.y / cellSize;

        // Make sure we don't go out of bounds
        if (cellX < 0 || cellX >= gridWidth || cellY < 0 || cellY >= gridHeight)
            return sf::Vector2f(0, 0); // Return zero if out of bounds

        return flowField[cellX][cellY].direction;
    }

private:
    int width, height, cellSize;
    int gridWidth, gridHeight;
    std::vector<std::vector<FlowFieldCell>> flowField;
};

// Function to generate a random position on the screen's edge
sf::Vector2f generateRandomEdgePosition() {
    int side = rand() % 4;

    if (side == 0) { // Top
        return sf::Vector2f(rand() % WINDOW_WIDTH, 0);
    } else if (side == 1) { // Bottom
        return sf::Vector2f(rand() % WINDOW_WIDTH, WINDOW_HEIGHT);
    } else if (side == 2) { // Left
        return sf::Vector2f(0, rand() % WINDOW_HEIGHT);
    } else { // Right
        return sf::Vector2f(WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
    }
}

// Function to clamp position inside window bounds
sf::Vector2f clampToWindowBounds(sf::Vector2f position) {
    position.x = std::max(0.f, std::min(position.x, (float)WINDOW_WIDTH - SQUARE_SIZE));
    position.y = std::max(0.f, std::min(position.y, (float)WINDOW_HEIGHT - SQUARE_SIZE));
    return position;
}

int main() {
    srand(time(0));

    // Create window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flow Field with Obstacles");

    // Create the flow field
    FlowField flowField(WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    // Create a green square (target)
    sf::RectangleShape greenSquare(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    greenSquare.setFillColor(sf::Color::Green);

    // Create a collection of blue squares (agents)
    std::vector<sf::Vector2f> blueSquares;
    std::vector<sf::RectangleShape> blueSquareShapes;
    for (int i = 0; i < NUM_BLUE_SQUARES; ++i) {
        blueSquares.push_back(generateRandomEdgePosition());
        sf::RectangleShape blueSquareShape(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        blueSquareShape.setFillColor(sf::Color::Blue);
        blueSquareShapes.push_back(blueSquareShape);
    }

    // Create obstacles (red squares)
    std::vector<sf::RectangleShape> redSquareShapes;
    for (int i = 0; i < 10; ++i) {
        sf::Vector2f pos(rand() % (WINDOW_WIDTH / GRID_SIZE) * GRID_SIZE, rand() % (WINDOW_HEIGHT / GRID_SIZE) * GRID_SIZE);
        redSquareShapes.push_back(sf::RectangleShape(sf::Vector2f(GRID_SIZE, GRID_SIZE)));
        redSquareShapes.back().setFillColor(sf::Color::Red);
        redSquareShapes.back().setPosition(pos);

        // Set this cell as an obstacle in the flow field
        flowField.setObstacle(pos.x / GRID_SIZE, pos.y / GRID_SIZE);
    }

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                // Move the green square to the mouse position
                if (event.mouseButton.button == sf::Mouse::Left) {
                    greenSquare.setPosition(event.mouseButton.x - SQUARE_SIZE / 2, event.mouseButton.y - SQUARE_SIZE / 2);
                    flowField.generateFlowField(greenSquare.getPosition()); // Regenerate the flow field on click
                }
            }
        }

        // Update the movement of the blue squares towards the green square
        for (auto& blueSquare : blueSquares) {
            // Get the flow direction at the blue square's current position
            sf::Vector2f flow = flowField.getFlowDirection(blueSquare);

            // Adjust the movement based on the flow direction
            if (flow != sf::Vector2f(0, 0)) {
                blueSquare += flow * -0.1f; // Adjust speed
            }

            // Clamp the blue square position to the window bounds
            blueSquare = clampToWindowBounds(blueSquare);

            // Check if a blue square touches the green square
            if (std::abs(blueSquare.x - greenSquare.getPosition().x) < SQUARE_SIZE &&
                std::abs(blueSquare.y - greenSquare.getPosition().y) < SQUARE_SIZE) {
                // Reassign the blue square to a random position on the edge
                blueSquare = generateRandomEdgePosition();
            }
        }

        window.clear(sf::Color::White);
        window.draw(greenSquare);

        // Draw the red obstacles
        for (const auto& redSquare : redSquareShapes) {
            window.draw(redSquare);
        }

        // Draw the blue squares
        for (size_t i = 0; i < NUM_BLUE_SQUARES; i++) {
            blueSquareShapes[i].setPosition(blueSquares[i]);
            window.draw(blueSquareShapes[i]);
        }

        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Кількість тайлів по ширині та висоті
int WIDTH = 20;
int HEIGHT = 20;
const int TILE_SIZE = 20;
const float MOVE_INTERVAL = 0.1f;

class Snake {
public:
    sf::Vector2i direction;
    int length;
    std::vector<sf::RectangleShape> body;

    Snake() {
        length = 1;
        direction = sf::Vector2i(1, 0); // Початковий напрямок (рух вправо)
        body.push_back(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        body[0].setPosition(WIDTH / 2 * TILE_SIZE, HEIGHT / 2 * TILE_SIZE); // Початкова позиція змії
        body[0].setFillColor(sf::Color::Green);
    }

    void move() {
        // Створення нового сегмента (голова змії)
        sf::RectangleShape newHead(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        newHead.setPosition(body[0].getPosition() + sf::Vector2f(direction.x * TILE_SIZE, direction.y * TILE_SIZE));
        newHead.setFillColor(sf::Color::Green);

        // Додаємо нову голову змії
        body.insert(body.begin(), newHead);

        // Видаляємо останній сегмент, якщо змія не з'їла яблуко
        if (body.size() > length) {
            body.pop_back();
        }
    }

    void changeDirection(int dx, int dy) {
        if ((dx == 1 && direction.x == -1) || (dx == -1 && direction.x == 1) ||
            (dy == 1 && direction.y == -1) || (dy == -1 && direction.y == 1))
            return;

        direction.x = dx;
        direction.y = dy;
    }

    sf::FloatRect getBounds() {
        return body[0].getGlobalBounds();
    }

    void grow() {
        length++;
    }

    void die() {
        std::cout << "Game Over! Final score: " << (length - 1) << std::endl;
        body.clear();
        length = 0;
    }

    bool isAlive() {
        return length > 0;
    }

    int getLength() {
        return length;
    }
};

class Food {
public:
    sf::RectangleShape shape;

    Food() {
        shape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        shape.setFillColor(sf::Color::Red);
    }

    // Функція для спавну їжі в межах ігрового поля, але не в межах червоних стін
    void spawn(const Snake& snake, int width, int height, int appleCount) {
        bool collidesWithSnake;
        if (appleCount % 5 == 0) {
            // Якщо кожне п'яте яблуко, спавнимо його в центрі поля
            int x = width / 2;
            int y = height / 2;
            shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        }
        else {
            // Інакше, спавнимо їжу в випадковому місці в межах червоного поля
            do {
                collidesWithSnake = false;
                int x = 1 + rand() % (width - 2);  // Додаємо 1 і віднімаємо 2, щоб яблуко не спавнювалось на межах
                int y = 1 + rand() % (height - 2); // Те саме для висоти
                shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);

                // Перевіряємо, чи не спавнюється їжа на зміні
                for (const auto& segment : snake.body) {
                    if (shape.getGlobalBounds().intersects(segment.getGlobalBounds())) {
                        collidesWithSnake = true;
                        break;
                    }
                }
            } while (collidesWithSnake);  // Повторюємо до тих пір, поки їжа не буде на тілі змії
        }
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

// Функція для перевірки зіткнення зі стінами
bool checkCollisionWithBorders(Snake& snake) {
    sf::FloatRect headBounds = snake.getBounds();

    // Перевіряємо, чи голова змії виходить за межі червоного поля
    return headBounds.left < TILE_SIZE || headBounds.top < TILE_SIZE ||
        headBounds.left + headBounds.width > WIDTH * TILE_SIZE - TILE_SIZE ||
        headBounds.top + headBounds.height > HEIGHT * TILE_SIZE - TILE_SIZE;
}

void drawBorders(sf::RenderWindow& window) {
    sf::RectangleShape topBorder(sf::Vector2f(WIDTH * TILE_SIZE, TILE_SIZE));
    topBorder.setFillColor(sf::Color::Red);
    topBorder.setPosition(0, 0);

    sf::RectangleShape bottomBorder(sf::Vector2f(WIDTH * TILE_SIZE, TILE_SIZE));
    bottomBorder.setFillColor(sf::Color::Red);
    bottomBorder.setPosition(0, (HEIGHT - 1) * TILE_SIZE);

    sf::RectangleShape leftBorder(sf::Vector2f(TILE_SIZE, HEIGHT * TILE_SIZE));
    leftBorder.setFillColor(sf::Color::Red);
    leftBorder.setPosition(0, 0);

    sf::RectangleShape rightBorder(sf::Vector2f(TILE_SIZE, HEIGHT * TILE_SIZE));
    rightBorder.setFillColor(sf::Color::Red);
    rightBorder.setPosition((WIDTH - 1) * TILE_SIZE, 0);

    window.draw(topBorder);
    window.draw(bottomBorder);
    window.draw(leftBorder);
    window.draw(rightBorder);
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    sf::RenderWindow window(sf::VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "Snake Game");
    window.setFramerateLimit(60);

    Snake snake;
    Food food;
    int applesEaten = 0;
    food.spawn(snake, WIDTH, HEIGHT, applesEaten);  // Спавн їжі всередині ігрового поля

    float moveTimer = 0.0f;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Управління напрямком змії
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake.direction.y == 0) snake.changeDirection(0, -1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake.direction.y == 0) snake.changeDirection(0, 1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake.direction.x == 0) snake.changeDirection(-1, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake.direction.x == 0) snake.changeDirection(1, 0);

        if (snake.isAlive()) {
            moveTimer += clock.restart().asSeconds();

            if (moveTimer >= MOVE_INTERVAL) {
                snake.move();
                moveTimer = 0.0f;
            }

            // Якщо змія з'їла яблуко
            if (snake.getBounds().intersects(food.getBounds())) {
                snake.grow();
                applesEaten++;
                food.spawn(snake, WIDTH, HEIGHT, applesEaten);  // Спавн їжі в межах ігрового поля
                std::cout << "Apples Eaten: " << (snake.getLength() - 1) << std::endl;

                // Кожні 5 яблук зменшуємо розмір поля
                if (applesEaten % 5 == 0) {
                    if (WIDTH > 5 && HEIGHT > 5) {  // Перевіряємо, що не зменшуємо розмір до занадто маленького
                        WIDTH--;
                        HEIGHT--;
                    }
                }
            }

            window.clear();
            for (auto& segment : snake.body) {
                window.draw(segment);
            }

            window.draw(food.shape);
            drawBorders(window);

            if (checkCollisionWithBorders(snake)) {
                snake.die();
            }

            window.display();
        }
        else {
            std::cout << "Game Over! Final score: " << (snake.getLength() - 1) << std::endl;
            window.close();
        }
    }

    return 0;
}

#include "raylib.h"
#include <vector>

const int screenWidth = 800;
const int screenHeight = 600;
const int gridSize = 20;
const int gridWidth = screenWidth / gridSize;
const int gridHeight = screenHeight / gridSize;

enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
};

enum Difficulty {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};

struct SnakeSegment {
    int x, y;
};

struct Food {
    int x, y;
};

std::vector<SnakeSegment> snake;
Food food;
Direction direction;
int frameCounter;
bool gameOver;
bool gameWon;
bool showDifficultyScreen = true;
Difficulty difficulty;
int frameSpeed;
int score;

void InitGame() {
    // Init all variables (must be here because of the restarting function)
    gameWon = false;
    gameOver = false;
    frameCounter = 0;
    score = 0;
    direction = RIGHT;
    snake.clear();
    snake.push_back({ gridWidth / 2, gridHeight / 2 });
    snake.push_back({ gridWidth / 2 - 1, gridHeight / 2 });

    while (true) { // Generate a new position for food that doesn't lie on the snake
        bool collision = false;
        int foodX = GetRandomValue(0, gridWidth - 1);
        int foodY = GetRandomValue(0, gridHeight - 1);
        for (const auto& segment : snake) {
            if (foodX == segment.x && foodY == segment.y) {
                collision = true;
                break;
            }
        }
        if (!collision) {
            food = { foodX, foodY };
            break;
        }
    }

    // Set the speed according to the selected difficulty
    switch (difficulty) {
        case EASY: frameSpeed = 20; break;
        case MEDIUM: frameSpeed = 10; break;
        case HARD: frameSpeed = 5; break;
    }
}

void DrawDifficultyScreen() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Snake", screenWidth / 2 - MeasureText("Snake", 50) / 2, screenHeight / 2 - 250, 50, DARKGRAY);
    DrawText("Select Difficulty", screenWidth / 2 - MeasureText("Select Difficulty", 30) / 2, screenHeight / 2 - 100, 30, DARKGRAY);
    DrawText("1. Easy", screenWidth / 2 - MeasureText("1. Easy", 20) / 2, screenHeight / 2 - 20, 20, GREEN);
    DrawText("2. Medium", screenWidth / 2 - MeasureText("2. Medium", 20) / 2, screenHeight / 2 + 20, 20, ORANGE);
    DrawText("3. Hard", screenWidth / 2 - MeasureText("3. Hard", 20) / 2, screenHeight / 2 + 60, 20, RED);

    EndDrawing();
}

void UpdateDifficultyScreen() {
    switch (GetKeyPressed()) {
        case KEY_ONE:
            difficulty = EASY;
            showDifficultyScreen = false;
            InitGame();
            break;
        case KEY_TWO:
            difficulty = MEDIUM;
            showDifficultyScreen = false;
            InitGame();
            break;
        case KEY_THREE:
            difficulty = HARD;
            showDifficultyScreen = false;
            InitGame();
            break;
    }
}

void UpdateGame() {
    if (gameOver || gameWon) {
        if (IsKeyPressed(KEY_ENTER)) {
            showDifficultyScreen = true;
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) && direction != DOWN) direction = UP;
    if (IsKeyPressed(KEY_RIGHT) && direction != LEFT) direction = RIGHT;
    if (IsKeyPressed(KEY_DOWN) && direction != UP) direction = DOWN;
    if (IsKeyPressed(KEY_LEFT) && direction != RIGHT) direction = LEFT;

    frameCounter++;
    if (frameCounter >= frameSpeed) {
        frameCounter = 0;

        SnakeSegment newHead = snake.front();
        if (direction == UP) newHead.y -= 1;
        if (direction == RIGHT) newHead.x += 1;
        if (direction == DOWN) newHead.y += 1;
        if (direction == LEFT) newHead.x -= 1;

        // Check collision with walls
        if (newHead.x < 0 || newHead.x >= gridWidth || newHead.y < 0 || newHead.y >= gridHeight) {
            gameOver = true;
        }

        // Check collision with self
        for (const auto& segment : snake) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                gameOver = true;
            }
        }

        if (!gameOver) {
            snake.insert(snake.begin(), newHead);

            // Check collision with food
            if (newHead.x == food.x && newHead.y == food.y) {
                score++; // Add score
                while (true) { // Generate a new position for food that doesn't lie on the snake
                    bool collision = false;
                    int foodX = GetRandomValue(0, gridWidth - 1);
                    int foodY = GetRandomValue(0, gridHeight - 1);
                    for (const auto& segment : snake) {
                        if (foodX == segment.x && foodY == segment.y) {
                            collision = true;
                            break;
                        }
                    }
                    if (!collision) {
                        food = { foodX, foodY };
                        break;
                    }
                }
            } else {
                snake.pop_back();
            }
        }

        if (score >= 50) {
            gameWon = true;
        }
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (gameOver) {
        if (score == 1)
            DrawText(TextFormat("Game Over! You scored %d point. Press ENTER to restart", score), screenWidth / 2 - MeasureText(TextFormat("Game Over! You scored %d point. Press ENTER to restart", score), 20) / 2, screenHeight / 2 - 10, 20, DARKGRAY);
        else
            DrawText(TextFormat("Game Over! You scored %d points. Press ENTER to restart", score), screenWidth / 2 - MeasureText(TextFormat("Game Over! You scored %d points. Press ENTER to restart", score), 20) / 2, screenHeight / 2 - 10, 20, DARKGRAY);
    }
    else if (gameWon) {
        DrawText("Congratulations! You won! Press ENTER to restart", screenWidth / 2 - MeasureText("Congratulations! You won! Press ENTER to restart", 20) / 2, screenHeight / 2 - 10, 20, DARKGRAY);
    }
    else {
            // Draw snake
            for (const auto& segment : snake) {
                DrawRectangle(segment.x * gridSize, segment.y * gridSize, gridSize, gridSize, DARKGREEN);
            }

            // Draw food
            DrawRectangle(food.x * gridSize, food.y * gridSize, gridSize, gridSize, RED);

            // Draw score
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
    }

    EndDrawing();
}

int main() {
    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (showDifficultyScreen) {
            UpdateDifficultyScreen();
            DrawDifficultyScreen();
        } else {
            UpdateGame();
            DrawGame();
        }
    }

    CloseWindow();
    return 0;
}
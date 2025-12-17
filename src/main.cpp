#include <iostream>
#include <raylib.h>
#include <deque>


const Color Green = { 173, 204, 96, 255 };
const Color DarkGreen = { 43, 51, 24,  255 };

const int ScreenWidth = 400;
const int ScreenHeight = 400;
const int CellSize = 16;

int CellCountX = ScreenWidth / CellSize;
int CellCountY = ScreenHeight / CellSize;

double LastUpdateTime = 0.0;
bool EventTriggered(double Interval) {

    // Check if the specified interval has passed since the last update
    double CurrentTime = GetTime();
    if (CurrentTime - LastUpdateTime >= Interval) {
        LastUpdateTime = CurrentTime;
        return true;
    }
    return false;
}

bool IsElementInDeque(std::deque<Vector2> Deque, Vector2 Element) {
    for (const auto& item : Deque) {
        if (item.x == Element.x && item.y == Element.y) {
            return true;
        }
    }
    return false;
}

class Entity {
public:
    virtual void Draw() = 0;
    virtual Vector2 GetPosition() = 0;
};

class Food : public Entity {
    Vector2 Position; // Position of the food item

public:
    Food(std::deque<Vector2> SnakeBody) {
        Respawn(SnakeBody);
    }

    // Generate a random position within the grid
    Vector2 GenerateRandomPosition() {
        float X = GetRandomValue(0, CellCountX - 1);
        float Y = GetRandomValue(0, CellCountY - 1);
        return Vector2{X, Y};
    }

    // Respawn the food at a random position
    void Respawn(std::deque<Vector2> SnakeBody) {
        
        Vector2 NewPosition = GenerateRandomPosition();
        while (IsElementInDeque(SnakeBody, NewPosition)) NewPosition = GenerateRandomPosition();
        Position = NewPosition;
    }

    Vector2 GetPosition() override {
        return Position;
    }
    // Draw the food
    void Draw() override {
        float Radius = CellSize / 2;
        float CenterX = Position.x * CellSize + Radius;
        float CenterY = Position.y * CellSize + Radius;

        DrawCircle(CenterX, CenterY, Radius, DarkGreen);
    }
};

class Snake : public Entity {

    std::deque<Vector2> Body = {Vector2{6, 6}, Vector2{5, 6}, Vector2{4, 6}}; // Initial snake body segments
public:
    Vector2 Direction = {1, 0}; // Initial movement direction to the right
    bool Growing = false;

    // Draw the snake
    void Draw() override{
        for (const auto& segment : Body)
        {
            Rectangle BodyCell = Rectangle{segment.x * CellSize, segment.y * CellSize, (float)CellSize, (float)CellSize};
            DrawRectangleRounded(BodyCell, 0.5f, 6, DarkGreen);
        }
    }

    std::deque<Vector2> GetBody() {
        return Body;
    }
    
    Vector2 GetPosition() override {
        return Body.front();
    }

    // Move the snake in the current direction
    void Move() {

        Body.push_front(Vector2{Body.front().x + Direction.x, Body.front().y + Direction.y});

        if(Growing) Growing = false;
        else Body.pop_back();
    }

    void Reset() {
        Body = {Vector2{6, 6}, Vector2{5, 6}, Vector2{4, 6}};
        Direction = Vector2{1, 0};
        Growing = false;
    }
};

class Game {

public:
    Snake Player = Snake();
    std::deque<Vector2> PlayerBody = Player.GetBody();
    Food Berry = Food(PlayerBody);
    bool Running = false;

    void Update() {

        if(!Running) return;

        Player.Move(); // Update snake position
        CheckEatFood(); 
        CheckCollisions();
        CheckSelfCollision();
    }
    
    void Draw() {
        Berry.Draw(); // Draw the food
        Player.Draw(); // Draw the snake
    }

    void GameOver() {
        // Reset the game state
        Player.Reset();
        Berry.Respawn(PlayerBody);
        Running = false;
    }

    void CheckEatFood() {
        // Check if the snake's head is at the same position as the food
        if (Player.GetPosition().x == Berry.GetPosition().x && Player.GetPosition().y == Berry.GetPosition().y ) {
            Berry.Respawn(PlayerBody); // Respawn food at a new position
            Player.Growing = true; // Set the snake to grow on the next move
        }
    }

    void CheckSelfCollision() {
        Vector2 Head = Player.GetPosition();
        std::deque<Vector2> Body = Player.GetBody();

        for (size_t i = 1; i < Body.size(); ++i) {
            if (Head.x == Body[i].x && Head.y == Body[i].y) {
                GameOver();
                break;
            }
        }
    }

    void CheckCollisions() {
        if (Player.GetPosition().x < 0 || Player.GetPosition().x >= CellCountX || Player.GetPosition().y < 0 || Player.GetPosition().y >= CellCountY) {
            GameOver();
        }
    }
};

int main() {

    InitWindow(ScreenWidth, ScreenHeight, "Snake Game");
    SetTargetFPS(60);

    
    Game SnakeGame;

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        
        if(EventTriggered(0.1))
            SnakeGame.Update(); // Update game state at fixed intervals

        // Handle input for snake direction
        if(IsKeyPressed(KEY_UP) && SnakeGame.Player.Direction.y == 0)
            SnakeGame.Player.Direction = Vector2{0, -1};
        if(IsKeyPressed(KEY_DOWN) && SnakeGame.Player.Direction.y == 0)
            SnakeGame.Player.Direction = Vector2{0, 1};
        if(IsKeyPressed(KEY_LEFT) && SnakeGame.Player.Direction.x == 0)
            SnakeGame.Player.Direction = Vector2{-1, 0};
        if(IsKeyPressed(KEY_RIGHT) && SnakeGame.Player.Direction.x == 0)
            SnakeGame.Player.Direction = Vector2{1, 0};

        if(IsKeyPressed(KEY_ENTER) && !SnakeGame.Running)
            SnakeGame.Running = true;

        ClearBackground(Green); // Clear the screen with green color
        SnakeGame.Draw(); // Draw the game entities

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
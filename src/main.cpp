#include <iostream>
#include <raylib.h>
#include <deque>


const Color Gray = { 100, 100, 100,  255 };
const Color DarkGray = { 56, 56, 56, 255 };
const Color LightGray = { 176, 176, 176, 255 };
const Color TransGray = { 128, 128, 128, 100 };

const Color Red = { 255, 94, 94, 255 };

const int CellSize = 16;
const int CellCountX = 25;
const int CellCountY = 25;

int OffsetX = 60;
int OffsetY = 60;

const int ScreenWidth = 2*OffsetX + CellCountX * CellSize;
const int ScreenHeight = 2*OffsetY + CellCountY * CellSize;

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
        float CenterX = Position.x * CellSize + Radius + OffsetX;
        float CenterY = Position.y * CellSize + Radius + OffsetY;

        DrawCircle(CenterX, CenterY, Radius, Red);
    }
};

class Snake : public Entity {

    std::deque<Vector2> Body = {Vector2{12, 12}}; // Initial snake body segments
public:
    Vector2 Direction = {1, 0}; // Initial movement direction to the right
    bool Growing = false;

    // Draw the snake
    void Draw() override{
        for (size_t i = 0; i < Body.size(); ++i)
        {
            Rectangle BodyCell = Rectangle{OffsetX + Body[i].x * CellSize, OffsetY + Body[i].y * CellSize, (float)CellSize, (float)CellSize};
            Color SegmentColor = (i == 0) ? DarkGray : Gray;    
            DrawRectangleRounded(BodyCell, 0.5f, 6, SegmentColor);
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
        Body = {Vector2{12, 12}};
        Direction = Vector2{1, 0};
        Growing = false;
    }
};

class Game {

public:
    Snake Player = Snake();
    std::deque<Vector2> PlayerBody = Player.GetBody();
    Food Berry = Food(PlayerBody);
    bool IsOver = false;
    bool IsStarted = false;
    bool Running = false;
    int score = 0;

    void Update() {

        if(!Running) return;

        Player.Move(); // Update snake position
        CheckEatFood(); 
        if(CheckCollisions()) GameOver();

    }
    
    void Draw() {
        Berry.Draw(); // Draw the food
        Player.Draw(); // Draw the snake
    }

    void GameOver() {
        // Reset the game state
        Running = false;
        IsOver = true;
        score = 0;
    }

    void Restart() {
        Player.Reset();
        Berry.Respawn(PlayerBody);
        IsOver = false;
        IsStarted = false;
        Running = false;
        score = 0;
    }

    void CheckEatFood() {
        // Check if the snake's head is at the same position as the food
        if (Player.GetPosition().x == Berry.GetPosition().x && Player.GetPosition().y == Berry.GetPosition().y ) {
            Berry.Respawn(PlayerBody); // Respawn food at a new position
            Player.Growing = true; // Set the snake to grow on the next move
            score ++;
        }
    }

    bool CheckSelfCollision() {
        Vector2 Head = Player.GetPosition();
        std::deque<Vector2> Body = Player.GetBody();

        for (size_t i = 1; i < Body.size(); ++i) {
            if (Head.x == Body[i].x && Head.y == Body[i].y) {
                return true;
            }
        }

        return false;
    }

    bool CheckCollisions() {
        if (Player.GetPosition().x < 0 || Player.GetPosition().x >= CellCountX || Player.GetPosition().y < 0 || Player.GetPosition().y >= CellCountY || CheckSelfCollision()) return true;

        return false;
    }
};

int main() {

    InitWindow(ScreenWidth, ScreenHeight, "Snake Game");
    SetTargetFPS(60);

    
    Game SnakeGame;

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        if(EventTriggered(0.1) && !SnakeGame.IsOver)
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

        if(IsKeyPressed(KEY_ENTER) && SnakeGame.IsOver)
            SnakeGame.Restart();
            
        ClearBackground(WHITE); // Clear the screen with green color
        DrawRectangleLinesEx(Rectangle{(float)OffsetX - 5, (float)OffsetY - 5, (float)(CellCountX * CellSize+10), (float)(CellCountY * CellSize+10)}, 5, LightGray); // Draw the game area background
        DrawText("Snake Game", OffsetX, OffsetY-30, 20, DarkGray);
        DrawText(TextFormat("Score: %d", SnakeGame.score), ScreenWidth - OffsetX - 80, OffsetY - 30, 20, DarkGray);
        
        if(!SnakeGame.IsStarted && !SnakeGame.Running && !SnakeGame.IsOver) {
            DrawText("Press enter to start", OffsetX+100, OffsetY+130, 20, DarkGray);
            DrawRectangle((float)OffsetX - 5, (float)OffsetY - 5, (float)(CellCountX * CellSize+10), (float)(CellCountY * CellSize+10), TransGray); 
        
        }

        if(SnakeGame.IsOver && !SnakeGame.Running) {
            DrawText("Game Over!", OffsetX+150, OffsetY+130, 20, DarkGray);
            DrawText("Press enter to restart", OffsetX+90, OffsetY+150, 20, DarkGray);
            DrawRectangle((float)OffsetX - 5, (float)OffsetY - 5, (float)(CellCountX * CellSize+10), (float)(CellCountY * CellSize+10), TransGray); 
        
        }
        SnakeGame.Draw(); // Draw the game entities

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
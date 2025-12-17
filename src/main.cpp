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

class Entity {
public:
    virtual void Draw() = 0;
};

class Food : public Entity {
    Vector2 Position; // Position of the food item

public:
    Food() {
        Respawn();
    }

    // Respawn the food at a random position
    void Respawn() {
        Position.x = GetRandomValue(0, CellCountX - 1);
        Position.y = GetRandomValue(0, CellCountY - 1);
    }

    // Draw the food
    void Draw() override {
        DrawCircle(Position.x * CellSize + CellSize / 2, Position.y * CellSize + CellSize / 2, CellSize / 2, DarkGreen);
    }
};

class Snake : public Entity {

    std::deque<Vector2> Body = {Vector2{6, 6}, Vector2{5, 6}, Vector2{4, 6}}; // Initial snake body segments
public:
    Vector2 Direction = {1, 0}; // Initial movement direction to the right

    // Draw the snake
    void Draw() override{
        for (const auto& segment : Body)
        {
            Rectangle BodyCell = Rectangle{segment.x * CellSize, segment.y * CellSize, (float)CellSize, (float)CellSize};
            DrawRectangleRounded(BodyCell, 0.5f, 6, DarkGreen);
        }
    }

    // Move the snake in the current direction
    void Move() {
        Body.pop_back();
        Body.push_front(Vector2{Body.front().x + Direction.x, Body.front().y + Direction.y});
    }
};

int main() {

    InitWindow(ScreenWidth, ScreenHeight, "Snake Game");
    SetTargetFPS(60);

    Food* Berry = new Food();
    Snake* Player = new Snake();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        
        // Drawing

        if(EventTriggered(0.4))
            Player->Move(); // Update snake position

        // Handle input for snake direction
        if(IsKeyPressed(KEY_UP) && Player->Direction.y == 0)
            Player->Direction = Vector2{0, -1};
        if(IsKeyPressed(KEY_DOWN) && Player->Direction.y == 0)
            Player->Direction = Vector2{0, 1};
        if(IsKeyPressed(KEY_LEFT) && Player->Direction.x == 0)
            Player->Direction = Vector2{-1, 0};
        if(IsKeyPressed(KEY_RIGHT) && Player->Direction.x == 0)
            Player->Direction = Vector2{1, 0};
        

        ClearBackground(Green); // Clear the screen with green color
        Berry->Draw(); // Draw the food
        Player->Draw(); // Draw the snake

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
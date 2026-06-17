#include <iostream>
#include <raylib.h>
#include <vector>
#include <deque>
#include <raymath.h>
using namespace std;

Color pink = {255, 192, 203, 255};   
Color darkPink = {180, 60, 100, 255};

float cellSize = 20;
float cellCount = 20;

float offset = 75;

double lastUpdatetime = 0; 

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
for (unsigned int i = 0; i < deque.size(); i++)
            {
              if(Vector2Equals(deque[i], element))
              {
                return true;
              }
            }
           return false;  
}

bool eventTriggered(double interval)
{
double currentTime = GetTime();
if(currentTime - lastUpdatetime >= interval)
      {
     lastUpdatetime = currentTime;
    return true;
      }
    return false;
}

class Snake
{
public:
deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
Vector2 direction = {1,0};
bool addSegment = false;
 void Draw()
 {
        for(int i = 0; i < (int)body.size(); i++)
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5, 6, darkPink);

            } 
        }

void Update()
  {
    body.push_front(Vector2Add(body[0], direction));
    if(addSegment == true)
    {
    addSegment = false;
    }
    else
    {
    body.pop_back();
    }
  }
  void Reset()
  {
    body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    direction = {1, 0};
  }
};

class Food 
{
public:
Vector2 position;
Texture2D texture; 

void LoadRandomFood(deque<Vector2> snakeBody)
{
    vector<string> foodImages = {
        "graphics/flower.png",
        "graphics/flower2.png",
        "graphics/flower3.png",
        "graphics/flower4.png",
        "graphics/flower5.png",
        "graphics/flower6.png",
        "graphics/flower7.png",
        "graphics/flower8.png",
        "graphics/flower9.png",


    };
    int randomIndex = GetRandomValue(0, foodImages.size() - 1);
    Image image = LoadImage(foodImages[randomIndex].c_str());
    ImageResize(&image, cellSize + 6, cellSize + 6);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = GenerateRandomPos(snakeBody);
}

Food (deque<Vector2> snakeBody)
{
    LoadRandomFood(snakeBody);
}

void Reset(deque<Vector2> snakeBody)
{
    UnloadTexture(texture);
    LoadRandomFood(snakeBody);
}

//destructor
~Food()
{
UnloadTexture(texture);

}

void Draw()
{
DrawTexture(texture,offset + position.x * cellSize,offset + position.y * cellSize, darkPink);

}

Vector2 GenerateRandomCell()
{
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
}

Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
{ 
Vector2 position = GenerateRandomCell();
while(ElementInDeque(position, snakeBody))
    {
     position = GenerateRandomCell();
    }
    return position;
}
};

class Game
{
public:
Snake snake = Snake();
Food food = Food(snake.body);
Font myFont;
bool gameOver = false;
bool showScore = false;
int score = 0;
bool running = true;

Game()  //constructor
{
    myFont = LoadFont("fonts/atwriter.ttf");
}

~Game()  //destructor
{
    UnloadFont(myFont);
}

void DrawCenteredText(const char* text, float y, float fontSize)
 {
    Vector2 textSize = MeasureTextEx(myFont, text, fontSize, 2);
    DrawTextEx(myFont, text, Vector2{offset + cellSize*cellCount/2 - textSize.x/2, y}, fontSize, 2, darkPink);
 }


void Draw()
{
    if(gameOver)
    {
        if(!showScore)
        {
            DrawCenteredText("ohh nooo, tryyy againn?", offset + cellSize*cellCount/2 - 15, 30);
            DrawCenteredText("press any key to continue", offset + cellSize*cellCount/2 + 40, 20);
        }
        if(showScore)
        {
            DrawCenteredText(TextFormat("score: %i", score), offset + cellSize*cellCount/2 - 60, 25);
            DrawCenteredText("press R to restart", offset + cellSize*cellCount/2 - 15, 20);
        }
        return;
    }
    snake.Draw();
    food.Draw();
    DrawTextEx(myFont, TextFormat("score: %i", score), Vector2{offset - 5, offset + cellSize*cellCount + 15}, 25, 2, darkPink);
}

void Update()
{
    if(running)
  {
    snake.Update();
    CheckCollisionsWithFood();
    CheckCollisionWithEdges();
    CheckCollisionWithTail();
  }
}

void CheckCollisionsWithFood()
 {
 if (Vector2Equals(snake.body[0], food.position))
         {
            food.Reset(snake.body);
            snake.addSegment = true;
            score++;
         }
 }
void CheckCollisionWithEdges()
 {
   if(snake.body[0].x == cellCount || snake.body[0].x == -1)
   {
    GameOver();
   }
   if(snake.body[0].y == cellCount || snake.body[0].y == -1)
   {
    GameOver();
   }
 }

 void CheckCollisionWithTail()
  {
   deque <Vector2> headlessBody =snake.body;
   headlessBody.pop_front();
   if(ElementInDeque(snake.body[0], headlessBody))
   {
       GameOver();
   }
  }

 void GameOver()
  {
   gameOver = true;
   snake.Reset();
   food.position = food.GenerateRandomPos(snake.body);
   running = false;
  }
void HandleInput()
{
    if(gameOver && !showScore && GetKeyPressed())
    {
        showScore = true;
    }
}
}; 

int main(){
InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "silly snake");

SetWindowPosition(50, 50);
SetTargetFPS(60);

Game game = Game(); 


while(!WindowShouldClose())
{ 
if(eventTriggered(0.2))
{
game.Update();
}
game.HandleInput();

if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
{
    game.snake.direction = {0, -1};
}
if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
{
    game.snake.direction = {0, 1};
}
if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
{
    game.snake.direction = {-1, 0};
}
if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
{
    game.snake.direction = {1, 0};
}

if(IsKeyPressed(KEY_R) && game.showScore)
{
    game.snake.Reset();
    game.food.Reset(game.snake.body);
    game.gameOver = false;
    game.showScore = false;
    game.score = 0;
    game.running = true;
}

BeginDrawing();
//drawing
ClearBackground(pink);
DrawRectangleLinesEx(Rectangle{offset - 5, offset - 5, cellSize*cellCount + 10, cellSize*cellCount + 10}, 5, darkPink);

Vector2 titleSize = MeasureTextEx(game.myFont, "silly snake", 30, 2);
DrawTextEx(game.myFont, "silly snake", Vector2{offset + cellSize*cellCount/2 - titleSize.x/2, offset/2 - titleSize.y/2}, 30, 2, darkPink);

game.Draw();
EndDrawing();
}

CloseWindow();
return 0;
}
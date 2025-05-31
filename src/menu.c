#include "menu.h"
#include <raylib.h>

static MenuButton Buttons[] = {
    {{64.0f, 80.0f}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "Play", 60, BLACK},
    {{64.0f, 200.0f}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "High Scores", 60, BLACK},
    {{64.0f, 320.0f}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "Quit", 60, BLACK}};

void drawButton(struct MenuButton b) {
    Rectangle button = {b.pos.x, b.pos.y, b.size.x , b.size.y};
    DrawRectangleRounded(button, b.round.x, b.round.y, b.buttonColor);
    int textx = b.pos.x + 20;
    int texty = b.pos.y + 10;
    DrawText(b.text, textx, texty, b.fontSize, b.fontColor);
}

void menu() {
  BeginDrawing();
  ClearBackground(SKYBLUE);
  //Limit Menu Fps
  SetTargetFPS(60);
  DrawFPS(0,0);
  // Menu objects for a 640x480
  // Play, High Score, Quit buttons
  int i;
  for (i = 0; i < 3; i++) {
      drawButton(Buttons[i]);
  }
  EndDrawing();
}


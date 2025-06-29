#include "menu.h"
#include "main.h"
#include <raylib.h>

Rectangle drawButton(struct MenuButton b) {
    Rectangle button = {b.pos.x, b.pos.y, b.size.x , b.size.y};
    DrawRectangleRounded(button, b.round.x, b.round.y, b.buttonColor);
    int textx = b.pos.x + 20;
    int texty = b.pos.y + 10;
    DrawText(b.text, textx, texty, b.fontSize, b.fontColor);
    return button;
}

void CreateButtons(MenuButton* buttons) {
    int width, height;
    width = GetRenderWidth();
    height = GetRenderHeight();
    //printf("%i, %i, %i", IsWindowFullscreen(), width, height);
    buttons[0] = (MenuButton){{width / 2 - 266, height * 0.1666}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "Play", 60, BLACK};
    buttons[1] = (MenuButton){{width / 2 - 266, height * 0.4166}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "High Scores", 60, BLACK};
    buttons[2] = (MenuButton){{width / 2 - 266, height * 0.6666}, {512.0f, 80.0f}, {8.0f, 10}, WHITE, "Quit", 60, BLACK};
}

int menu(Vector2 mPos, int isMouseDown) {
    message m = none;
    BeginDrawing();
    ClearBackground(SKYBLUE);
    //Limit Menu Fps
    SetTargetFPS(60);
    DrawFPS(0,0);
    // Play, High Score, Quit buttons
    int i;
    MenuButton menuButtons[3];
    CreateButtons(menuButtons);
    Rectangle buttons[3];
    for (i = 0; i < 3; i++) {
        buttons[i] = drawButton(menuButtons[i]);
    }
    DrawCircle(mPos.x, mPos.y, 10.0, RED);
    //Check for mouse click on button
    if (isMouseDown) {
        if (CheckCollisionPointRec(mPos, buttons[0])) {
            m = menuPlay;
        } else if (CheckCollisionPointRec(mPos, buttons[1])) {
            m = menuHighScore;
        } else if (CheckCollisionPointRec(mPos, buttons[2])) {
            m = menuQuit;
        }
    }
    EndDrawing();
    return m;
}


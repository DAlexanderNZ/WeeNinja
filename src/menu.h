#include <raylib.h>
#ifndef WEENINJA_MENU_H
#define WEENINJA_MENU_H
int menu ();
typedef struct MenuButton{
    Vector2 pos;
    Vector2 size;
    Vector2 round;
    Color buttonColor;
    const char* text;
    int fontSize;
    Color fontColor;
}MenuButton;
#endif


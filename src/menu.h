#include <raylib.h>
#ifndef WEENINJIA_MENU_H
#define WEENINJIA_MENU_H
void menu ();
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


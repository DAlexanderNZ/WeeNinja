#include "main.h"
#ifndef WEENINJA_MESSAGE_H
#define WEENINJA_MESSAGE_H
typedef enum message {
    none = 0,
    menuPlay = 1,
    menuHighScore = 2,
    menuQuit = 3
} message;

int handleMsg(enum message msg);
#endif

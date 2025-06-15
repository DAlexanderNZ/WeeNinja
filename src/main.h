#ifndef WEENINJA_MAIN_H
#define WEENINJA_MAIN_H
#include "application.h"
#include "fruit.h"
#include "input.h"
#include "menu.h"
#include "message.h"
#include "model.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum message {
  none = 0,
  menuPlay = 1,
  menuHighScore = 2,
  menuQuit = 3
} message;
#endif

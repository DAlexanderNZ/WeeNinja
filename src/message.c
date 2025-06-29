#include "message.h"


int handleMsg(enum message msg) {
  switch (msg) {
  case none:
    break;
  case menuPlay:
    // Call 3d game
    break;
  case menuHighScore:
    // highscores
    break;
  case menuQuit:
    return 1;
  }
  return 0;
}

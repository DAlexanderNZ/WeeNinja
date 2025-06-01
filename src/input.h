#ifndef INPUT_H
#define INPUT_H
#include "SF1eFilter.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void poll_position(float pos[]);
void poll_velocity(float pos[]);
int init_input();
void reset_filter();
void free_input();

#endif

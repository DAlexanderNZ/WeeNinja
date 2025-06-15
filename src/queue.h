#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct button_event_t {
  uint16_t button;
  Vector2 position;
} button_event_t;

#define QUEUE_LENGTH 10

void drain_queue(button_event_t *poll_queue, int *queue_length);
void push_queue(button_event_t button_event);
void init_queue();

#endif // QUEUE_H_

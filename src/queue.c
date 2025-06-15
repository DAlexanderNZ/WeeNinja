#include "queue.h"

button_event_t queue_buffer[QUEUE_LENGTH] = {0};
int queue_start = 0;
int queue_end = 0;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

void push_queue(button_event_t button_event) {
  pthread_mutex_lock(&queue_lock);
  queue_buffer[queue_end] = button_event;
  queue_end = (queue_end + 1) % QUEUE_LENGTH;
  if (queue_end == queue_start) {
    queue_start = (queue_start + 1) % QUEUE_LENGTH;
  }
  pthread_mutex_unlock(&queue_lock);
}

void drain_queue(button_event_t *poll_queue, int *queue_length) {
  pthread_mutex_lock(&queue_lock);
  int len = (queue_end - queue_start + QUEUE_LENGTH) % QUEUE_LENGTH;
  *queue_length = len;
  for (int i = 0; i < len; i++) {
    memcpy(poll_queue + i, queue_buffer + (queue_start + i) % QUEUE_LENGTH,
           sizeof(button_event_t));
  }
  queue_start = 0;
  queue_end = 0;
  pthread_mutex_unlock(&queue_lock);
}

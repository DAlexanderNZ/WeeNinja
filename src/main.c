#include "main.h"
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

#define FOV_X 45.0
#define FOV_Y 45.0
#define FLICK_THRESHOLD 3.0

Vector2 screen = {320, 240};
Vector2 screen_extents = {320, 240};
Vector2 targetScreen = {320, 240};

int flicking = false;
Vector2 flick_direction = {0.0, 0.0};
Vector2 shot_start = {0.0, 0.0};
int shooting = 0;
#define ALPHA 0.6

void print_buttons(uint16_t buttons) {
    switch (buttons) {
    case CWIID_BTN_B:
        shot_start = targetScreen;
        shooting = 1;
    }
}

void DrawSlicer(Camera camera, Vector2 at) {
    Ray ray = GetScreenToWorldRay(at, camera);
    // Project the ray direction onto the z = 0 plane from the ray position
    float t = -ray.position.z / ray.direction.z;
    Vector3 OnZ0Plane =
        Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    DrawSphere(OnZ0Plane, 0.1, (Color){0, 0, 255, 85});
}

Vector2 Lerp2(Vector2 from, Vector2 to, float alpha) {
    return (Vector2){from.x + alpha * (to.x - from.x),
                     from.y + alpha * (to.y - from.y)};
}

int main(int argc, char **argv) {

    int use_wiimote = argc == 2 && !strncmp(argv[1], "YES", 3);
    if (use_wiimote) {
        init_input();
    }

    while (1) {
        float position[3] = {0.0, 0.0, 0.0};
        poll_position(position);
        printf("Est position: (%f, %f, %f)", position[0], position[1],
               position[2]);
    }

    if (use_wiimote) {
        free_input();
    }
    return 0;
}

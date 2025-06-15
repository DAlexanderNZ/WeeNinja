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

int shooting = 0;
int screen_width;
int screen_height;
Vector2 shot_start = (Vector2){.x = 0.0, .y = 0.0};
Vector2 screen = (Vector2){.x = 0.0, .y = 0.0};
float cutoffSlope = 1.0f;
float minCutoffFrequency = 0.1f;

void print_buttons(uint16_t buttons) {
    switch (buttons) {
    case CWIID_BTN_A:
        cutoffSlope += 0.1;
    case CWIID_BTN_B:
        cutoffSlope -= 0.1;
    case CWIID_BTN_UP:
        minCutoffFrequency += 0.1;
    case CWIID_BTN_DOWN:
        minCutoffFrequency -= 0.1;
    }
    configure_filter(minCutoffFrequency, cutoffSlope);
}

void DrawSlicer(Camera camera, Vector2 at) {
    Ray ray = GetScreenToWorldRay(at, camera);
    // Project the ray direction onto the z = 0 plane from the ray position
    float t = -ray.position.z / ray.direction.z;
    Vector3 OnZ0Plane =
        Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    DrawSphere(OnZ0Plane, 0.1, (Color){0, 0, 255, 85});
}

int main(int argc, char **argv) {
    int use_wiimote = argc == 2 && !strncmp(argv[1], "YES", 3);
    cwiid_wiimote_t* wiimote = NULL;
    if (use_wiimote) {
        wiimote = init_input(&print_buttons);
        if (!wiimote) {
            return -1;
        }
    }

    InitWindow(640, 480, "WeeNinja");

    ToggleFullscreen();
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();
    // Main Menu
    int message = 0;
    while(message != 1) {
        message = menu();
    }
    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0f;

    GameState state;
    wn_state_init(&state);

    int shouldQuit = 0;

    float fruit_timer = 0.0f;
    while (!WindowShouldClose() && !shouldQuit) {
        /* PollInputEvents(); */
        if (use_wiimote) {
            float position[2] = {0.0, 0.0};
            poll_position(position);
            screen = (Vector2){.x = screen_width * (0.5 + position[0]),
                                       .y = screen_height * (0.5 + position[1])};
        } else {
            screen = GetMousePosition();
            shot_start = GetMousePosition();
            shooting = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        }

        if (shooting) {
            Ray ray = GetScreenToWorldRay(shot_start, camera);
            wn_fruit_pick(&state, ray);
            shooting = false;
        }

        BeginDrawing();
        BeginMode3D(camera);

        ClearBackground(WHITE);

        DrawSlicer(camera, screen);

        fruit_timer += GetFrameTime();
        if (fruit_timer > 0.25f) {
            fruit_timer = 0.0f;

            int type;
            switch (rand() % 4) {
                case 0:
                    type = FRUIT_APPLE;
                    break;
                case 1:
                    type = FRUIT_KIWIFRUIT;
                    break;
                case 2:
                    type = FRUIT_ORANGE;
                    break;
                default:
                    type = FRUIT_PINEAPPLE;
                    break;
            }

            wn_spawnfruit(&state, type, FRUIT_CHIRALITY_LEFT);
        }

        wn_update(&state);
        wn_drawfruit(&state);

        EndMode3D();

        /* shouldQuit = handleMsg(menu()); */

        /* menu(); */
        EndDrawing();
    }

    if (use_wiimote) {
        free_input();
    }

    CloseWindow();
    return 0;
}

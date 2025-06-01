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

void print_buttons(uint16_t buttons) {
    switch (buttons) {
    case CWIID_BTN_B:
        float position[3] = {0.0, 0.0, 0.0};
        float screen[2] = {0.0, 0.0};
        poll_position(position);
        position_to_screen_space(position, screen_width, screen_height, screen);
        shot_start.x = screen[0];
        shot_start.y = screen[1];
        shooting = true;
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

int main(int argc, char **argv) {
    cwiid_wiimote_t *wiimote;
    int use_wiimote = argc == 2 && !strncmp(argv[1], "YES", 3);
    if (use_wiimote) {
        int err = init_input();
        if (err) {
            return err;
        }
    }

    InitWindow(640, 480, "WeeNinja");

    ToggleFullscreen();
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();
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
        PollInputEvents();

        float position[3] = {0.0, 0.0, 0.0};
        float screen_pos[2] = {0.0, 0.0};
        position_to_screen_space(position, screen_width, screen_height,
                                 screen_pos);
        Vector2 screen = (Vector2){.x = screen_pos[0], .y = screen_pos[1]};
        poll_position(position);

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
        if (fruit_timer > 1.0f) {
            fruit_timer = 0.0f;
            wn_spawnfruit(&state, rand() % _N_FRUIT);
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

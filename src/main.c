#include "main.h"
#include "application.h"
#include "fruit.h"
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
#define CY 384.0
#define CX 512.0

Vector2 screen = {320, 240};
Vector2 screen_extents = {320, 240};
Vector2 targetScreen = {320, 240};
struct acc_cal wm_cal;
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

void ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                      Vector2 *output_screen_coords) {
    float mid_y = ((float)(py1 + py2)) / 2.0;
    float mid_x = ((float)(px1 + px2)) / 2.0;

    float offset_y = -(CY - mid_y) / screen_extents.y;
    float offset_x = (CX - mid_x) / screen_extents.x;

    output_screen_coords->x =
        screen_extents.x / 2 + offset_x * screen_extents.x;
    output_screen_coords->y =
        screen_extents.y / 2 + offset_y * screen_extents.y;
}

void handle_accel_event(struct cwiid_acc_mesg msg) {}

void track_ir_event(struct cwiid_ir_src srcs[]) {
    uint16_t px1 = 0;
    uint16_t px2 = 0;
    uint16_t py1 = 0;
    uint16_t py2 = 0;
    int blob_count = 0;

    for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
        if (srcs[i].valid) {
            if (blob_count == 0) {
                px1 = srcs[i].pos[CWIID_X];
                py1 = srcs[i].pos[CWIID_Y];
                blob_count++;
            } else if (blob_count == 1) {
                px2 = srcs[i].pos[CWIID_X];
                py2 = srcs[i].pos[CWIID_Y];
                blob_count++;
            } else {
                break;
            }
        }
    }
    if (blob_count == 2) {
        ir_to_real_space(px1, py1, px2, py2, &targetScreen);
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

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
    for (int i = 0; i < mesg_count; i++) {
        union cwiid_mesg msg = mesg_array[i];
        switch (msg.type) {
        case CWIID_MESG_BTN:
            print_buttons(msg.btn_mesg.buttons);
            break;
        case CWIID_MESG_IR:
            track_ir_event(msg.ir_mesg.src);
            break;
        case CWIID_MESG_ACC:
            handle_accel_event(msg.acc_mesg);
            break;
        default:
            break;
        }
    }
}

Vector2 Lerp2(Vector2 from, Vector2 to, float alpha) {
    return (Vector2){from.x + alpha * (to.x - from.x),
                     from.y + alpha * (to.y - from.y)};
}

int main(int argc, char **argv) {
    cwiid_wiimote_t *wiimote;
    int use_wiimote = argc == 2 && !strncmp(argv[1], "YES", 3);
    printf("Using wiimote: %d\n", use_wiimote);
    if (use_wiimote) {
        bdaddr_t bdaddr = *BDADDR_ANY;

        wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);
        if (!wiimote) {
            fprintf(stderr, "Unable to connect\n");
            return 1; // Exit on failure to connect
        }

        if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
            fprintf(stderr, "Unable to set callback\n");
            cwiid_close(wiimote);
            return 1;
        }

        if (cwiid_set_rpt_mode(wiimote,
                               CWIID_RPT_BTN | CWIID_RPT_IR | CWIID_RPT_ACC)) {
            fprintf(stderr, "Unable to set report mode\n");
            cwiid_close(wiimote);
            return 1;
        }
        cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal);
    }

    InitWindow(640, 480, "WeeNinja");
    // Main Menu
    int message = 0;
    while(message != 1) {
        message = menu();
    }
    /* ToggleFullscreen(); */
    screen_extents.x = (float)GetScreenWidth();
    screen_extents.y = (float)GetScreenHeight();
    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0f;

    GameState state;
    wn_state_init(&state);

    int shouldQuit = 0;
    screen = targetScreen;

    float fruit_timer = 0.0f;
    while (!WindowShouldClose() && !shouldQuit) {
        /* PollInputEvents(); */

        if (use_wiimote) {
            screen = Lerp2(screen, targetScreen, 0.7);
        } else {
            screen = GetMousePosition();
            shot_start = GetMousePosition();
            shooting = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        }

        if (shooting) {
            Ray ray = GetScreenToWorldRay(shot_start, camera);
            wn_fruit_pick(&state, ray);
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

            wn_spawnfruit(&state, type);
        }

        wn_update(&state);
        wn_drawfruit(&state);

        EndMode3D();

        /* shouldQuit = handleMsg(menu()); */

        /* menu(); */
        EndDrawing();
    }

    if (use_wiimote) {
        cwiid_close(wiimote);
    }

    CloseWindow();
    return 0;
}

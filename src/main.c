#include "menu.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <raylib.h>
#include <raymath.h>
#include <unistd.h>

#define FOV_X 45.0
#define FOV_Y 45.0
#define CY 384.0
#define CX 512.0

Vector2 screen = {320, 240};
Vector2 targetScreen = {320, 240};

#define ALPHA 0.6

void print_buttons(uint16_t buttons) {}

void ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                      Vector2 *output_screen_coords) {
    float mid_y = ((float)(py1 + py2)) / 2.0;
    float mid_x = ((float)(px1 + px2)) / 2.0;

    float offset_y = -(CY - mid_y) / 768.0f;
    float offset_x = (CX - mid_x) / 1024.0f;

    output_screen_coords->x = 320.0f + offset_x * 640.0f;
    output_screen_coords->y = 240.0f + offset_y * 480.0f;
}

void print_ir_event(struct cwiid_ir_src srcs[]) {
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

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
    for (int i = 0; i < mesg_count; i++) {
        union cwiid_mesg msg = mesg_array[i];
        switch (msg.type) {
        case CWIID_MESG_BTN:
            print_buttons(msg.btn_mesg.buttons);
            break;
        case CWIID_MESG_IR:
            print_ir_event(msg.ir_mesg.src);
            break;
        default:
            break;
        }
    }

    float t = -ray.position.z / ray.direction.z;
    Vector3 OnZ0Plane =
        Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    DrawSphere(OnZ0Plane, 0.1, BLUE);
}

Vector2 Lerp(Vector2 from, Vector2 to, float alpha) {
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

        if (cwiid_set_rpt_mode(wiimote, CWIID_RPT_BTN | CWIID_RPT_IR)) {
            fprintf(stderr, "Unable to set report mode\n");
            cwiid_close(wiimote);
            return 1;
        }
    }

    InitWindow(640, 480, "WeeNinja");

    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0f;

    Model m = LoadModel("resource/goodart/apple.obj");
    Texture2D tex = LoadTexture("resource/goodart/apple.png");
    m.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;

    Matrix xform = MatrixIdentity();

    SetTargetFPS(60);

    screen = targetScreen;

    while (!WindowShouldClose()) {
        PollInputEvents();

        screen = Lerp(screen, targetScreen, 0.5);

        xform = MatrixTranslate(0.0f, 0.0f, -7.0f);

        BeginDrawing();
        BeginMode3D(camera);

        ClearBackground(WHITE);

        DrawSlicer(camera, screen);
        EndMode3D();

        /* menu(); */
        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadModel(m);

    if (use_wiimote) {
        cwiid_close(wiimote);
    }

    CloseWindow();
    return 0;
}

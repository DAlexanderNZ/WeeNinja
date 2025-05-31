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

float screen_x = 0;
float screen_y = 0;

void print_buttons(uint16_t buttons) {}

void ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                      float *screen_x, float *screen_y) {
    float mid_y = ((float)(py1 + py2)) / 2.0;
    float mid_x = ((float)(px1 + px2)) / 2.0;
    float offset_y = (CY - mid_y) / 768;
    float offset_x = (CX - mid_x) / 1024;
    *screen_y = offset_y;
    *screen_x = offset_x;
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
        ir_to_real_space(px1, py1, px2, py2, &screen_x, &screen_y);
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
        if (mesg_array[i].type == CWIID_MESG_BTN) {
            print_buttons(mesg_array[i].btn_mesg.buttons);
        }
    }
}

#define INV_SQRT2 0.414213562373
void DrawSlicer(Camera camera) {
    Vector2 screen = {320 + screen_x * 640, 240 + screen_y * 480};
    Ray ray = GetScreenToWorldRay(screen, camera);
    // project the ray direction onto the z = 0 plane from the ray position
    float t = -ray.position.z / ray.direction.z;
    Vector3 OnZ0Plane =
        Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    DrawSphere(OnZ0Plane, 0.1, BLUE);
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
        }

        if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
            fprintf(stderr, "Unable to set callback\n");
            cwiid_close(wiimote);
        }

        if (cwiid_set_rpt_mode(wiimote, CWIID_RPT_BTN | CWIID_RPT_IR)) {
            fprintf(stderr, "Unable to set report mode\n");
            cwiid_close(wiimote);
        }
    }

    InitWindow(640, 480, "WeeNinja");

    Camera3D camera = {0};
    camera.position.x = 0.0f;
    camera.position.y = 0.0f;
    camera.position.z = 1.0f;

    camera.target.x = 0.0f;
    camera.target.y = 0.0f;
    camera.target.z = -1.0f;

    camera.up.x = 0.0f;
    camera.up.y = 1.0f;
    camera.up.z = 0.0f;

    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0f;

    Model m = LoadModel("resource/goodart/apple.obj");
    Texture2D tex = LoadTexture("resource/goodart/apple.png");
    m.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;

    Matrix xform = MatrixIdentity();

    float rot = 0.0f;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        PollInputEvents();
        xform = MatrixTranslate(0.0f, 0.0f, -7.0f);

        BeginDrawing();
        BeginMode3D(camera);

        ClearBackground(WHITE);

        /* DrawMesh(m.meshes[0], m.materials[0], xform); */
        DrawSlicer(camera);
        EndMode3D();

        /* menu(); */
        EndDrawing();

        SwapScreenBuffer();
    }
    if (use_wiimote) {
        cwiid_close(wiimote);
    }

    return 0;
}

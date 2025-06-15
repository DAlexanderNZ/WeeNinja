#include "main.h"

typedef enum GAME_SCREEN { MAIN_MENU, GAME } game_screen_t;

int screen_width;
int screen_height;
float cutoffSlope = 1.0f;
float minCutoffFrequency = 0.1f;

Vector2 get_wiimote_screen_position() {
    float position[2] = {0};
    poll_position(position);
    Vector2 screen = {.x = screen_width * (position[0] + 0.5),
                      .y = screen_height * (position[1] + 0.5)};
    return screen;
}
void handle_button_event(uint16_t buttons) {
    button_event_t event = {.button = buttons,
                            .position = get_wiimote_screen_position()};
    push_queue(event);
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
    cwiid_wiimote_t *wiimote = NULL;
    if (use_wiimote) {
        wiimote = init_input(&handle_button_event);
        if (!wiimote) {
            return -1;
        }
    }

    InitWindow(640, 480, "WeeNinja");

    ToggleFullscreen();
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();
    // Main Menu
    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0f;

    GameState state;
    wn_state_init(&state);

    int shouldQuit = 0;
    int shooting = 0;
    Vector2 shot_start = (Vector2){.x = 0.0, .y = 0.0};
    Vector2 screen = (Vector2){.x = 0.0, .y = 0.0};

    float fruit_timer = 0.0f;
    game_screen_t game_screen = MAIN_MENU;
    while (!WindowShouldClose() && !shouldQuit) {
        if (use_wiimote) {
            screen = get_wiimote_screen_position();
            button_event_t events[QUEUE_LENGTH] = {0};
            int queue_length = 0;
            drain_queue(events, &queue_length);
            for (int i = 0; i < queue_length; i++) {
                switch (events[i].button) {
                case CWIID_BTN_A:
                    shooting = 1;
                    shot_start = events[i].position;
                    break;
                }
            }
        } else {
            screen = GetMousePosition();
            shot_start = GetMousePosition();
            shooting = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        }

        switch (game_screen) {
        case MAIN_MENU:
            int menu_msg = menu(screen, shooting);
            if (menu_msg == menuPlay) {
                game_screen = GAME;
            }
            break;
        case GAME:

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
            EndDrawing();
            break;
        }
    }

    if (use_wiimote) {
        free_input();
    }

    CloseWindow();
    return 0;
}

#include "main.h"
#include <raylib.h>

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
    InitAudioDevice();
    float fruit_timer = 0.0f;
    game_screen_t game_screen = MAIN_MENU;
    enum MusicName current_playing_track = _N_MUSIC;
    Music current_track = {0};
    srand(time(NULL));
    while (!WindowShouldClose() && !shouldQuit) {
        if (current_playing_track != _N_MUSIC) {
            UpdateMusicStream(current_track);
        }

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
            if (IsKeyPressed(KEY_F11)) {
                ToggleFullscreen();
            }
        }

        switch (game_screen) {
        case MAIN_MENU:
            if (current_playing_track == _N_MUSIC) {
                current_track = get_music(MUSIC_MENU);
                current_playing_track = MUSIC_MENU;
            }

            int menu_msg = menu(screen, shooting);
            if (menu_msg == menuPlay) {
                StopMusicStream(current_track);
                current_playing_track = _N_MUSIC;
                game_screen = GAME;
            } else if (menu_msg == menuQuit) {
                CloseWindow();
                shouldQuit = true;
            }

            float music_length = GetMusicTimeLength(current_track);
            float played_music = GetMusicTimePlayed(current_track);
            if (music_length - played_music < 0.1f) {
                SeekMusicStream(current_track, 0.0f);
            }
            if (!IsMusicStreamPlaying(current_track)) {
                SetMusicVolume(current_track, 1.0);
                PlayMusicStream(current_track);
            }
            break;

        case GAME: {
            if (shooting) {
                Ray ray = GetScreenToWorldRay(shot_start, camera);
                int score = wn_fruit_pick(&state, ray);

                if (score < 0) {
                    wn_state_init(&state);
                    Sound boom = get_sound(AUDIO_BOOM_1);
                    int should_boom_variant = rand() % 10;
                    printf("Should boom variant: %d\n", should_boom_variant);
                    if (should_boom_variant == 1) {

                        boom = get_sound(AUDIO_BOOM_2);
                    }

                    if (IsSoundPlaying(boom)) {
                        StopSound(boom);
                    }
                    PlaySound(boom);
                } else if (score > 0) {
                    Sound slice =
                        get_sound(rand() % (AUDIO_SLICE_4 - AUDIO_SLICE_1 + 1) +
                                  AUDIO_SLICE_1);
                    if (IsSoundPlaying(slice)) {
                        StopSound(slice);
                    }
                    PlaySound(slice);
                    state.score += score;
                }

                shooting = false;
            }

            BeginDrawing();
            BeginMode3D(camera);

            fruit_timer += GetFrameTime();
            if (fruit_timer > 0.25f) {
                fruit_timer = 0.0f;

                int type;
                switch (rand() % 5) {
                case 0:
                    type = FRUIT_APPLE;
                    break;
                case 1:
                    type = FRUIT_KIWIFRUIT;
                    break;
                case 2:
                    type = FRUIT_ORANGE;
                    break;
                case 3:
                    type = FRUIT_BOMB;
                    break;
                case 4:
                    type = FRUIT_PINEAPPLE;
                    break;
                }

                wn_spawnfruit(&state, type, FRUIT_CHIRALITY_LEFT);
            }

            ClearBackground(WHITE);
            wn_update(&state);
            wn_drawfruit(&state);

            DrawSlicer(camera, screen);
            EndMode3D();

            char scoreText[256] = {0};
            snprintf(scoreText, sizeof scoreText, "Score: %d", state.score);
            DrawText(scoreText, 0, 0, 16, RED);

            EndDrawing();
            break;
        }
        }
        // Set the Window size to the render size so the mouse pos lines up for
        // the slicer
        SetWindowSize(GetRenderWidth(), GetRenderHeight());
    }

    if (use_wiimote) {
        free_input();
    }
    unload_music();
    unload_audio();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

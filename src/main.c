#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#include "menu.h"

int main(int argc, char **argv)
{
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
    
    while (!WindowShouldClose()) {
        PollInputEvents();

        rot += 0.5f * GetFrameTime();
        xform = MatrixTranslate(0.0f, 0.0f, -7.0f);
        xform = MatrixMultiply(MatrixRotateY(rot), xform);

        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(WHITE);

        DrawMesh(m.meshes[0], m.materials[0], xform);

        EndMode3D();

        /* menu(); */
        EndDrawing();

        SwapScreenBuffer();
    }
    
    return 0;
}

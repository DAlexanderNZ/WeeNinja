#include <stdio.h>
#include <raylib.h>

int main(int argc, char **argv) {
    InitWindow(640, 480, "WeeNinja");

    while (!WindowShouldClose()) {
        PollInputEvents();
        SwapScreenBuffer();
    }
    
    return 0;
}

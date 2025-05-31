#include "application.h"
#include "fruit.h"
#include "model.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#define WEENINJA_GRAVITY 8.0f

void wn_state_init(GameState *state) {
    state->n_fruit = 0;

    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
        state->fruit[i].alive = false;
    }
}

void wn_spawnfruit(GameState *state, int type) {
    float xvel = (2.0f * ((float)rand() / (float)RAND_MAX) - 0.5f);
    float yvel = ((float)rand() / (float)RAND_MAX) + 0.5f;

    float xpos = 2.0f * ((float)rand() / (float)RAND_MAX) - 0.5f;

    Fruit f;
    f.position.x = 8.0f * xpos;
    f.position.y = -10.0f;
    f.velocity.x = xvel;
    f.velocity.y = yvel * 14.0f;
    f.theta = 0.0f;
    f.omega = 0.3f;
    f.type = type;
    f.alive = true;

    state->fruit[state->n_fruit] = f;
    state->n_fruit++;
}

void wn_update(GameState *state) {
    for (int i = 0; i < state->n_fruit; i++) {
        Fruit *f = &state->fruit[i];
        if (!f->alive) {
            continue;
        }

        f->position.x += f->velocity.x * GetFrameTime();
        f->position.y += f->velocity.y * GetFrameTime();
        f->velocity.y -= WEENINJA_GRAVITY * GetFrameTime();
        f->theta += f->omega * GetFrameTime();

        if (f->position.y < -10.0f) {
            f->alive = false;
        }
    }
}

void wn_drawfruit(const GameState *state) {
    for (int i = 0; i < state->n_fruit; i++) {
        const Fruit *f = &state->fruit[i];
        if (!state->fruit[i].alive) {
            continue;
        }

        Model m = get_fruit_model(f->type);
        Matrix xform = MatrixIdentity();
        xform = MatrixMultiply(xform, MatrixRotateZ(f->theta));
        xform = MatrixMultiply(
            xform, MatrixTranslate(f->position.x, f->position.y, -20.0));

        DrawMesh(m.meshes[0], m.materials[0], xform);
    }
}

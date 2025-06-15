#include "application.h"
#include "fruit.h"
#include "model.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#define WEENINJA_GRAVITY 10.0f

static void provision_fruit(GameState *state, Fruit **fruit) {
    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
        if (!state->fruit[i].alive) {
            *fruit = &state->fruit[i];
            (*fruit)->alive = true;
            return;
        }
    }

    fprintf(stderr, "NO MORE FRUIT\n");
    exit(-1);
}

void wn_state_init(GameState *state) {
    state->n_fruit = 0;
    state->score = 0;

    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
        state->fruit[i].alive = false;
    }
}

void wn_spawnfruit(GameState *state, int type, int chirality) {
    Fruit *f;
    provision_fruit(state, &f);
    float xvel = 2.0f * (((float)rand() / (float)RAND_MAX) - 0.5f);
    float yvel = ((float)rand() / (float)RAND_MAX) + 0.5f;

    float xpos = 2.0f * (((float)rand() / (float)RAND_MAX) - 0.5f);

    f->position.x = 8.0f * xpos;
    f->position.y = -10.0f;
    f->velocity.x = xvel;
    f->velocity.y = yvel * 14.0f;
    f->theta = 0.0f;
    f->omega = 0.3f;
    f->type = type;
    f->alive = true;
    f->chirality = chirality;
}

void wn_killfruit(GameState *state, Fruit *f) { f->alive = 0; }

void wn_update(GameState *state) {
    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
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
    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
        const Fruit *f = &state->fruit[i];
        if (!state->fruit[i].alive) {
            continue;
        }

        Model m = get_fruit_model(f->type);
        Matrix xform = MatrixIdentity();

        xform = MatrixMultiply(xform, MatrixRotateZ(f->theta));
        if (f->chirality == FRUIT_CHIRALITY_LEFT) {
            /* nop */
        } else if (f->chirality == FRUIT_CHIRALITY_RIGHT) {
            xform = MatrixMultiply(xform, MatrixScale(-1, 1, 1));
        }

        xform = MatrixMultiply(
            xform, MatrixTranslate(f->position.x, f->position.y, -20.0));

        for (int j = 0; j < m.meshCount; j++) {
            int material_index = m.meshMaterial[j];
            DrawMesh(m.meshes[j], m.materials[material_index], xform);
        }
    }
}

int wn_fruit_pick(GameState *state, Ray ray) {
    const float z_plane = -20.0f;
    int score = 0;

    for (int i = 0; i < WEENINJA_MAX_FRUIT; i++) {
        if (!state->fruit[i].alive) {
            continue;
        }

        float t = (z_plane - ray.position.z) / ray.direction.z;
        Vector3 in_plane =
            Vector3Add(ray.position, Vector3Scale(ray.direction, t));
        Fruit *f = &state->fruit[i];
        const float dist_sq = Vector3DistanceSqr(
            in_plane, (Vector3){f->position.x, f->position.y, -20.0f});

        if (dist_sq < 1.0f) {
            int s = wn_splitfruit(state, f);

            if (score < 0) {
                return -1;
            } else {
                return score += s;
            }
        }
    }

    return score;
}

int wn_splitfruit(GameState *state, Fruit *f) {
    int new_type = -1;
    switch (f->type) {
    case FRUIT_APPLE:
        new_type = FRUIT_APPLE_HALF;
        break;
    case FRUIT_KIWIFRUIT:
        new_type = FRUIT_KIWIFRUIT_HALF;
        break;
    case FRUIT_ORANGE:
        new_type = FRUIT_ORANGE_HALF;
        break;
    case FRUIT_PINEAPPLE:
        break;

    case FRUIT_BOMB:
        return -1;
    default: return 0;
    }

    f->alive = false;
    Fruit *left;
    Fruit *right;

    provision_fruit(state, &left);
    provision_fruit(state, &right);

    if (f->type == FRUIT_PINEAPPLE) {
        left->type = FRUIT_PINEAPPLE_HALF_TOP;
        right->type = FRUIT_PINEAPPLE_HALF_BOTTOM;
    } else {
        left->type = new_type;
        right->type = new_type;
    }

    left->position = f->position;
    left->velocity.x = -8.0f;
    left->velocity.y = f->velocity.y;
    left->omega = f->omega * 4.0f;
    left->theta = f->theta;
    left->alive = true;
    left->chirality = FRUIT_CHIRALITY_LEFT;

    right->position = f->position;
    right->velocity.x = 8.0f;
    right->velocity.y = f->velocity.y;
    right->omega = f->omega * 4.0f;
    right->theta = f->theta;
    right->alive = true;

    if (f->type == FRUIT_PINEAPPLE) {
        right->chirality = FRUIT_CHIRALITY_LEFT;
    } else {
        right->chirality = FRUIT_CHIRALITY_RIGHT;
    }

    return 1;
}

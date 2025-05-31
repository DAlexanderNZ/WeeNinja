/*
 * Copyright (C) 2025 WeeNinja Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef WEENINJA_APPLICATION_H
#define WEENINJA_APPLICATION_H

#ifndef WEENINJA_MAX_FRUIT
#define WEENINJA_MAX_FRUIT 128
#endif

#include "fruit.h"

typedef struct GameState {
    Fruit fruit[WEENINJA_MAX_FRUIT];
    int n_fruit;
} GameState;

void wn_state_init(GameState *state);
void wn_update(GameState *state);
void wn_spawnfruit(GameState *state, int type);
void wn_killfruit(GameState *state, Fruit *f);
void wn_drawfruit(const GameState *state);
void wn_fruit_pick(GameState *state, Ray ray);

#endif

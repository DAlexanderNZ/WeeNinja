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


#ifndef WEENINJA_FRUIT_H
#define WEENINJA_FRUIT_H

#include <raylib.h>

enum FruitType {
  FRUIT_APPLE = 0,
  FRUIT_ORANGE = 1,
  FRUIT_KIWIFRUIT = 2,
  FRUIT_PINEAPPLE = 3,
  FRUIT_APPLE_HALF = 4,
  FRUIT_ORANGE_HALF = 5,
  FRUIT_KIWIFRUIT_HALF = 6,
  FRUIT_PINEAPPLE_HALF_TOP = 7,
  FRUIT_PINEAPPLE_HALF_BOTTOM = 8,
  FRUIT_BOMB = 9,
  FRUIT_BAMBOO = 10,
  _N_FRUIT
};

enum FruitChirality {
    FRUIT_CHIRALITY_LEFT = 0,
    FRUIT_CHIRALITY_RIGHT = 1,
};

typedef struct Fruit {
    int     type;
    Vector2 position;
    Vector2 velocity;
    float   theta;
    float   omega;
    int     alive;
    int     chirality;
} Fruit;

#endif

/*
 * Copyright (C) 2025  WeeNinja Contributors
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

#include "model.h"
#include "fruit.h"
#include <raylib.h>

typedef struct LazyMesh {
  enum {
    NOT_LOADED,
    LOADED,
  };

  int status;
  union {
      int nuh;
      Model model;
  } data;
} LazyMesh;

const static WNModel models[] = {
    [FRUIT_APPLE] = {.model_file = "resource/goodart/apple.obj",
                     .texture_file = "resource/goodart/apple.png"},
    [FRUIT_ORANGE] = {.model_file = "resource/goodart/orange.obj",
                      .texture_file = "resource/goodart/orange.png"},
    [FRUIT_KIWIFRUIT] = {.model_file = "resource/goodart/kiwifruit.obj",
                         .texture_file = "resource/goodart/kiwifruit.png"},
    [FRUIT_PINEAPPLE] = {.model_file = "resource/goodart/pineapple.obj",
                         .texture_file = "resource/goodart/pineapple.png"},
    [FRUIT_APPLE_HALF] = {.model_file = "resource/goodart/apple_half.obj",
                          .texture_file =
                              "resource/goodart/Apple_Half_Texture.png"},
    [FRUIT_ORANGE_HALF] = {.model_file = "resource/goodart/orange_half.obj",
                           .texture_file = "resource/goodart/orange_split.png"},
    [FRUIT_KIWIFRUIT_HALF] = {.model_file =
                                  "resource/goodart/kiwifruit_half.obj",
                              .texture_file =
                                  "resource/goodart/kiwifruit_half.png"},
    [FRUIT_PINEAPPLE_HALF_TOP] = {.model_file =
                                      "resource/goodart/pineapple_half_top.obj",
                                  .texture_file =
                                      "resource/goodart/pineapple.png"},
    [FRUIT_PINEAPPLE_HALF_BOTTOM] = {
        .model_file = "resource/goodart/pineapple_half_bottom.obj",
        .texture_file = "resource/goodart/pineapple.png" }
};

static LazyMesh loaded_meshes[_N_FRUIT] = {
    [FRUIT_APPLE] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_ORANGE] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_KIWIFRUIT] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_PINEAPPLE] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_APPLE_HALF] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_ORANGE_HALF] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_KIWIFRUIT_HALF] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_PINEAPPLE_HALF_TOP] = {.status = NOT_LOADED, .data = {.nuh = 0}},
    [FRUIT_PINEAPPLE_HALF_BOTTOM] = {.status = NOT_LOADED, .data = {.nuh = 0}},
};

static void load_mesh(int fruit) {
    const WNModel *wnmodel = &models[fruit];

    Model model = LoadModel(wnmodel->model_file);
    Texture2D texture = LoadTexture(wnmodel->texture_file);

    loaded_meshes[fruit].status = LOADED;
    loaded_meshes[fruit].data.model = model;
    loaded_meshes[fruit]
        .data.model.materials[0]
        .maps[MATERIAL_MAP_DIFFUSE]
        .texture = texture;
}

Model get_fruit_model(int fruit) {
    if (loaded_meshes[fruit].status == NOT_LOADED) {
        load_mesh(fruit);
    }

    return loaded_meshes[fruit].data.model;
}

#ifndef RUBIK_H
#define RUBIK_H

#include "raymath.h"

#define CUBIE_POS_EXPAND(pos) pos.i, pos.j, pos.k
enum {
  Rubik_NO_FACE = 0,
  Rubik_F,
  Rubik_B,
  Rubik_R,
  Rubik_L,
  Rubik_U,
  Rubik_D
};

typedef struct CubiePos {
  int i, j, k;
} CubiePos;

typedef struct CubieData {
  CubiePos position;
  Matrix orientationt;
} CubieData;

typedef struct RubikRenderState {
  Vector3 rotation;
  int current_animated_face;
  float move_duration;
  float elapsed_anim_time;
  CubieData cubie_data[3][3][3];
} RubikRenderState;

void update_rubik_render_state(RubikRenderState *rs);
int faces_containing(int i, int j, int k);
Vector3 axis_from_face(int face);

#endif
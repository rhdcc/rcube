#include "rubik.h"

int faces_containing(int i, int j, int k) {
  int mask = 0;
  if(i == 0 && j == 0 && k == 0) return mask;

  if(i == -1) { mask |= (1 << Rubik_L); }
  else if(i ==  1) { mask |= (1 << Rubik_R); }

  if(j == -1) { mask |= (1 << Rubik_D); }
  else if(j ==  1) { mask |= (1 << Rubik_U); }

  if(k == -1) { mask |= (1 << Rubik_B); }
  else if(k == 1) { mask |= (1 << Rubik_F); }

  return mask;
}

Vector3 axis_from_face(int face) { // TODO: Maybe just have an array?
  switch(face) {
    case 1:
    case 2: return (Vector3) { 0.0f, 0.0f, 1.0f };
    case 3:
    case 4: return (Vector3) { 1.0f, 0.0f, 0.0f };
    case 5:
    case 6: return (Vector3) { 0.0f, 1.0f, 0.0f };
    default: return (Vector3) {0};
  }
}

void update_rubik_render_state(RubikRenderState *rs) {
  for(int k = -1; k < 2; ++k) {
    for(int j = -1; j < 2; ++j) {
      for(int i = -1; i < 2; ++i) {
        if(faces_containing(CUBIE_POS_EXPAND(rs->cubie_data[k + 1][j + 1][i + 1].position)) & (1 << rs->current_animated_face)) {
          // Update cubie orientation
          rs->cubie_data[k + 1][j + 1][i + 1].orientationt = MatrixMultiply(
            rs->cubie_data[k + 1][j + 1][i + 1].orientationt,
            MatrixRotateXYZ(Vector3Scale(axis_from_face(rs->current_animated_face), DEG2RAD * 90.f * rs->counter_clockwise))
          );
        
          // Now update the cubie position
          CubiePos cp = rs->cubie_data[k + 1][j + 1][i + 1].position;
          Vector3 cubie_positionf = Vector3RotateByAxisAngle(
            (Vector3){ (float)cp.i, (float)cp.j, (float)cp.k },
            axis_from_face(rs->current_animated_face),
            90.0f * DEG2RAD * rs->counter_clockwise
          );

          // Now, fix precision issues... (Suuuper jancky, but I'm really lazy...)
          rs->cubie_data[k + 1][j + 1][i + 1].position.i =
            FloatEquals(cubie_positionf.x,  0.0f) *  0 +
            FloatEquals(cubie_positionf.x,  1.0f) *  1 +
            FloatEquals(cubie_positionf.x, -1.0f) * -1;

          rs->cubie_data[k + 1][j + 1][i + 1].position.j =
            FloatEquals(cubie_positionf.y,  0.0f) *  0 +
            FloatEquals(cubie_positionf.y,  1.0f) *  1 +
            FloatEquals(cubie_positionf.y, -1.0f) * -1;

          rs->cubie_data[k + 1][j + 1][i + 1].position.k =
            FloatEquals(cubie_positionf.z,  0.0f) *  0 +
            FloatEquals(cubie_positionf.z,  1.0f) *  1 +
            FloatEquals(cubie_positionf.z, -1.0f) * -1;
        }
      }
    }
  }
}
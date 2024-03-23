#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define COL_EXPAND(color) color.r, color.g, color.b, color.a
#define VEC3_EXPAND(vec) vec.x, vec.y, vec.z
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

void draw_cube(Vector3 position, float size) {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  float width = size;
  float height = size;
  float length = size;

  rlPushMatrix();
  rlTranslatef(position.x, position.y, position.z);

  rlBegin(RL_TRIANGLES);

  // Front face
  rlColor4ub(COL_EXPAND(BLUE));
  rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
  rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
  rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left

  rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right
  rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
  rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right

  // Back face
  rlColor4ub(COL_EXPAND(GREEN));
  rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left
  rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
  rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right

  rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
  rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
  rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left

  // Top face
  rlColor4ub(COL_EXPAND(WHITE));
  rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
  rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left
  rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right

  rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
  rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
  rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right

  // Bottom face
  rlColor4ub(COL_EXPAND(YELLOW));
  rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
  rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
  rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left

  rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Right
  rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
  rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left

  // Right face
  rlColor4ub(COL_EXPAND(ORANGE));
  rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
  rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
  rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left

  rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left
  rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
  rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left

  // Left face
  rlColor4ub(COL_EXPAND(RED));
  rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
  rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
  rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right

  rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
  rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
  rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
  rlEnd();
  rlPopMatrix();

  DrawCubeWires(position, width, height, length, BLACK);
}

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

void draw_rubiks_cube(RubikRenderState *rs, float scale) {
  rlPushMatrix();
  rlRotatef(rs->rotation.x, 1.0f, 0.0f, 0.0f); // Rotate the entire Rubik's cube
  rlRotatef(rs->rotation.y, 0.0f, 1.0f, 0.0f);
  rlRotatef(rs->rotation.z, 0.0f, 0.0f, 1.0f);

  for(int k = -1; k < 2; ++k) {
    for(int j = -1; j < 2; ++j) {
      for(int i = -1; i < 2; ++i) {
        CubiePos cubie_position = rs->cubie_data[k + 1][j + 1][i + 1].position;
        Matrix cubie_orientation = rs->cubie_data[k + 1][j + 1][i + 1].orientationt;
        rlPushMatrix();
        if(faces_containing(CUBIE_POS_EXPAND(cubie_position)) & (1 << rs->current_animated_face)) {
          rlRotatef(
            (360.0f / 4) * (rs->elapsed_anim_time / rs->move_duration),
            VEC3_EXPAND(axis_from_face(rs->current_animated_face))
          );
        }

        rlScalef(scale, scale, scale);
        rlTranslatef((float)cubie_position.i, (float)cubie_position.j, (float)cubie_position.k);
        
        Quaternion q = QuaternionFromMatrix(cubie_orientation);
        Vector3 v;
        float an;
        QuaternionToAxisAngle(q, &v, &an);
        rlRotatef(an * RAD2DEG, VEC3_EXPAND(v));
        draw_cube((Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f);
        rlPopMatrix();
      }
    }
  }

  rlPopMatrix();
}

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Cube - The Final");

  Camera3D camera = { 0 };
  camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.projection = CAMERA_PERSPECTIVE;

  RubikRenderState rs = { .move_duration = 0.6f };
  for(int k = -1; k < 2; ++k) {
    for(int j = -1; j < 2; ++j) {
      for(int i = -1; i < 2; ++i) {
        rs.cubie_data[k + 1][j + 1][i + 1].position = (CubiePos){ i, j, k };
        rs.cubie_data[k + 1][j + 1][i + 1].orientationt = MatrixIdentity();
      }
    }
  }

  SetTargetFPS(60);
  while(!WindowShouldClose()) {
    float delta_time = GetFrameTime();

    if(!rs.current_animated_face) {
      if(IsKeyDown(KEY_U)) rs.current_animated_face = Rubik_U;
      if(IsKeyDown(KEY_D)) rs.current_animated_face = Rubik_D;
      if(IsKeyDown(KEY_F)) rs.current_animated_face = Rubik_F;
      if(IsKeyDown(KEY_B)) rs.current_animated_face = Rubik_B;
      if(IsKeyDown(KEY_L)) rs.current_animated_face = Rubik_L;
      if(IsKeyDown(KEY_R)) rs.current_animated_face = Rubik_R;
    }

    if(rs.elapsed_anim_time == rs.move_duration) {
      for(int k = -1; k < 2; ++k) {
        for(int j = -1; j < 2; ++j) {
          for(int i = -1; i < 2; ++i) {
            if(faces_containing(CUBIE_POS_EXPAND(rs.cubie_data[k + 1][j + 1][i + 1].position)) & (1 << rs.current_animated_face)) {
              // Update cubie orientation
              rs.cubie_data[k + 1][j + 1][i + 1].orientationt = MatrixMultiply(
                rs.cubie_data[k + 1][j + 1][i + 1].orientationt,
                MatrixRotateXYZ(Vector3Scale(axis_from_face(rs.current_animated_face), DEG2RAD * 90.f))
              );
            
              // Now update the cubie position
              CubiePos cp = rs.cubie_data[k + 1][j + 1][i + 1].position;
              Vector3 cubie_positionf = Vector3RotateByAxisAngle(
                (Vector3){ (float)cp.i, (float)cp.j, (float)cp.k },
                axis_from_face(rs.current_animated_face),
                90.0f * DEG2RAD
              );

              // Now, fix precision issues... (Suuuper jancky, but I'm really lazy...)
              rs.cubie_data[k + 1][j + 1][i + 1].position.i =
                FloatEquals(cubie_positionf.x,  0.0f) *  0 +
                FloatEquals(cubie_positionf.x,  1.0f) *  1 +
                FloatEquals(cubie_positionf.x, -1.0f) * -1;

              rs.cubie_data[k + 1][j + 1][i + 1].position.j =
                FloatEquals(cubie_positionf.y,  0.0f) *  0 +
                FloatEquals(cubie_positionf.y,  1.0f) *  1 +
                FloatEquals(cubie_positionf.y, -1.0f) * -1;

              rs.cubie_data[k + 1][j + 1][i + 1].position.k =
                FloatEquals(cubie_positionf.z,  0.0f) *  0 +
                FloatEquals(cubie_positionf.z,  1.0f) *  1 +
                FloatEquals(cubie_positionf.z, -1.0f) * -1;
            }
          }
        }
      } 

      rs.elapsed_anim_time = 0;
      rs.current_animated_face = Rubik_NO_FACE;
    }

    if(rs.current_animated_face) {
      rs.elapsed_anim_time += delta_time;
      if(rs.elapsed_anim_time >= rs.move_duration) rs.elapsed_anim_time = rs.move_duration;
    }

    BeginDrawing();

    BeginMode3D(camera);

    ClearBackground(BLACK);
    draw_rubiks_cube(&rs, 1.3f);

    EndMode3D();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

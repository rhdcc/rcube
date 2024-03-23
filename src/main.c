#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "rubik.h"

#define COL_EXPAND(color) color.r, color.g, color.b, color.a
#define VEC3_EXPAND(vec) vec.x, vec.y, vec.z

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
      update_rubik_render_state(&rs);

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

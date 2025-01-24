#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <string.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#define GS_SHADER_PATH "shaders/gaussian_splat.frag"
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 800;
int screenHeight = 450;

//----------------------------------------------------------------------------------
// Structs for the Gaussian Splat Shader viewer
//----------------------------------------------------------------------------------
typedef struct {
  Shader shader;
  int resolutionLoc;
  int centerLoc;
  int sigmaXXLoc;
  int sigmaYYLoc;
  int sigmaXYLoc;
  int amplitudeLoc;
} GaussianShader;

typedef struct {
  float amplitude;
  float sigmaXX;
  float sigmaYY;
  float sigmaXY;
  Vector2 center;
} GaussianParams;

typedef struct {
  GaussianShader gsShader;
  GaussianParams gsParams;
  RenderTexture2D target;

} MyDrawFrame;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(MyDrawFrame *myFrame);

void InitGausianShader(GaussianShader *gausianShader);

void UpdateGausianShader(const GaussianParams *params,
                         GaussianShader *gausianShader);

void InitMyDrawFrame(MyDrawFrame *myFrame);

//----------------------------------------------------------------------------------
// Main Entry Point
//----------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  MyDrawFrame myFrame;
  InitMyDrawFrame(&myFrame);
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {

    UpdateDrawFrame(&myFrame);
  }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void InitMyDrawFrame(MyDrawFrame *myFrame) {
  myFrame->gsParams.amplitude = 1.0f;
  myFrame->gsParams.sigmaXX = 1.0f;
  myFrame->gsParams.sigmaYY = 1.0f;
  myFrame->gsParams.sigmaXY = 0.0f;
  myFrame->gsParams.center = (Vector2){0.0f, 0.0f};
  myFrame->target = LoadRenderTexture(screenWidth, screenHeight);
  // I should have made it so this happens in Set Gausian values
  InitGausianShader(&myFrame->gsShader);
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(MyDrawFrame *myFrame) {
  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update your variables here
  //----------------------------------------------------------------------------------
  // Update parameters

  float scale = 0.001f;
  float time = GetTime();
  float angle = time * 0.5f;
  float c = cosf(angle);
  float s = sinf(angle);

  myFrame->gsParams.sigmaXX = scale * (c * c + 0.25f * s * s);
  myFrame->gsParams.sigmaYY = scale * (s * s + 0.25f * c * c);
  myFrame->gsParams.sigmaXY = scale * 0.75f * s * c;
  myFrame->gsParams.amplitude = 1.0f;
  myFrame->gsParams.center =
      (Vector2){GetMousePosition().x / (float)GetScreenWidth(),
                GetMousePosition().y / (float)GetScreenHeight()};
  TraceLog(LOG_INFO, "Center: %f, %f", myFrame->gsParams.center.x,
           myFrame->gsParams.center.y);
  TraceLog(LOG_INFO, "SigmaXX: %f", myFrame->gsParams.sigmaXX);
  TraceLog(LOG_INFO, "SigmaYY: %f", myFrame->gsParams.sigmaYY);
  TraceLog(LOG_INFO, "SigmaXY: %f", myFrame->gsParams.sigmaXY);
  TraceLog(LOG_INFO, "Amplitude: %f", myFrame->gsParams.amplitude);
  UpdateGausianShader(&myFrame->gsParams, &myFrame->gsShader);
  BeginTextureMode(myFrame->target); // Enable drawing to texture
  ClearBackground(BLACK);            // Clear the render texture

  // Draw a rectangle in shader mode to be used as shader canvas
  // NOTE: Rectangle uses font white character texture coordinates,
  // so shader can not be applied here directly because input vertexTexCoord
  // do not reent full screen coordinates (space where want to apply
  // shader)
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
  EndTextureMode(); // End drawing to texture (now we have a blank texture

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();

  // DrawText("Congrats! You created your first window!", 190, 200, 20,
  // LIGHTGRAY);
  BeginShaderMode(myFrame->gsShader.shader);

  DrawTextureRec(myFrame->target.texture,
                 (Rectangle){0, 0, (float)myFrame->target.texture.width,
                             (float)-myFrame->target.texture.height},
                 (Vector2){0.0f, 0.0f}, WHITE);
  EndShaderMode();
  EndDrawing();
  //----------------------------------------------------------------------------------
}

void InitGausianShader(GaussianShader *gausianShader) {
  gausianShader->shader = LoadShader(
      NULL, "/home/jpleona/jpleona_c/raylibGS/myGS/resources/shaders/gsn.fs");
  gausianShader->amplitudeLoc =
      GetShaderLocation(gausianShader->shader, "u_Amplitude");
  gausianShader->sigmaXXLoc =
      GetShaderLocation(gausianShader->shader, "u_Sigma_xx");
  gausianShader->sigmaYYLoc =
      GetShaderLocation(gausianShader->shader, "u_Sigma_yy");
  gausianShader->sigmaXYLoc =
      GetShaderLocation(gausianShader->shader, "u_Sigma_xy");
  gausianShader->centerLoc =
      GetShaderLocation(gausianShader->shader, "u_Center");
  gausianShader->resolutionLoc =
      GetShaderLocation(gausianShader->shader, "u_Resolution");

  float resolution[2] = {(float)screenWidth, (float)screenHeight};
  SetShaderValue(gausianShader->shader, gausianShader->resolutionLoc,
                 resolution, SHADER_UNIFORM_VEC2);
  TraceLog(LOG_INFO, "Resolution: %f x %f", resolution[0], resolution[1]);
}

void UpdateGausianShader(const GaussianParams *params,
                         GaussianShader *gausianShader) {
  // Update shader parameters
  SetShaderValue(gausianShader->shader, gausianShader->amplitudeLoc,
                 &params->amplitude, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gausianShader->shader, gausianShader->sigmaXXLoc,
                 &params->sigmaXX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gausianShader->shader, gausianShader->sigmaYYLoc,
                 &params->sigmaYY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gausianShader->shader, gausianShader->sigmaXYLoc,
                 &params->sigmaXY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gausianShader->shader, gausianShader->centerLoc,
                 &params->center, SHADER_UNIFORM_VEC2);
}
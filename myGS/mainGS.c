/*******************************************************************************************
 *
 *   raylib [shaders] example - Sieve of Eratosthenes
 *
 *   NOTE: Sieve of Eratosthenes, the earliest known (ancient Greek) prime
 *number sieve.
 *
 *       "Sift the twos and sift the threes,
 *        The Sieve of Eratosthenes.
 *        When the multiples sublime,
 *        the numbers that are left are prime."
 *
 *   NOTE: This example raylib OpenGL 3.3 or ES2 versions for shaders
 *support, OpenGL 1.1 does not support shaders, recompile raylib to OpenGL 3.3
 *version.
 *
 *   NOTE: Shaders used in this example are #version 330 (OpenGL 3.3).
 *
 *   Example originally created with raylib 2.5, last time updated with
 *raylib 4.0
 *
 *   Example contributed by ProfJski and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an
 *OSI-certified, BSD-like license that allows static linking with closed source
 *software
 *
 *   Copyright (c) 2019-2024 ProfJski and Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

struct GsShader {
  // is there somethin
  int resolution;
  int center;
  int sigma;
  int sigmaXX;
  int sigmaXY;
  int amplitude;
  Shader shader;
  int resolutionLoc;
  int centerLoc;
  int sigmaXXLoc;
  int sigmaYYLoc;
  int sigmaXYLoc;
  int amplitudeLoc;
};

// void SetupGsShader(Shader *shader, struct GsShader *gsShader) {
//   // Shader shader = Load
//   Shader shader = LoadShader(0, "");
//   // Only try to get location if shader loaded successfully
//   int u_time_loc = GetShaderLocation(shader, "u_time");

//   // Get shader uniform locations
//   int resolutionLoc = GetShaderLocation(shader, "u_Resolution");
//   int centerLoc = GetShaderLocation(shader, "u_Center");
//   int sigmaXXLoc = GetShaderLocation(shader, "u_Sigma_xx");
//   int sigmaYYLoc = GetShaderLocation(shader, "u_Sigma_yy");
//   int sigmaXYLoc = GetShaderLocation(shader, "u_Sigma_xy");
//   int amplitudeLoc = GetShaderLocation(shader, "u_Amplitude");
// };

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight,
             "raylib [shaders] example - Sieve of Eratosthenes");

  RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
  // Load Eratosthenes shader
  // NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default
  // vertex shader
  // /home/jpleona/jpleona_C/raylibGS/myGS/resources/shaders/eratosthenes.fs
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  Shader shader = LoadShader(
      0,
      TextFormat("/home/leona/jpleona_c/raylibGS/myGS/resources/shaders/gsn.fs",
                 GLSL_VERSION));
  if (shader.id == 0) {
    TraceLog(LOG_ERROR, "Shader failed to load");
    // Handle error
  }

  // Only try to get location if shader loaded successfully
  int u_time_loc = GetShaderLocation(shader, "u_time");

  // Get shader uniform locations
  int resolutionLoc = GetShaderLocation(shader, "u_Resolution");
  int centerLoc = GetShaderLocation(shader, "u_Center");
  int sigmaXXLoc = GetShaderLocation(shader, "u_Sigma_xx");
  int sigmaYYLoc = GetShaderLocation(shader, "u_Sigma_yy");
  int sigmaXYLoc = GetShaderLocation(shader, "u_Sigma_xy");
  int amplitudeLoc = GetShaderLocation(shader, "u_Amplitude");
  // Set static uniforms
  float resolution[2] = {(float)screenWidth, (float)screenHeight};
  SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Draw
    float center[2] = {GetMousePosition().x / (float)screenWidth,
                       GetMousePosition().y / (float)screenHeight};

    // Time-based rotation for testing
    float time = GetTime();
    float scale = 0.001f; // Base scale for the Gaussian

    // Create a rotating elliptical Gaussian
    float angle = time * 0.5f; // Slower rotation
    float c = cosf(angle);
    float s = sinf(angle);

    // Compute covariance matrix for rotated elliptical Gaussian
    float sigmaXX = scale * (c * c + 0.25f * s * s);
    float sigmaYY = scale * (s * s + 0.25f * c * c);
    float sigmaXY = scale * 0.75f * s * c;
    float amplitude = 1.0f;

    SetShaderValue(shader, centerLoc, center, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, sigmaXXLoc, &sigmaXX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, sigmaYYLoc, &sigmaYY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, sigmaXYLoc, &sigmaXY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, amplitudeLoc, &amplitude, SHADER_UNIFORM_FLOAT);
    //----------------------------------------------------------------------------------
    BeginTextureMode(target); // Enable drawing to texture
    ClearBackground(BLACK);   // Clear the render texture

    // Draw a rectangle in shader mode to be used as shader canvas
    // NOTE: Rectangle uses font white character texture coordinates,
    // so shader can not be applied here directly because input vertexTexCoord
    // do not reent full screen coordinates (space where want to apply
    // shader)
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    EndTextureMode(); // End drawing to texture (now we have a blank texture
                      // available for the shader)

    BeginDrawing();
    ClearBackground(RAYWHITE); // Clear screen background

    BeginShaderMode(shader);
    SetShaderValue(shader, u_time_loc, &time, SHADER_UNIFORM_FLOAT);
    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates
    // (left-bottom)
    DrawTextureRec(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               (float)-target.texture.height},
                   (Vector2){0.0f, 0.0f}, WHITE);
    EndShaderMode();
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadShader(shader);        // Unload shader
  UnloadRenderTexture(target); // Unload render texture

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

// flywheel for game design
// struct for storing the valus and the locations for the gausian shader

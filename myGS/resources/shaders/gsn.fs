#version 300 es
precision highp float;

uniform vec2 u_Resolution;
uniform vec2 u_Center;
uniform float u_Sigma_xx;
uniform float u_Sigma_yy;
uniform float u_Sigma_xy;
uniform float u_Amplitude;

out vec4 fragColor;

void main() {
    // Get normalized pixel coordinates
    vec2 uv = vec2(gl_FragCoord.x / u_Resolution.x, 1.0f - (gl_FragCoord.y / u_Resolution.y)  // Flip Y coordinate
    ); 
    // Convert to centered coordinate system
    // ... why is this flipped in the y dir
    vec2 pos = uv - u_Center;

    // Add a small epsilon to prevent division by zero
    float det = max(u_Sigma_xx * u_Sigma_yy - u_Sigma_xy * u_Sigma_xy, 1e-6f);

    // Compute inverse of covariance matrix (with safety checks)
    float inv_xx = u_Sigma_yy / det;
    float inv_yy = u_Sigma_xx / det;
    float inv_xy = -u_Sigma_xy / det;

    // Compute quadratic form with safety clamp
    float quad_form = min(pos.x * (inv_xx * pos.x + inv_xy * pos.y) +
        pos.y * (inv_xy * pos.x + inv_yy * pos.y), 100.0f  // Prevent exp() from overflowing
    );

    float gaussian = u_Amplitude * exp(-0.5f * quad_form);

    fragColor = vec4(vec3(gaussian), 1.0f);
}
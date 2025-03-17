#version 100
precision mediump float;

varying vec3 fragPosition;
varying vec3 fragNormal;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    float ratio = 1.00 / 5.10;
    vec3 I = normalize(fragPosition - cameraPos);
    // vec3 R = reflect(I, normalize(fragNormal));
    vec3 R = refract(I, normalize(fragNormal), ratio);
    gl_FragColor = vec4(textureCube(skybox, R).rgb, 1.0);
}

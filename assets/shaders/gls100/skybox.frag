#version 100
precision mediump float;

varying vec3 fragTexCoords;

uniform samplerCube skybox;

void main()
{
    gl_FragColor = textureCube(skybox, fragTexCoords);
}

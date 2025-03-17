#version 100
precision mediump float;

attribute vec3 aPos;

varying vec3 fragTexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    fragTexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

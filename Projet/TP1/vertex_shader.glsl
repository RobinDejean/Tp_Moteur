#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in float vertexNoise;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

out vec2 uv;
out vec4 position;
out float TerrainNoise;

void main(){

        gl_Position = MVP * vec4(vertices_position_modelspace,1);
        position = vec4(vertices_position_modelspace, 1);
        uv = vertexUV;
        TerrainNoise = vertexNoise;
}


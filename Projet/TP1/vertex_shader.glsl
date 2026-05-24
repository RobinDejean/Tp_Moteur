#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in float vertexNoise;
layout(location = 3) in vec3 normal_modelspace;
layout(location = 4) in vec3 tangent_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 model;

out vec2 uv;
out vec4 position;
out float TerrainNoise;
out mat3 TBN;

void main(){
        vec3 T = normalize(mat3(model) * tangent_modelspace);
        vec3 N = normalize(mat3(model) * normal_modelspace);

        // orthonormalisation
        T = normalize(T - dot(T, N) * N);

        vec3 B = cross(N, T);

        TBN = mat3(T, B, N);
        //TBN = mat3(1.0);

        gl_Position = MVP * vec4(vertices_position_modelspace,1);
        position = model * vec4(vertices_position_modelspace, 1);
        uv = vertexUV;
        TerrainNoise = vertexNoise;
}


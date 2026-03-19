#include "fonctions.hpp"
#include "globals.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
//GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>
//#include "common/image_ppm.h"
#include "common/ImageBase.h"
// #include <common/ImageBase.cpp>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "globals.hpp"
#include "fonctions.hpp"

void openOBJ(const std::string& filename, Mesh& mesh)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Cannot open " << filename << std::endl;
        return;
    }

    mesh.indexed_vertices.clear();
    mesh.uvs.clear();
    mesh.indices.clear();
    mesh.triangles.clear();

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        // Vertex position
        if (prefix == "v")
        {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        }
        // Texture coordinates
        else if (prefix == "vt")
        {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            uv.y = 1-uv.y;
            temp_uvs.push_back(uv);
        }
        // Face
        else if (prefix == "f")
        {
            std::vector<unsigned int> faceIndices;

            std::string vertexData;
            while (ss >> vertexData)
            {
                std::stringstream vs(vertexData);
                std::string vStr, vtStr, vnStr;

                std::getline(vs, vStr, '/');
                std::getline(vs, vtStr, '/');
                std::getline(vs, vnStr, '/');

                unsigned int vIndex = std::stoi(vStr);
                unsigned int uvIndex = 0;

                if (!vtStr.empty())
                    uvIndex = std::stoi(vtStr);

                glm::vec3 position = temp_vertices[vIndex - 1];
                glm::vec2 uv(0.0f);

                if (uvIndex > 0 && uvIndex <= temp_uvs.size())
                    uv = temp_uvs[uvIndex - 1];

                mesh.indexed_vertices.push_back(position);
                mesh.uvs.push_back(uv);

                unsigned int newIndex = mesh.indexed_vertices.size() - 1;
                faceIndices.push_back(newIndex);
            }

            //  Triangulation automatique (fan triangulation)
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
            {
                mesh.indices.push_back(faceIndices[0]);
                mesh.indices.push_back(faceIndices[i]);
                mesh.indices.push_back(faceIndices[i + 1]);

                mesh.triangles.push_back({
                    faceIndices[0],
                    faceIndices[i],
                    faceIndices[i + 1]
                });
            }
        }
    }

    file.close();
}

void sphere(Mesh &mesh, float radius, int nblignes)
{
    mesh.indexed_vertices.clear();
    mesh.indices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    mesh.noise.clear();

    // Génération des sommets
    for (unsigned int j = 0; j <= nblignes; j++)
    {
        float v = (float)j / nblignes;
        float theta = v * glm::pi<float>(); // latitude [0, PI]

        for (unsigned int i = 0; i <= nblignes; i++)
        {
            float u = (float)i / nblignes;
            float phi = u * glm::two_pi<float>(); // longitude [0, 2PI]

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            mesh.indexed_vertices.push_back(glm::vec3(x, y, z));
            mesh.uvs.push_back(glm::vec2(u,v));
            mesh.noise.push_back(0.0f);
        }
    }

    // Génération des indices
    for (unsigned int j = 0; j < nblignes; j++)
    {
        for (unsigned int i = 0; i < nblignes; i++)
        {
            unsigned int first  = j * (nblignes + 1) + i;
            unsigned int second = first + nblignes + 1;

            // Triangle 1
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);

            // Triangle 2
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);

            mesh.triangles.push_back({ first, second, first + 1 });
            mesh.triangles.push_back({ second, second + 1, first + 1 });
        }
    }
}

void setupCube(Mesh &mesh, float taille)
{
    mesh.indexed_vertices.clear();
    mesh.indices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    mesh.noise.clear();

    mesh.indexed_vertices.push_back(glm::vec3(0,0,0));
    mesh.indexed_vertices.push_back(glm::vec3(taille,0,0));
    mesh.indexed_vertices.push_back(glm::vec3(taille,0,taille));
    mesh.indexed_vertices.push_back(glm::vec3(0,0, taille));
    mesh.indexed_vertices.push_back(glm::vec3(0,taille,0));
    mesh.indexed_vertices.push_back(glm::vec3(taille,taille,0));
    mesh.indexed_vertices.push_back(glm::vec3(taille,taille,taille));
    mesh.indexed_vertices.push_back(glm::vec3(0,taille, taille));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));
    mesh.uvs.push_back(glm::vec2(0,0));


    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    
    mesh.indices.push_back(0);
    mesh.indices.push_back(2);
    mesh.indices.push_back(3);

    mesh.indices.push_back(4);
    mesh.indices.push_back(5);
    mesh.indices.push_back(6);

    mesh.indices.push_back(4);
    mesh.indices.push_back(6);
    mesh.indices.push_back(7);

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(5);
    
    mesh.indices.push_back(0);
    mesh.indices.push_back(5);
    mesh.indices.push_back(4);

    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(6);

    mesh.indices.push_back(1);
    mesh.indices.push_back(6);
    mesh.indices.push_back(5);

    mesh.indices.push_back(2);
    mesh.indices.push_back(3);
    mesh.indices.push_back(7);

    mesh.indices.push_back(2);
    mesh.indices.push_back(7);
    mesh.indices.push_back(6);

    mesh.indices.push_back(3);
    mesh.indices.push_back(0);
    mesh.indices.push_back(4);

    mesh.indices.push_back(3);
    mesh.indices.push_back(4);
    mesh.indices.push_back(7);
}

void setupMesh(Mesh& mesh) {
    // On crée et on "bind" le VAO
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    // Création du VBO pour les positions
    glGenBuffers(1, &mesh.indexed_vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.indexed_vertices_vbo);
    // on copie les positions
    glBufferData(GL_ARRAY_BUFFER, mesh.indexed_vertices.size() * sizeof(glm::vec3), mesh.indexed_vertices.data(), GL_STATIC_DRAW);

    // lecture VAO
    glEnableVertexAttribArray(0);
    // 0,3 = canal 0 vertex shader, 3 = nb float a lire
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // creation ebo pour les indices
    glGenBuffers(1, &mesh.indices_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indices_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    // creation uvs vbo
    if (!mesh.uvs.empty()) {
        glGenBuffers(1, &mesh.uvs_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.uvs_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(glm::vec2), mesh.uvs.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        // 1,2 = canal 1 vertex shader, 2 = nb float a lire
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    //creation noise vbo
    if (!mesh.noise.empty()) {
        glGenBuffers(1, &mesh.noise_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.noise_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.noise.size() * sizeof(float), mesh.noise.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(2); // Canal 2 !
        // 2 = Canal 2, 1 = un seul float a lire
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // On debind
    glBindVertexArray(0);
}

void render(Mesh& mesh) {
    //bind vao
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SceneRender(Node* node, glm::mat4 transformationParent, GLuint MatrixID, glm::mat4 viewProj, GLuint programID) {
    if (node == nullptr) return;
    glm::mat4 modelMatrix = transformationParent * node->computeMatTransformation();

    if (node->mesh != nullptr) {
        glm::mat4 MVP = viewProj * modelMatrix;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));

        glUniform1i(glGetUniformLocation(programID, "mode"), node->mode);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node->textureID); 
        
        render(*(node->mesh));
    }
    for (Node* enfant : node->enfants) {
        SceneRender(enfant, modelMatrix, MatrixID, viewProj,programID);
    }
}

void world(Mesh &mesh){
    mesh.indexed_vertices.clear();
    mesh.indices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    mesh.noise.clear();
        for(unsigned int i = 0; i < longueur; i++){
        for(unsigned int j = 0; j < hauteur; j++){
            float y = (float)heightMap[j][i]/255.;
            
            mesh.indexed_vertices.push_back(vec3(((float) i / (longueur-1)) - 0.5, y, ((float) j / (hauteur-1)) - 0.5));

            mesh.uvs.push_back(
                vec2((float)i / (longueur - 1),
                    (float)j / (hauteur - 1))
            );

            float random_val = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;
            mesh.noise.push_back(random_val);
        }
    }

    for(unsigned int i = 0; i < longueur - 1; i++){
        for(unsigned int j = 0; j < hauteur - 1; j++){
            mesh.triangles.push_back({j * longueur + i, j * longueur + i + 1, (j + 1) * longueur + i + 1});

            mesh.indices.push_back(j * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i + 1);
            mesh.indices.push_back(j * longueur + i + 1);

            mesh.triangles.push_back({j * longueur + i, (j + 1) * longueur + i, (j + 1) * longueur + i + 1});
            
            mesh.indices.push_back(j * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i + 1);
        }
    }
}

void worldFlat(Mesh &mesh){
    mesh.indexed_vertices.clear();
    mesh.indices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    mesh.noise.clear();
        for(unsigned int i = 0; i < longueur; i++){
        for(unsigned int j = 0; j < hauteur; j++){
            
            mesh.indexed_vertices.push_back(vec3(((float) i / (longueur-1)) - 0.5, 0., ((float) j / (hauteur-1)) - 0.5));

            mesh.uvs.push_back(
                vec2((float)i / (longueur - 1),
                    (float)j / (hauteur - 1))
            );
        }
    }

    for(unsigned int i = 0; i < longueur - 1; i++){
        for(unsigned int j = 0; j < hauteur - 1; j++){
            mesh.triangles.push_back({j * longueur + i, j * longueur + i + 1, (j + 1) * longueur + i + 1});

            mesh.indices.push_back(j * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i + 1);
            mesh.indices.push_back(j * longueur + i + 1);

            mesh.triangles.push_back({j * longueur + i, (j + 1) * longueur + i, (j + 1) * longueur + i + 1});
            
            mesh.indices.push_back(j * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i + 1);
        }
    }
}

// pour modifier la taille du terrain, obligé de delete puis recreer
void updateTerrain() {
    // on delete les anciens buffers
    glDeleteBuffers(1, &terrain.indexed_vertices_vbo);
    glDeleteBuffers(1, &terrain.indices_vbo);
    glDeleteBuffers(1, &terrain.uvs_vbo);
    glDeleteBuffers(1,&terrain.noise_vbo);
    glDeleteVertexArrays(1, &terrain.VAO);

    //on regenere le terrain
    world(terrain);

    // on recreer les buffers
    setupMesh(terrain);
}

void updateHeight(glm::vec3 &position){
    float z = position.z;
    float x = position.x;
    if (x > 0.5f || x < -0.5f || z > 0.5f || z < -0.5f ){
        return;
    }
    
    float gridX = (x + 0.5f) * (longueur - 1);
    float gridZ = (z + 0.5f) * (hauteur - 1);

    int i = (int)gridX;
    int j = (int)gridZ;

    int cell = j * (longueur - 1) + i;
    int tri = cell * 2;

    float fx = gridX - i;
    float fz = gridZ - j;

    if (fx + fz > 1.0f)
        tri += 1;

    int tri1 = terrain.indices[tri *3];
    int tri2 = terrain.indices[tri *3+1];
    int tri3 = terrain.indices[tri *3+2];

    glm::vec3 p1 = terrain.indexed_vertices[tri1];
    glm::vec3 p2 = terrain.indexed_vertices[tri2];
    glm::vec3 p3 = terrain.indexed_vertices[tri3];

    float D=(p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float lambda1 = ((p2.z - p3.z) * (x - p3.x) + (p3.x - p2.x) * (z - p3.z)) / D;
    float lambda2 = ((p3.z - p1.z) * (x - p3.x) + (p1.x - p3.x) * (z - p3.z)) / D;
    float lambda3 = 1 - lambda1 - lambda2;

    position.y = lambda1 * p1.y + lambda2 * p2.y + lambda3 *p3.y +0.02;

}

void updatePos(Node &node){
    glm::vec3 a = glm::vec3(0,-9.81f,0);
    node.vitesse += a * deltaTime; 
    // node.transformation =  glm::translate(
    // node.transformation,
    // node.vitesse * deltaTime);

    node.translation +=  node.vitesse * deltaTime;

}

void collisionTerrain(Node &node){
    float z = node.translation.z;
    float x = node.translation.x;
    if (x > 0.49f || x < -0.49f || z > 0.49f || z < -0.49f ){
        node.vitesse = glm::vec3(0.f);
        node.translation -= 0.01 * node.translation;
        return;
    }
    
    float gridX = (x + 0.5f) * (longueur - 1);
    float gridZ = (z + 0.5f) * (hauteur - 1);

    int i = (int)gridX;
    int j = (int)gridZ;

    int cell = j * (longueur - 1) + i;
    int tri = cell * 2;

    float fx = gridX - i;
    float fz = gridZ - j;

    if (fx + fz > 1.0f)
        tri += 1;

    int tri1 = terrain.indices[tri *3];
    int tri2 = terrain.indices[tri *3+1];
    int tri3 = terrain.indices[tri *3+2];

    glm::vec3 p1 = terrain.indexed_vertices[tri1];
    glm::vec3 p2 = terrain.indexed_vertices[tri2];
    glm::vec3 p3 = terrain.indexed_vertices[tri3];

    float D=(p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float lambda1 = ((p2.z - p3.z) * (x - p3.x) + (p3.x - p2.x) * (z - p3.z)) / D;
    float lambda2 = ((p3.z - p1.z) * (x - p3.x) + (p1.x - p3.x) * (z - p3.z)) / D;
    float lambda3 = 1 - lambda1 - lambda2;

    glm::vec3 n = glm::normalize(glm::cross(p2-p1,p3 - p1));

    float terrain_y = lambda1 * p1.y + lambda2 * p2.y + lambda3 *p3.y;
    if(node.translation.y - terrain_y >= 0){
        return;
    }
    else{
        node.translation.y = terrain_y + 0.001;
        node.vitesse =  node.vitesse - 2*glm::dot(node.vitesse, n) * n;
        //node.vitesse.x = -node.vitesse.x;

    }

}


void updateMeshResolution(Node &node){
    if(glm::distance(macaqueTranslate, camera_position) > 2.f ){
        node.mesh = &macaqueLow;
    }
    if(glm::distance(macaqueTranslate, camera_position) < 2.f ){
        node.mesh = &macaque;
    }
}
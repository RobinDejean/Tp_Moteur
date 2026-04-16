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
#include "common/ImageBase.h"


#ifndef MESH_HPP
#define MESH_HPP


class Mesh{
    std::vector<std::vector<unsigned int>> triangles;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> uvs;
    std::vector<float> noise;

    GLuint VAO;
    GLuint indexed_vertices_vbo;
    GLuint uvs_vbo;
    GLuint noise_vbo;
    GLuint indices_vbo;


    public:
        //CONSTRUCTORS
        Mesh();
        

        //DESTRUCTORS
        ~Mesh();

        //GETTERS
        const std::vector<std::vector<unsigned int>>& getTriangles() const;
        const std::vector<glm::vec3>& getIndexedVertices() const;
        const std::vector<glm::vec2>& getUvs() const;
        const std::vector<float>& getNoise() const;
        const std::vector<unsigned int>& getIndices() const;


        //SETTERS
        void setTriangles(const std::vector<std::vector<unsigned int>>& triangles);
        void setIndexedVertices(const std::vector<glm::vec3>& indexed_vertices);
        void setUvs(const std::vector<glm::vec2>& uvs);
        void setNoise(const std::vector<float>& noise);
        void setIndices(const std::vector<unsigned int>& indices);

        //METHODS
        void addTriangle(const std::vector<unsigned int>& triangle);
        void addIndexedVertex(const glm::vec3& vertex);
        void addUv(const glm::vec2& uv);
        void addNoise(float noiseValue);
        void addIndice(unsigned int indice);
        void setupMesh();
        void render();
        void world(int longueur, int hauteur, ImageBase& heightMap);
        void worldPenche(int longueur, int hauteur, double pourcentage);
        void deleteBuffers();

        //CREATE MESHES
        void sphere(float radius, int nblignes);
        void cube(float taille);
        void car(float taille);
        void createWheel(float radius, float width, int segments);

};


#endif

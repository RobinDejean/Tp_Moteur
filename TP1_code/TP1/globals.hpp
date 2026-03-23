#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/ImageBase.h"

// =====================
// STRUCTS
// =====================

struct Mesh{
    std::vector<std::vector<unsigned int>> triangles;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
    std::vector<float> noise;

    GLuint VAO = 0;
    GLuint indexed_vertices_vbo = 0;
    GLuint uvs_vbo = 0;
    GLuint noise_vbo = 0;
    GLuint indices_vbo = 0;
};

struct Coeff{
    double friction_statique;
    double friction_cinetique;
    double rebond;

    Coeff();
    Coeff(double fs, double fc, double r);
};

struct Node{
    Mesh* mesh;
    double volume;
    std::vector<Node*> enfants;

    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    GLuint textureID;
    int mode;
    glm::vec3 vitesse;
    double masse;

    Coeff coeff;

    Node();
    glm::mat4 computeMatTransformation();
};


struct SceneGraph{
    Node* racine;
};

// =====================
// GLOBALS
// =====================

// fenêtre
extern GLFWwindow* window;

// caméra
extern glm::vec3 camera_position;
extern glm::vec3 camera_target;
extern glm::vec3 camera_up;
extern glm::vec3 camera_front;
extern glm::vec3 macaqueTranslate;

// paramètres caméra
extern float cameraSpeed;
extern float angle;
extern float zoom;
extern float theta;
extern glm::mat4 rotationY;

// temps
extern float deltaTime;
extern float lastFrame;

// terrain
extern int longueur;
extern int hauteur;
extern ImageBase heightMap;

// meshes
extern Mesh terrain;
extern Mesh boat;
extern Mesh soleil;
extern Mesh lune;
extern Mesh terre;
extern Mesh mars;
extern Mesh macaque;
extern Mesh macaqueLow;
extern Mesh cube;

// scene graph
extern SceneGraph Planete;
extern Node NodeSoleil;
extern Node NodeLune;
extern Node NodeTerrain;
extern Node NodeTerre;
extern Node NodeMars;
extern Node NodeMacaque;
extern Node NodeCube;

// autres
extern int mode;
extern bool mouvement;
extern double d_air;
extern double d_eau;

// =====================
// FONCTIONS
// =====================

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
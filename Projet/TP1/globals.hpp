#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/ImageBase.h"
#include "common/Transformation.hpp"
#include "common/Mesh.hpp"
#include "common/Node.hpp"

// =====================
// STRUCTS
// =====================

/* struct Mesh{
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
}; */

struct Ressort;

/* struct Node{
    Mesh* mesh;
    double volume;
    std::vector<Node*> enfants;
    std::vector<Node*> gravite;
    std::vector<Ressort*> ressort;
    
    Transformation transformation;
    
    GLuint textureID;
    int mode;
    glm::vec3 vitesse;
    double masse;
    
    Coeff coeff;
    
    Node();
    glm::mat4 computeMatTransformation();
}; */

struct Ressort {
    Node* autre;
    double raideur;
    double amortissement;
    double longueurRepos;
    Ressort(Node *autre, double raideur, double longueurRepos);
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
extern Mesh MeshCar;
extern Mesh MeshWheel;
extern Mesh MeshTerrain;

// scene graph
extern SceneGraph SceneCar;
extern Node NodeCar;
extern Node NodeFrontLeftWheel;
extern Node NodeFrontRightWheel;
extern Node NodeBackLeftWheel;
extern Node NodeBackRightWheel;

extern SceneGraph SceneTerrain;
extern Node NodeTerrain;

extern Ressort ressortSoleil;

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
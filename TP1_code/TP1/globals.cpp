#include "globals.hpp"
#include <glm/gtc/matrix_transform.hpp>

// fenêtre
GLFWwindow* window = nullptr;

// caméra
glm::vec3 camera_position = glm::vec3(0.0f, 0.8f, 0.f);
glm::vec3 camera_target   = glm::vec3(1.f, 0.8f, 0.f);
glm::vec3 camera_up       = glm::vec3(0.f,1.0f,0.f);
glm::vec3 camera_front    = glm::normalize(camera_target - camera_position);
glm::vec3 macaqueTranslate = glm::vec3(0.f);

// paramètres caméra
float cameraSpeed = 0.0f;
float angle = 0.0f;
float zoom = 1.0f;
float theta = 1.0f;
glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));

// temps
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// terrain
int longueur = 512;
int hauteur = 512;
ImageBase heightMap;

// meshes
Mesh terrain;
Mesh eau;
Mesh boat;
Mesh soleil;
Mesh lune;
Mesh terre;
Mesh mars;
Mesh macaque;
Mesh macaqueLow;
Mesh cube;

// scene
SceneGraph Planete;
SceneGraph Cube;
SceneGraph Soleil;
Node NodeSoleil;
Node NodeLune;
Node NodeTerrain;
Node NodeEau;
Node NodeTerre;
Node NodeMars;
Node NodeMacaque;
Node NodeCube;

Ressort ressortSoleil(&NodeSoleil, 1., 30.f);


// autres
int mode = 0;
bool mouvement = false;
double d_air = 1;
double d_eau = 1000;

// =====================
// NODE
// =====================


Coeff::Coeff(){
    friction_statique = 0.4;
    friction_cinetique = 0.3;
    rebond = 0.5;
}

Coeff::Coeff(double fs, double fc, double r){
    friction_statique = fs;
    friction_cinetique = fc;
    rebond = r;
}

Ressort::Ressort(Node* autre, double raideur, double longueurRepos){
    this->autre = autre;
    this->raideur = raideur;
    this->longueurRepos = longueurRepos;
    this->amortissement = 0.2;
}

Node::Node(){
    mesh = nullptr;
    volume = 1;
    mode = 0;
    textureID = 0;
    scale = glm::vec3(1,1,1);
    rotation = glm::vec3(0,0,0);
    translation = glm::vec3(0,0,0);
    vitesse = glm::vec3(0);
    masse = 500.f;
    coeff = Coeff();
}

glm::mat4 Node::computeMatTransformation(){
    return glm::translate(glm::mat4(1.0f), translation)
        * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0))
        * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1))
        * glm::scale(glm::mat4(1.0f), scale);
}
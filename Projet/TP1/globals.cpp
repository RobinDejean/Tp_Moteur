#include "globals.hpp"
#include <glm/gtc/matrix_transform.hpp>

// fenêtre
GLFWwindow* window = nullptr;

// caméra
glm::vec3 camera_position = glm::vec3(0.0f, 0.8f, -10.f);
glm::vec3 camera_target   = glm::vec3(1.f, 0.8f, 0.f);
glm::vec3 camera_up       = glm::vec3(0.f,1.0f,0.f);
glm::vec3 camera_front    = glm::normalize(camera_target - camera_position);
glm::vec3 macaqueTranslate = glm::vec3(0.f);

Camera camera;

// paramètres caméra
float cameraSpeed = 0.0f;
float angle = 0.0f;
float zoom = 1.0f;
float theta = 1.0f;
glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));

//volant
float currentSteeringAngle = 0.0f;
float maxSteering = 0.3f;
const float steeringSpeed = 2.0f;

//
float acceleration = 0.0f;
float freinage = 0.0f;

// temps
double deltaTime = 0.0f;
double lastFrame = 0.0f;

// variables
float tailleCar = 1.0;
float rayonRoue = 0.3;
float widthRoue = 0.5;
std::vector<double> bestMapTime;

// terrain
int longueur = 512;
int hauteur = 512;
ImageBase heightMap;

// meshes
Mesh MeshCar;
Mesh MeshWheel;
Mesh MeshTerrain;

//textures
GLuint TextureIDRoad;
GLuint TextureIDTerre;
GLuint TextureIDGlace;
GLuint TextureIDGrass;

GLuint TextureIDRoadNormal;
GLuint TextureIDTerreNormal;
GLuint TextureIDGlaceNormal;
GLuint TextureIDGrassNormal;

GLuint TextureIDRoadRoughness;
GLuint TextureIDTerreRoughness;
GLuint TextureIDGlaceRoughness;
GLuint TextureIDGrassRoughness;

GLuint normalMapLoc;
GLuint roughnessMapLoc;



// scene
SceneGraph SceneCar;
Node NodeCar;
Node NodeFrontLeftWheel;
Node NodeFrontRightWheel;
Node NodeBackLeftWheel;
Node NodeBackRightWheel;
Node NodeCheckpoint;
Node NodeFinish;
Mesh CheckpointMesh;

SceneGraph SceneTerrain;
Node NodeTerrain;

Car car = Car(&NodeCar, 1000.);

//Ressort ressortSoleil(&NodeSoleil, 1., 30.f);

// MAP
float blockSize = 20.f;
int mapWidth = 20;
int mapHeight = 20;
int mapDepth = 20;
Map map(mapWidth, mapHeight, mapDepth);

Mesh MeshRoadLine;
Node NodeRoadLine;
Node NodeRoadLine90;
Node NodeDirtLine;
Node NodeDirtLine90;
Node NodeGrassLine;
Node NodeGrassLine90;

Mesh MeshRoadCorner;
Node NodeRoadCorner;
Node NodeRoadCorner90;
Node NodeRoadCorner180;
Node NodeRoadCorner270;
Node NodeDirtCorner;
Node NodeDirtCorner90;
Node NodeDirtCorner180;
Node NodeDirtCorner270;
Node NodeGrassCorner;
Node NodeGrassCorner90;
Node NodeGrassCorner180;
Node NodeGrassCorner270;

Mesh MeshRoadLinePenche;
Node NodeRoadLinePenche;
Node NodeRoadLinePenche90;
Node NodeRoadLinePenche180;
Node NodeRoadLinePenche270;
Node NodeDirtLinePenche;
Node NodeDirtLinePenche90;
Node NodeDirtLinePenche180;
Node NodeDirtLinePenche270;
Node NodeGrassLinePenche;
Node NodeGrassLinePenche90;
Node NodeGrassLinePenche180;
Node NodeGrassLinePenche270;

Mesh MeshRoadQuarterPipe;
Node NodeRoadQuarterPipe;
Node NodeRoadQuarterPipeUp;
Node NodeRoadQuarterPipe90;
Node NodeRoadQuarterPipeUp90;
Node NodeRoadQuarterPipe180;
Node NodeRoadQuarterPipeUp180;
Node NodeRoadQuarterPipe270;
Node NodeRoadQuarterPipeUp270;

// OBSTACLES

Mesh MeshPillar;
Node NodePillar;

glm::vec3 centreMoyenRoues;
glm::vec3 offsetChassis;
std::vector<glm::vec3> positionsInitialesRoues;

// autres
int mode = 0;
bool mouvement = false;
double d_air = 1;
double d_eau = 1000;

// =====================
// NODE
// =====================


/* Coeff::Coeff(){
    friction_statique = 0.4;
    friction_cinetique = 0.3;
    rebond = 0.5;
}

Coeff::Coeff(double fs, double fc, double r){
    friction_statique = fs;
    friction_cinetique = fc;
    rebond = r;
} */

Ressort::Ressort(Node* autre, double raideur, double longueurRepos){
    this->autre = autre;
    this->raideur = raideur;
    this->longueurRepos = longueurRepos;
    this->amortissement = 0.2;
}

/* Node::Node(){
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
} */
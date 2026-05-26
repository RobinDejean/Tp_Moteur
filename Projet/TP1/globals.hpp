#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/ImageBase.h"
#include "common/Transformation.hpp"
#include "common/Mesh.hpp"
#include "common/Node.hpp"
#include "common/Car.hpp"
#include "common/Map.hpp"
#include "common/camera/Camera.hpp"


enum class TypeSurface {
    ROUTE,
    TERRE,
    GAZON,
    GLACE
};

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
            
            class Camera;
            extern Camera camera;

            extern glm::vec3 centreMoyenRoues;
            extern glm::vec3 offsetChassis;

            extern std::vector<glm::vec3> positionsInitialesRoues;
            
            // paramètres caméra
            extern float cameraSpeed;
            extern float angle;
            extern float zoom;
            extern float theta;
            extern glm::mat4 rotationY;
            
            //volant
            extern float currentSteeringAngle;
            extern float maxSteering;
            extern const float steeringSpeed;
            
//
extern float acceleration;
extern float freinage;

// temps
extern double deltaTime;
extern double lastFrame;

//variables
extern float tailleCar;
extern float rayonRoue;
extern float widthRoue;
extern std::vector<double> bestMapTime;

// terrain
extern int longueur;
extern int hauteur;
extern ImageBase heightMap;

// meshes
extern Mesh MeshCar;
extern Mesh MeshWheel;
extern Mesh MeshTerrain;

//textures
extern GLuint TextureIDRoad;
extern GLuint TextureIDTerre;
extern GLuint TextureIDGlace;
extern GLuint TextureIDGrass;

extern GLuint TextureIDRoadNormal;
extern GLuint TextureIDTerreNormal;
extern GLuint TextureIDGlaceNormal;
extern GLuint TextureIDGrassNormal;

extern GLuint TextureIDRoadRoughness;
extern GLuint TextureIDTerreRoughness;
extern GLuint TextureIDGlaceRoughness;
extern GLuint TextureIDGrassRoughness;

extern GLuint normalMapLoc;
extern GLuint roughnessMapLoc;

// scene graph
extern SceneGraph SceneCar;
extern Node NodeCar;
extern Node NodeFrontLeftWheel;
extern Node NodeFrontRightWheel;
extern Node NodeBackLeftWheel;
extern Node NodeBackRightWheel;

extern Node NodeCheckpoint;
extern Node NodeFinish;
extern Mesh CheckpointMesh;

extern SceneGraph SceneTerrain;
extern Node NodeTerrain;

extern Ressort ressortSoleil;

extern Car car;

// MAP
extern float blockSize;
extern int mapWidth;
extern int mapHeight;
extern int mapDepth;
extern Map map;
extern Map map2;

extern Mesh MeshRoadLine;
extern Node NodeRoadLine;
extern Node NodeRoadLine90;
extern Node NodeDirtLine;
extern Node NodeDirtLine90;
extern Node NodeGrassLine;
extern Node NodeGrassLine90;

extern Mesh MeshRoadCorner;
extern Node NodeRoadCorner;
extern Node NodeRoadCorner90;
extern Node NodeRoadCorner180;
extern Node NodeRoadCorner270;
extern Node NodeDirtCorner;
extern Node NodeDirtCorner90;
extern Node NodeDirtCorner180;
extern Node NodeDirtCorner270;
extern Node NodeGrassCorner;
extern Node NodeGrassCorner90;
extern Node NodeGrassCorner180;
extern Node NodeGrassCorner270;

extern Mesh MeshRoadLinePenche;
extern Node NodeRoadLinePenche;
extern Node NodeRoadLinePenche90;
extern Node NodeRoadLinePenche180;
extern Node NodeRoadLinePenche270;
extern Node NodeDirtLinePenche;
extern Node NodeDirtLinePenche90;
extern Node NodeDirtLinePenche180;
extern Node NodeDirtLinePenche270;
extern Node NodeGrassLinePenche;
extern Node NodeGrassLinePenche90;
extern Node NodeGrassLinePenche180;
extern Node NodeGrassLinePenche270;

extern Mesh MeshRoadQuarterPipe;
extern Node NodeRoadQuarterPipe;
extern Node NodeRoadQuarterPipeUp;
extern Node NodeRoadQuarterPipe90;
extern Node NodeRoadQuarterPipeUp90;
extern Node NodeRoadQuarterPipe180;
extern Node NodeRoadQuarterPipeUp180;
extern Node NodeRoadQuarterPipe270;
extern Node NodeRoadQuarterPipeUp270;

//OBSTACLES
extern Mesh MeshPillar;
extern Node NodePillar;

// autres
extern int mode;
extern bool mouvement;
extern double d_air;
extern double d_eau;

// =====================
// FONCTIONS
// =====================

inline GLuint* getTexture(TypeSurface surface) {
    switch (surface) {
        case TypeSurface::ROUTE: return &TextureIDRoad;
        case TypeSurface::TERRE: return &TextureIDTerre;
        case TypeSurface::GAZON: return &TextureIDGrass;
        case TypeSurface::GLACE: return &TextureIDGlace;
        default: return &TextureIDRoad;
    }
}

inline GLuint* getTextureNormal(TypeSurface surface) {
    switch (surface) {
        case TypeSurface::ROUTE: return &TextureIDRoadNormal;
        case TypeSurface::TERRE: return &TextureIDTerreNormal;
        case TypeSurface::GAZON: return &TextureIDGrassNormal;
        case TypeSurface::GLACE: return &TextureIDGlaceNormal;
        default: return &TextureIDRoadNormal;
    }
}

inline GLuint* getTextureRoughness(TypeSurface surface) {
    switch (surface) {
        case TypeSurface::ROUTE: return &TextureIDRoadRoughness;
        case TypeSurface::TERRE: return &TextureIDTerreRoughness;
        case TypeSurface::GAZON: return &TextureIDGrassRoughness;
        case TypeSurface::GLACE: return &TextureIDGlaceRoughness;
        default: return &TextureIDRoadRoughness;
    }
}


inline float getAdherenceSol(TypeSurface surface) {
    switch (surface) {
        case TypeSurface::ROUTE: return 1.0f;
        case TypeSurface::TERRE: return 0.8f;
        case TypeSurface::GAZON: return 0.5f;
        case TypeSurface::GLACE: return 0.2f;
        default: return 1.0f;
    }
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
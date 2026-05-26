// Include standard headers
// #include <glm/ext/vector_float3.hpp>
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
//#include <common/ImageBase.cpp>
#include "common/Mesh.hpp"
#include "common/Node.hpp"
#include "common/Transformation.hpp"
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "globals.hpp"
#include "fonctions.hpp"
#include "common/texture.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <thread>
#include <chrono>

int main() {
    // 1. INITIALISATION DE GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur lors de l'initialisation de GLFW\n";
        return -1;
    }
    bool start = false;
    // initialisation
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creation fenetre
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window); 

    // 3. INITIALISATION DE GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }


    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 4. CONFIGURATION GLOBALE OPENGL


    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull triangles which normal is not towards the camera
    glDisable(GL_CULL_FACE);

    // charger les shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");


    //heightMap.load("Assets/Heightmap_Mountain.pgm");
    //Planete.racine = &NodeTerrain;

    std::map<std::string, Mesh> maVoiture;
    openOBJ("Assets/carv6.obj", maVoiture);

    // Maintenant tu as accès à tes pièces individuellement !
    Mesh& carrosserie = maVoiture["Skin_Mesh.004"];
    Mesh& roueAvantGauche = maVoiture["WheelFron0_Mesh.003"];
    Mesh& roueAvantDroite = maVoiture["WheelFront_Mesh"];
    Mesh& roueArriereGauche = maVoiture["WheelBackL_Mesh.002"];
    Mesh& roueArriereDroite = maVoiture["WheelBackR_Mesh.001"];
    
    glm::vec3 centreChassis = calculerCentreMesh(maVoiture["Skin_Mesh.004"]);
    glm::vec3 centreAV_G = calculerCentreMesh(maVoiture["WheelFron0_Mesh.003"]);
    glm::vec3 centreAV_D = calculerCentreMesh(maVoiture["WheelFront_Mesh"]);
    glm::vec3 centreAR_G = calculerCentreMesh(maVoiture["WheelBackL_Mesh.002"]);
    glm::vec3 centreAR_D = calculerCentreMesh(maVoiture["WheelBackR_Mesh.001"]);

    recentrerMesh(roueAvantGauche, centreAV_G);
    recentrerMesh(roueAvantDroite, centreAV_D);
    recentrerMesh(roueArriereGauche, centreAR_G);
    recentrerMesh(roueArriereDroite, centreAR_D);
    recentrerMesh(carrosserie, centreChassis);
    
    carrosserie.setupMesh();
    roueAvantGauche.setupMesh();
    roueAvantDroite.setupMesh();
    roueArriereGauche.setupMesh();
    roueArriereDroite.setupMesh();

    positionsInitialesRoues.resize(4);
// 3. On déduit les positions locales et on les applique aux Transformations

    positionsInitialesRoues[0] = centreAV_G - centreChassis;
    positionsInitialesRoues[1] = centreAV_D - centreChassis;
    positionsInitialesRoues[2] = centreAR_G - centreChassis;
    positionsInitialesRoues[3] = centreAR_D - centreChassis;

    for (int i = 0; i < 4; i++)
        std::cout << "Position initiale roue " << i << " : " << positionsInitialesRoues[i].x << ", " << positionsInitialesRoues[i].y << ", " << positionsInitialesRoues[i].z << std::endl;

    // 4. On place les noeuds pour la toute première frame du jeu
    NodeFrontLeftWheel.transformation.setTranslation(positionsInitialesRoues[0]);
    NodeFrontRightWheel.transformation.setTranslation(positionsInitialesRoues[1]);
    NodeBackLeftWheel.transformation.setTranslation(positionsInitialesRoues[2]);
    NodeBackRightWheel.transformation.setTranslation(positionsInitialesRoues[3]);

    /* glm::mat4 rotationCorrection = glm::rotate(
    glm::mat4(1.0f), 
    glm::radians(180.0f), 
    glm::vec3(1.0f, 0.0f, 0.0f) // L'axe de rotation (X ici, essaie Z (0,0,1) si elle vrille)
    ); */

    // 2. On applique cette matrice à ton châssis
    //NodeCar.transformation.setRotationFromMatrix(rotationCorrection);

    centreMoyenRoues = (centreAV_G + centreAV_D + centreAR_G + centreAR_D) / 4.0f;
    offsetChassis = centreChassis - centreMoyenRoues;

    SceneCar.racine = &NodeCar;
    SceneTerrain.racine = &NodeTerrain;

    TextureIDRoad = loadDDS("Assets/roadTexture/asphalt_02_diff_4k.dds");
    TextureIDGrass = loadDDS("Assets/grassTexture/brown_mud_leaves_01_diff_4k.dds");
    TextureIDGlace = loadDDS("Assets/snowrocks.dds");
    TextureIDTerre = loadDDS("Assets/dirtTexture/dirt_floor_diff_4k.dds");
    std::cout << "Textures chargées : " << std::endl;

    TextureIDRoadNormal = loadDDS("Assets/roadTexture/asphalt_02_nor_gl_4k.dds");
    TextureIDGrassNormal = loadDDS("Assets/grassTexture/brown_mud_leaves_01_nor_gl_4k.dds");
    TextureIDTerreNormal = loadDDS("Assets/dirtTexture/dirt_floor_nor_gl_4k.dds");
    std::cout << "Textures normales chargées : " << std::endl;

    TextureIDRoadRoughness = loadDDS("Assets/roadTexture/asphalt_02_rough_4k.dds");
    TextureIDGrassRoughness = loadDDS("Assets/grassTexture/brown_mud_leaves_01_rough_4k.dds");
    TextureIDTerreRoughness = loadDDS("Assets/dirtTexture/dirt_floor_rough_4k.dds");
    std::cout << "Textures rugosité chargées : " << std::endl;

    std::cout << "Textures chargées : " << std::endl;
    
    //associer mesh au noeud
    //NodeSoleil.setMesh(&soleil);
    NodeCar.setMesh(&carrosserie);
    NodeFrontLeftWheel.setMesh(&roueAvantGauche);
    NodeFrontRightWheel.setMesh(&roueAvantDroite);
    NodeBackLeftWheel.setMesh(&roueArriereGauche);
    NodeBackRightWheel.setMesh(&roueArriereDroite);
    //NodeCar.transformation.setScale(glm::vec3(30.f));
    /* NodeFrontLeftWheel.transformation.setScale(glm::vec3(5.f));;
    NodeFrontRightWheel.transformation.setScale(glm::vec3(5.f));
    NodeBackLeftWheel.transformation.setScale(glm::vec3(5.f));
    NodeBackRightWheel.transformation.setScale(glm::vec3(5.f)); */
    
    // ---------------------------------------------------- MAP NODES ----------------------------------------------------
    // LINE
    
    NodeRoadLine = Node(&MeshRoadLine, TypeSurface::ROUTE);
    NodeRoadLine90 = Node(&MeshRoadLine, TypeSurface::ROUTE);
    NodeRoadLine90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    
    NodeDirtLine = Node(&MeshRoadLine, TypeSurface::TERRE);
    NodeDirtLine90 = Node(&MeshRoadLine, TypeSurface::TERRE);
    NodeDirtLine90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));

    NodeGrassLine = Node(&MeshRoadLine, TypeSurface::GAZON);
    NodeGrassLine90 = Node(&MeshRoadLine, TypeSurface::GAZON);
    NodeGrassLine90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    
    //QUARTERPIPE EST CENTRE SUR (0,0) donc il faut faire + Blocksize / 2 en y et il est en 90 de base
    
    NodeRoadQuarterPipe = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipe.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));
    NodeRoadQuarterPipeUp = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipeUp.transformation.setEulerAngles(glm::vec3(glm::radians(-90.f), 0., 0.));
    NodeRoadQuarterPipeUp.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));

    NodeRoadQuarterPipe90 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipe90.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));
    NodeRoadQuarterPipeUp90 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipeUp90.transformation.setEulerAngles(glm::vec3(glm::radians(-90.f), glm::radians(90.f), 0.));
    NodeRoadQuarterPipeUp90.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));

    NodeRoadQuarterPipe180 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipe180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeRoadQuarterPipe180.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));
    NodeRoadQuarterPipeUp180 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipeUp180.transformation.setEulerAngles(glm::vec3(glm::radians(-90.f), glm::radians(180.f), 0.));
    NodeRoadQuarterPipeUp180.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));

    NodeRoadQuarterPipe270 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipe270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));
    NodeRoadQuarterPipe270.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));
    NodeRoadQuarterPipeUp270 = Node(&MeshRoadQuarterPipe, TypeSurface::ROUTE);
    NodeRoadQuarterPipeUp270.transformation.setEulerAngles(glm::vec3(glm::radians(-90.f), glm::radians(270.f), 0.));
    NodeRoadQuarterPipeUp270.transformation.setTranslation(glm::vec3(0., blockSize / 2.0f, 0.));

    
    //CORNER

    NodeRoadCorner = Node(&MeshRoadCorner, TypeSurface::ROUTE);
    NodeRoadCorner90 = Node(&MeshRoadCorner, TypeSurface::ROUTE);
    NodeRoadCorner90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    NodeRoadCorner180 = Node(&MeshRoadCorner, TypeSurface::ROUTE);
    NodeRoadCorner180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeRoadCorner270 = Node(&MeshRoadCorner, TypeSurface::ROUTE);
    NodeRoadCorner270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));

    NodeDirtCorner = Node(&MeshRoadCorner, TypeSurface::TERRE);
    NodeDirtCorner90 = Node(&MeshRoadCorner, TypeSurface::TERRE);
    NodeDirtCorner90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    NodeDirtCorner180 = Node(&MeshRoadCorner, TypeSurface::TERRE);
    NodeDirtCorner180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeDirtCorner270 = Node(&MeshRoadCorner, TypeSurface::TERRE);
    NodeDirtCorner270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));
    
    //LINE PENCHÉE

    NodeRoadLinePenche = Node(&MeshRoadLinePenche, TypeSurface::ROUTE);
    NodeRoadLinePenche90 = Node(&MeshRoadLinePenche, TypeSurface::ROUTE);
    NodeRoadLinePenche90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    NodeRoadLinePenche180 = Node(&MeshRoadLinePenche, TypeSurface::ROUTE);
    NodeRoadLinePenche180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeRoadLinePenche270 = Node(&MeshRoadLinePenche, TypeSurface::ROUTE);
    NodeRoadLinePenche270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));

    NodeDirtLinePenche = Node(&MeshRoadLinePenche, TypeSurface::TERRE);
    NodeDirtLinePenche90 = Node(&MeshRoadLinePenche, TypeSurface::TERRE);
    NodeDirtLinePenche90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    NodeDirtLinePenche180 = Node(&MeshRoadLinePenche, TypeSurface::TERRE);
    NodeDirtLinePenche180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeDirtLinePenche270 = Node(&MeshRoadLinePenche, TypeSurface::TERRE);
    NodeDirtLinePenche270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));

    NodeGrassLinePenche = Node(&MeshRoadLinePenche, TypeSurface::GAZON);
    NodeGrassLinePenche90 = Node(&MeshRoadLinePenche, TypeSurface::GAZON);
    NodeGrassLinePenche90.transformation.setEulerAngles(glm::vec3(0., glm::radians(90.f), 0.));
    NodeGrassLinePenche180 = Node(&MeshRoadLinePenche, TypeSurface::GAZON);
    NodeGrassLinePenche180.transformation.setEulerAngles(glm::vec3(0., glm::radians(180.f), 0.));
    NodeGrassLinePenche270 = Node(&MeshRoadLinePenche, TypeSurface::GAZON);
    NodeGrassLinePenche270.transformation.setEulerAngles(glm::vec3(0., glm::radians(270.f), 0.));
    
    //OBSTACLES

    NodePillar.setMesh(&MeshPillar);
    
    //NodeTerrain.setMesh(&MeshTerrain);
    
    
    //ajouter les enfants
    //NodeTerrain.enfants.push_back(&NodeMacaque);
    NodeCar.addEnfant(&NodeFrontLeftWheel);
    NodeCar.addEnfant(&NodeFrontRightWheel);
    NodeCar.addEnfant(&NodeBackLeftWheel);
    NodeCar.addEnfant(&NodeBackRightWheel);
    
    //creer les mesh
    //mars.sphere(0.075,20);
    /* openOBJ("Assets/Macaque.obj", macaque);
    openOBJ("Assets/MacaqueLow.obj", macaqueLow); */
    //MeshCar.car(tailleCar);
    //MeshWheel.createWheel(rayonRoue,widthRoue, 32);
    //MeshTerrain.worldPenche(500,500,0);
    MeshRoadLine.road_line();
    MeshRoadCorner.road_corner();
    MeshRoadLinePenche.road_line_penche();
    MeshRoadQuarterPipe.road_quarterpipe();
    CheckpointMesh.checkpoint();
    NodeCheckpoint.setMesh(&CheckpointMesh);
    NodeCheckpoint.setMode(2);
    NodeFinish.setMesh(&CheckpointMesh);
    NodeFinish.setMode(3);
    
    MeshPillar.pillar();
    //NodeTerrain.transformation.setScale(100.);
    
    // TEXTURES
    //charge la texture
    GLuint TextureIDRock = loadDDS("Assets/rock.dds");
    // recup l'emplacement du shader
    GLuint TextureUniformRock = glGetUniformLocation(programID,"myRockSampler");
    
    GLuint TextureIDGrass = loadDDS("Assets/grass.dds");
    GLuint TextureUniformGrass = glGetUniformLocation(programID,"myGrassSampler");
    GLuint TextureIDSnowRocks = loadDDS("Assets/snowrocks.dds");
    GLuint TextureUniformSnowRocks = glGetUniformLocation(programID,"mySnowRocksSampler");
    
    
    //GLuint TextureIDTerre = loadDDS("Assets/terre.dds");
    GLuint TextureIDSoleil = loadDDS("Assets/soleil.dds");
    GLuint TextureIDLune = loadDDS("Assets/lune.dds");
    GLuint TextureIDMars = loadDDS("Assets/mars.dds");
    GLuint TextureIDMacaque = loadDDS("Assets/Macaque_texture.dds");
    GLuint TextureVoiture = loadDDS("Assets/textureVoiture2.dds");
    GLuint TextureRoue = loadDDS("Assets/textureRoue.dds");
    
    glUseProgram(programID);
     // active le slot de texture 1 = GL_TEXTURE1
    glActiveTexture(GL_TEXTURE1);
    //on bind notre texture
    glBindTexture(GL_TEXTURE_2D, TextureIDRock);
    // on dit que slot 0 = notre texture
    glUniform1i(TextureUniformRock, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureIDGrass);
    glUniform1i(TextureUniformGrass, 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, TextureIDSnowRocks);
    glUniform1i(TextureUniformSnowRocks, 3);

    // Match the texture units used when binding in Map::render
    glUniform1i(glGetUniformLocation(programID, "myDiffuseSampler"), 4);    // GL_TEXTURE4
    glUniform1i(glGetUniformLocation(programID, "myNormalSampler"), 7);     // GL_TEXTURE7
    glUniform1i(glGetUniformLocation(programID, "myRoughnessSampler"), 6);  // GL_TEXTURE6
    glUniform1i(glGetUniformLocation(programID, "myMetallicSampler"), 5);   // GL_TEXTURE5 (if used)
    normalMapLoc = glGetUniformLocation(programID, "useNormalMap");
    roughnessMapLoc = glGetUniformLocation(programID, "useRoughnessMap");

    NodeTerrain.setMode(1);
    //NodeSoleil.setTextureID(TextureIDSoleil);
    NodeCar.setTextureID(&TextureVoiture);
    /* NodeCar.setTextureNormalID(&TextureIDRoadNormal);
    NodeCar.setTextureRoughnessID(&TextureIDRoadRoughness); */
    NodeFrontLeftWheel.setTextureID(&TextureRoue);
    /* NodeFrontLeftWheel.setTextureNormalID(&TextureIDRoadNormal);
    NodeFrontLeftWheel.setTextureRoughnessID(&TextureIDRoadRoughness); */
    NodeFrontRightWheel.setTextureID(&TextureRoue);
    /* NodeFrontRightWheel.setTextureNormalID(&TextureIDRoadNormal);
    NodeFrontRightWheel.setTextureRoughnessID(&TextureIDRoadRoughness); */
    NodeBackLeftWheel.setTextureID(&TextureRoue);
    /* NodeBackLeftWheel.setTextureNormalID(&TextureIDRoadNormal);
    NodeBackLeftWheel.setTextureRoughnessID(&TextureIDRoadRoughness); */
    NodeBackRightWheel.setTextureID(&TextureRoue);
    /* NodeBackRightWheel.setTextureNormalID(&TextureIDRoadNormal);
    NodeBackRightWheel.setTextureRoughnessID(&TextureIDRoadRoughness); */

    /* NodeRoadLine.setTextureID(TextureIDRoad);
    NodeRoadLine90.setTextureID(TextureIDRoad);
    NodeRoadCorner.setTextureID(TextureIDRoad);
    NodeRoadLinePenche.setTextureID(TextureIDRoad);
    NodeRoadLinePenche90.setTextureID(TextureIDRoad);
    NodeRoadQuarterPipe90.setTextureID(TextureIDRoad);
    NodeRoadQuarterPipe.setTextureID(TextureIDRoad);
    NodeRoadQuarterPipeUp90.setTextureID(TextureIDRoad); */

    NodePillar.setTextureID(&TextureIDRock);
    /* NodePillar.setTextureNormalID(&TextureIDRoadNormal);
    NodePillar.setTextureRoughnessID(&TextureIDRoadRoughness); */

    //NodeCube.gravite.push_back(&NodeSoleil);
    //NodeCube.ressort.push_back(&ressortSoleil);
    // NodeCube.setVitesse(glm::vec3(1.f,0.f,0.f));
    // NodeSoleil.transformation.setScale(glm::vec3(10.,10.,10.));
    // NodeSoleil.transformation.setTranslation(glm::vec3(0.f, 50.f, 0.0f));
    // NodeSoleil.setMasse(10000000000.f);
    /* NodeBackLeftWheel.transformation.setTranslation(glm::vec3(0.2, 0., 1.));
    NodeBackRightWheel.transformation.setTranslation(glm::vec3(0.2, 0., 0.));
    NodeFrontLeftWheel.transformation.setTranslation(glm::vec3(1.8, 0., 1.));
    NodeFrontRightWheel.transformation.setTranslation(glm::vec3(1.8, 0., 0.)); */

    /* NodeBackLeftWheel.transformation.setTranslation(glm::vec3(0.05f,0.05f, 0.05f));
    NodeBackRightWheel.transformation.setTranslation(glm::vec3(-0.05f,-0.05f, -0.05f));
    NodeFrontLeftWheel.transformation.setTranslation(glm::vec3(-0.05f,-0.05f, 0.05f));
    NodeFrontRightWheel.transformation.setTranslation(glm::vec3(-0.05f,0.05f, -0.05f)); */

    // --------------------- MAP ---------------------
    map.addNode(9, 10, 9, &NodeRoadLine);
    map.setStart(9, 10, 9, 0, &NodeCheckpoint);
    map.addNode(9, 10, 10, &NodeRoadLine);
    map.addNode(9, 10, 11, &NodeRoadLine);
    map.addNode(9, 10, 12, &NodeRoadLinePenche);

    map.addNode(9, 10, 15, &NodeRoadQuarterPipe);
    map.addNode(9, 11, 15, &NodeRoadQuarterPipeUp);
    map.addCheckPoint(9, 11, 15, 0, &NodeCheckpoint);
    map.addNode(9, 11, 14, &NodeRoadQuarterPipeUp180);
    map.addNode(9, 9, 14, &NodeRoadQuarterPipe180);
    map.addNode(9, 9, 15, &NodeDirtLine);
    map.addNode(9, 9, 16, &NodeDirtCorner);
    map.addNode(8, 9, 16, &NodeDirtLine90);
    map.addNode(7, 9, 16, &NodeDirtLine90);
    map.addNode(6, 9, 16, &NodeDirtLinePenche270);

    map.addNode(3, 9, 16, &NodeGrassLine90);
    map.addNode(2, 9, 16, &NodeGrassLine90);
    
    map.setFinish(3, 9, 16, 0, &NodeFinish);

    // obstacles

    map.addNode(9, 10, 11, &NodePillar);


    
    camera.init();
    camera.setTarget(&NodeCar);
    NodeCar.transformation.setTranslation(glm::vec3(-10.f, 10.f, -10.f));
    NodeCar.transformation.addEulerAngles(glm::vec3(0.f, glm::radians(-90.f), 0.f));

    NodeCheckpoint.transformation.addEulerAngles(glm::vec3(glm::radians(-90.f), 0.f, 0.f));
    NodeCheckpoint.transformation.setTranslation(glm::vec3( 0.f, 0.f,blockSize/2.f));
    NodeFinish.transformation.addEulerAngles(glm::vec3(0.f, glm::radians(-90.f), 0.f));
    NodeFinish.transformation.setTranslation(glm::vec3(blockSize/2.f, 0.f, 0.f));
    /* glUseProgram(programID);
    GLuint TextureRouePBR = loadDDS("Assets/roadTexture/asphalt_02_diff_4k.dds");
    std::cout << "Texture ID : " << TextureRouePBR << std::endl;
    GLuint TextureRoueDisp = loadDDS("Assets/roadTexture/asphalt_02_disp_4k.dds");
    std::cout << "Texture ID : " << TextureRoueDisp << std::endl;
    GLuint TextureRoueRough = loadDDS("Assets/roadTexture/asphalt_02_rough_4k.dds");
    std::cout << "Texture ID : " << TextureRoueRough << std::endl;
    /* GLuint TextureRoueNormal = loadDDS("Assets/roadTexture/asphalt_02_nor_gl_4k.dds");
    std::cout << "Texture ID : " << TextureRoueNormal << std::endl;
    Texture textureRoueNormal = Texture("Assets/roadTexture/asphalt_02_nor_gl_4k.png");
    GLuint TextureRoueNormal = textureRoueNormal.getTextureId();
    std::cout << "Texture ID : " << TextureRoueNormal << std::endl; */


    /* GLuint TextureUniformDiffuse = glGetUniformLocation(programID,"myDiffuseSampler");
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, TextureRouePBR);
    glUniform1i(TextureUniformDiffuse, 4);

    GLuint TextureUniformDisp = glGetUniformLocation(programID,"myMetallicSampler");
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, TextureRoueDisp);
    glUniform1i(TextureUniformDisp, 5);

    GLuint TextureUniformRoughness = glGetUniformLocation(programID,"myRoughnessSampler");
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, TextureRoueRough);
    glUniform1i(TextureUniformRoughness, 6);

    GLuint TextureUniformNormal = glGetUniformLocation(programID,"myNormalSampler");
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, TextureRoueNormal);
    glUniform1i(TextureUniformNormal, 7); */

    bestMapTime = loadBestTime("best_times.csv");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup ImGui style (Style sombre par défaut, on le customisera après)
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330"); // Ajuste selon la version de ton GLSL

    std::cout << "Initialisation terminée, lancement de la boucle de rendu..." << std::endl;
    FILE * f = fopen("pos.csv", "w");
    const double targetFPS = 30.0;
    const double targetFrameTime = 1.0 / targetFPS;

    // 5. LA BOUCLE DE RENDU
    do{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //temps
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        if (deltaTime < targetFrameTime) {
            double sleepTime = targetFrameTime - deltaTime;
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
        //std::cout << "Delta time: " << deltaTime << " seconds" << std::endl;  
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //clavier
        processInput(window);
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float ratio = (float)width / (float)height;
        
        const int SUB_STEPS = 16;
        double sub_dt = deltaTime / SUB_STEPS;
        
        car.calculPosition(deltaTime, acceleration, freinage);
        for (int step = 0; step < SUB_STEPS; step++)
        {
            car.solver(sub_dt, map);
        }
        //std::cout << "Car position: " << NodeCar.transformation.getTranslation().x << ", " << NodeCar.transformation.getTranslation().y << ", " << NodeCar.transformation.getTranslation().z << std::endl;
        //std::cout << "Car velocity: " << glm::length(NodeCar.getVitesse()) << std::endl;
        camera.update(deltaTime, window);
        // matrice projection (perspective)
        glm::mat4 projectionMatrix = camera.getProjectionMatrix(); 
        glm::mat4 viewMatrix = camera.getViewMatrix();
        
        //matrice model
        glm::mat4 modelMatrix = glm::mat4();
        
        //mat viewProj
        glm::mat4 viewProj = projectionMatrix * viewMatrix;
        
        // matrice mvp
        glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
        
        //mettre a jour angle pour vitesse de roatation
        
        //updateHeight(macaqueTranslate);
        //updateMeshResolution(NodeMacaque);
        
        
        // choix des shaders a utiliser
        //glUseProgram(programID);
        
        // On trouve où est la variable "MVP" dans le Vertex Shader, et on lui envoie notre calcul
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        glUniform3fv(glGetUniformLocation(programID, "camPos"), 1, glm::value_ptr(camera.getPosition()));
        //glUniformMatrix4fv(glGetUniformLocation(programID,"MVP"),1,false ,glm::value_ptr(MVP));
        
        if (start && deltaTime > 1.f) {
            car.reset(currentSteeringAngle);
            map.reset();
            start = false;
        }
        
        //CAMERA
        /* camera_target = NodeCar.getCarCenter(1.);
        camera_position = camera_target + glm::vec3(NodeCar.getTransformation().getRotationMatrix() * glm::vec4(-6, 2., 0., 0.)); */
        
        SceneRender(SceneCar.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);
        //SceneRender(SceneTerrain.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);
        map.render(MatrixID, viewProj, programID);

        ImGui::SetNextWindowPos(ImVec2(width / 2.0f - 100, height - 120), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);

        ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBackground | 
                                   ImGuiWindowFlags_NoScrollbar;

        ImGui::Begin("Speedometer", nullptr, hudFlags);

        float vitesseKmh = glm::length(NodeCar.getVitesse()) * 3.6f;
        char texte[32];
        snprintf(texte, sizeof(texte), "%.0f", vitesseKmh);
        float windowWidth = ImGui::GetWindowSize().x;

        ImGui::SetWindowFontScale(2.5f);
        float textWidth = ImGui::CalcTextSize(texte).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%.0f", vitesseKmh);
        ImGui::SetWindowFontScale(1.0f);
        textWidth = ImGui::CalcTextSize("KM/H").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("KM/H");

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(width - 200, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 80), ImGuiCond_Always);
        ImGui::Begin("Chrono", nullptr, hudFlags);
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("TIME: %.3f", glfwGetTime() - map.getStartTime());
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(width - 250, 50), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, map.getCheckPoints().size() * 50 *2), ImGuiCond_Always);
        ImGui::Begin("Checkpoints", nullptr, hudFlags);
        ImGui::SetWindowFontScale(1.5f);
        std::vector<double> times = map.getTimes();
        for(int i = 0; i < times.size(); i++) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Checkpoint %d: %.3f s", i + 1, times[i]);
            std::cout << "Checkpoint " << i + 1 << ": " << times[i] << " seconds" << std::endl;
            std::cout << "Best time for checkpoint " << i + 1 << ": " << bestMapTime[i] << " seconds" << std::endl;
            std::cout << times[i] - bestMapTime[i] << std::endl;
            if (bestMapTime[i] < times[i]) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "+ %.3f s", times[i] - bestMapTime[i]);
            }else{
                ImGui::TextColored(ImVec4(.0f, .0f, 1.0f, 1.0f), " %.3f s", times[i] - bestMapTime[i]);
            }
        }
        double finishTime = map.getFinishTime();
        //std::cout << "Finish Time: " << finishTime << " seconds" << std::endl;
        if(finishTime > 0.0) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Finish Time: %.3f s", finishTime);
            if (bestMapTime.back() > finishTime) {
                saveBestTime(map, "best_times.csv");
                ImGui::TextColored(ImVec4(.0f, .0f, 1.0f, 1.0f), " %.3f s", finishTime - bestMapTime.back());
                //bestMapTime = loadBestTime("best_times.csv");
            }else{
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "+ %.3f s", finishTime - bestMapTime.back());
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* render(soleil);
        render(lune);
        render(terrain);*/
        
        // b. Échanger les buffers (Double Buffering : on affiche ce qu'on vient de dessiner)
        glfwSwapBuffers(window);
        
        // c. Récupérer les événements (clavier, souris, redimensionnement)
        glfwPollEvents();
    }while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );
    
    // 6. NETTOYAGE ET FERMETURE
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        camera.setThirdView(!camera.isThirdView());
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        car.resetToCheckpoint(map.getTimes(), currentSteeringAngle);
    }
        
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    glm::vec3 camera_right = glm::cross(camera_front, camera_up);
    //CNTROLES
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ){
        currentSteeringAngle -= steeringSpeed * deltaTime;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ){
        currentSteeringAngle += steeringSpeed * deltaTime;
    }
    else{
        if (currentSteeringAngle > 0.05f) currentSteeringAngle -= steeringSpeed * deltaTime;
        else if (currentSteeringAngle < -0.05f) currentSteeringAngle += steeringSpeed * deltaTime;
        else currentSteeringAngle = 0.0f;
    }
    //std::cout << "Puissance: " << car.getPuissance() << " Vitesse: " << glm::length(NodeCar.getVitesse()) << std::endl;
    maxSteering = 0.8f * ((car.getPuissance() - glm::length(NodeCar.getVitesse())) / car.getPuissance());
    currentSteeringAngle = glm::clamp(currentSteeringAngle, -maxSteering, maxSteering);
    //std::cout << "Max Steering: " << maxSteering << " Current Steering Angle: " << currentSteeringAngle << std::endl;
    NodeFrontLeftWheel.transformation.setEulerAngles(glm::vec3(0.f, currentSteeringAngle, 0.f));
    NodeFrontRightWheel.transformation.setEulerAngles(glm::vec3(0.f, currentSteeringAngle, 0.f));


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        acceleration = 1.0f;
    }else{
        acceleration = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        freinage = 1.0f;
    }
    else{
        freinage = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        car.reset(currentSteeringAngle);
        map.reset();
        bestMapTime = loadBestTime("best_times.csv");

    }
    glfwSetKeyCallback(window, keyCallback);
    //CAMERA



    /* if (mode == 0  ) {
        cameraSpeed = 2.5 * deltaTime;
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            
            camera_position += camera_front * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
        // Move backward
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            camera_position -= camera_front * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
        // Strafe right
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
            camera_position += camera_right * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
        // Strafe left
        if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
            camera_position -= camera_right * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));
            camera_front = glm::vec3(rotationY * glm::vec4(camera_front, 0.0));
            camera_target = camera_front + camera_position;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));
            camera_front = glm::vec3(glm::inverse(rotationY) * glm::vec4(camera_front, 0.0));
            camera_target = camera_front + camera_position;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS){ // up
            camera_position += camera_up * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){ // down
            camera_position -= camera_up * deltaTime * cameraSpeed*20;
            camera_target = camera_front + camera_position;
        }
    }
 */
    /* if (mode == 1 ) {
        rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(10*cameraSpeed), glm::vec3(0, 1, 0));
        camera_front = glm::normalize(camera_target - camera_position);
        camera_right = glm::normalize(glm::cross(camera_front, glm::vec3(0.f, 1.f, 0.f)));
        camera_up    = glm::normalize(glm::cross(camera_right, camera_front));
        camera_position = glm::vec3(rotationY * glm::vec4(camera_position,1.));
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
            cameraSpeed += 0.5 * deltaTime;
        }
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
            cameraSpeed -= 0.5 * deltaTime;
        }
    }
    if (mode == 2){
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            
            macaqueTranslate.z -= 0.1f * deltaTime ;
        }
        // Move backward
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            macaqueTranslate.z += 0.1f * deltaTime;
        }
        // Strafe right
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
            macaqueTranslate.x += 0.1f * deltaTime;
        }
        // Strafe left
        if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
            macaqueTranslate.x -= 0.1f * deltaTime;
        }
    } */

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
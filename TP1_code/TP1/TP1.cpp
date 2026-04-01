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
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "globals.hpp"
#include "fonctions.hpp"

int main() {
    // 1. INITIALISATION DE GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur lors de l'initialisation de GLFW\n";
        return -1;
    }

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


    heightMap.load("Assets/Heightmap_Mountain.pgm");
    Planete.racine = &NodeTerrain;
    Cube.racine = &NodeCube;
    Soleil.racine = &NodeSoleil;


    //associer mesh au noeud
    NodeSoleil.mesh = &soleil;
    NodeLune.mesh = &lune;
    NodeTerrain.mesh = &terrain;
    NodeEau.mesh = &eau;
    NodeTerre.mesh = &terre;
    NodeMars.mesh = &mars;
    NodeMacaque.mesh = &macaque;
    NodeCube.mesh = &cube;
    
    //ajouter les enfants
    //NodeTerrain.enfants.push_back(&NodeMacaque);
    //NodeTerrain.enfants.push_back(&NodeSoleil);
    NodeTerrain.enfants.push_back(&NodeEau);
    NodeSoleil.enfants.push_back(&NodeTerre);
    NodeTerre.enfants.push_back(&NodeLune);
    NodeSoleil.enfants.push_back(&NodeMars);
    //NodeTerrain.enfants.push_back(&NodeCube);

    
    float angleSoleil = 0.0f;
    float angleTerre  = 0.0f;
    float angleLune   = 0.0f;

    //creer les mesh
    sphere(mars,0.075,20);
    sphere(soleil,0.2,20);
    sphere(lune,0.05,20);
    sphere(terre,0.1,20);
    world(terrain);
    worldFlat(eau);
    openOBJ("Assets/Macaque.obj", macaque);
    openOBJ("Assets/MacaqueLow.obj", macaqueLow);

    setupCube(cube,1.);

    //setup des mesh
    setupMesh(soleil);
    setupMesh(lune);
    setupMesh(terre);
    setupMesh(terrain);
    setupMesh(eau);
    setupMesh(mars);
    setupMesh(macaque);
    setupMesh(macaqueLow);
    setupMesh(cube);


    // TEXTURES
    //charge la texture
    GLuint TextureIDRock = loadDDS("Assets/rock.dds");
    // recup l'emplacement du shader
    GLuint TextureUniformRock = glGetUniformLocation(programID,"myRockSampler");

    GLuint TextureIDGrass = loadDDS("Assets/grass.dds");
    GLuint TextureUniformGrass = glGetUniformLocation(programID,"myGrassSampler");
    GLuint TextureIDSnowRocks = loadDDS("Assets/snowrocks.dds");
    GLuint TextureUniformSnowRocks = glGetUniformLocation(programID,"mySnowRocksSampler");

    GLuint TextureIDTerre = loadDDS("Assets/terre.dds");
    GLuint TextureIDSoleil = loadDDS("Assets/soleil.dds");
    GLuint TextureIDLune = loadDDS("Assets/lune.dds");
    GLuint TextureIDMars = loadDDS("Assets/mars.dds");
    GLuint TextureIDMacaque = loadDDS("Assets/Macaque_texture.dds");
    
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

    glUniform1i(glGetUniformLocation(programID, "myPlaneteSampler"), 0);

    NodeTerrain.mode = 1;
    NodeEau.mode = 2;
    NodeCube.mode = 1;
    NodeSoleil.mode = 0;
    NodeTerre.mode = 0;
    NodeLune.mode = 0;
    NodeMars.mode = 0;
    NodeMacaque.mode = 0;
    NodeSoleil.textureID = TextureIDSoleil;
    NodeTerre.textureID = TextureIDTerre;
    NodeLune.textureID = TextureIDLune;
    NodeMars.textureID = TextureIDMars;
    NodeMacaque.textureID = TextureIDMacaque;
    NodeCube.textureID = TextureIDLune;

    NodeTerrain.scale = glm::vec3(10.,1.,10.);
    NodeCube.scale = glm::vec3(0.05,0.05,0.05);
    //NodeCube.gravite.push_back(&NodeSoleil);
    //NodeCube.ressort.push_back(&ressortSoleil);
    NodeCube.vitesse = glm::vec3(1.f,0.f,0.f);
    NodeSoleil.scale = glm::vec3(10.,10.,10.);
    NodeSoleil.translation = glm::vec3(0.f, 50.f, 0.0f);
    NodeSoleil.masse = 10000000000.f;


    FILE * f = fopen("pos.csv", "w");
    // 5. LA BOUCLE DE RENDU
    do{
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //temps
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //clavier
        processInput(window);
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float ratio = (float)width / (float)height;


        // matrice projection (perspective)
        glm::mat4 projectionMatrix = glm::perspective (glm::radians (45.f), ratio, 0.1f, 1000.f);

        // matrice view
        glm::mat4 viewMatrix = glm::lookAt(
            camera_position,
            camera_target,
            camera_up
        );

        //matrice model
        glm::mat4 modelMatrix = glm::mat4();

        //mat viewProj
        glm::mat4 viewProj = projectionMatrix * viewMatrix;

        // matrice mvp
        glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

        //mettre a jour angle pour vitesse de roatation

        angleSoleil += 1.0f * deltaTime;
        angleTerre  += 0.5f * deltaTime;
        angleLune   += 3.0f * deltaTime;

        //updateHeight(macaqueTranslate);
        //updateMeshResolution(NodeMacaque);

        if (mouvement){
            updatePos(NodeCube);
            collisionTerrain(NodeCube);
            fprintf(f,"%f %f %f %f %f %f \n", NodeCube.translation.x, NodeCube.translation.y, NodeCube.translation.z, NodeCube.vitesse.x, NodeCube.vitesse.y, NodeCube.vitesse.z);
            camera_target = NodeCube.translation;
            camera_position = NodeCube.translation + glm::vec3(0.,100.,100.)  * NodeCube.scale.x;
        }

        

        // NodeMacaque.transformation = glm::translate(glm::mat4(1.0f), macaqueTranslate)
        //                                 * glm::scale(glm::mat4(1.0), glm::vec3(0.03,0.03, -0.03));

        //NodeCube.transformation = glm::translate(glm::mat4(1.0f), macaqueTranslate);

        // NodeSoleil.transformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.5f, 0.0f))
        //                         * glm::rotate(glm::mat4(1.0f), angleSoleil, glm::vec3(0, 1, 0));

        // NodeLune.transformation = glm::rotate(glm::mat4(1.0f), angleLune, glm::vec3(0, 1, 0)) 
        //                      * glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));

        // NodeTerre.transformation = glm::rotate(glm::mat4(1.0f), angleTerre, glm::vec3(0, 1, 0)) // vitesse de rotation autour du soleil
        //                         * glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) // translation
        //                         * glm::rotate(glm::mat4(1.0f), angleTerre, glm::vec3(0, 1, 0)); // rotation autour d'elle meme


        // NodeMars.transformation = glm::rotate(glm::mat4(1.0f), angleTerre*0.8f, glm::vec3(0, 1, 0)) // vitesse de rotation autour du soleil
        //                         * glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, 0.0f, 0.0f)) // translation
        //                         * glm::rotate(glm::mat4(1.0f), angleTerre*0.8f, glm::vec3(0, 1, 0)); // rotation autour d'elle meme


        // choix des shaders a utiliser
        //glUseProgram(programID);

        // On trouve où est la variable "MVP" dans le Vertex Shader, et on lui envoie notre calcul
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        //glUniformMatrix4fv(glGetUniformLocation(programID,"MVP"),1,false ,glm::value_ptr(MVP));


        SceneRender(Planete.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);
        SceneRender(Cube.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);
        SceneRender(Soleil.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);

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
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        if (mode == 0){

            // mode 1
            cameraSpeed = 2.5 * deltaTime;
            mode = 1;
            camera_target = glm::vec3(0.f, 0.f, 0.f);
            camera_position   = glm::vec3(0.0f, 3.f,  3.f);
            camera_up    = normalize(glm::vec3(0.f,-1.0f,  1.f));
        }
        else if (mode == 1){
            mode = 2;
            camera_target = glm::vec3(0.f, 0.5f, 0.f);
            camera_position   = glm::vec3(0.0f, 2.f,  1.5f);
            camera_up    = normalize(glm::vec3(0.f,1.0f,  -1.f));
        }



        else if (mode == 2){

            //mode 0
            mode = 0;
            cameraSpeed = 0.25;
            camera_position   = glm::vec3(0.0f, 0.8f,  0.f);
            //camera_position   = glm::vec3(0.0f, 0.0f,  -2.f);
            camera_target = glm::vec3(1.f, 0.8f, 0.f);
            //camera_target = glm::vec3(0.f, 0.0f, 0.f);


            
            camera_up    = glm::vec3(0.f,1.0f,  0.f);
            camera_front = glm::normalize(camera_target - camera_position);
            rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));
        }
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
            mouvement ? mouvement = false : mouvement = true;
            NodeCube.vitesse = glm::vec3(0,0,0);
            if (NodeCube.translation.y < - 0.5f){
                NodeCube.translation.y = 1.f;
                NodeCube.translation.x = 0.f;
                NodeCube.translation.z = 0.f;

            }
        }
        if (key == GLFW_KEY_R && action == GLFW_PRESS){
            NodeCube.vitesse = glm::vec3(0,0,0);
            NodeCube.translation.y = 1.f;
            NodeCube.translation.x = NodeTerrain.translation.x - 0.01;
            NodeCube.translation.z = 0.f;

            }
        
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    glm::vec3 camera_right = glm::cross(camera_front, camera_up);
    

    // ancien tp 1
    /* if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_right;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_right;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_up;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_up; */
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS){
        if (longueur < 506 && hauteur < 506){
            longueur += 5;
            hauteur += 5;
            updateTerrain();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS){
        if (longueur > 6 && hauteur > 6){
            longueur -= 5   ;
            hauteur -= 5;
            updateTerrain();
            
        }
    }
    if (mode == 0  ) {
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
            camera_position -= camera_up * deltaTime * cameraSpeed*2;
            camera_target = camera_front + camera_position;
        }
    }

    if (mode == 1 ) {
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
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
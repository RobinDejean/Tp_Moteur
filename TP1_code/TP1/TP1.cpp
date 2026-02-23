// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

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
#include "common/image_ppm.h"
#include "common/ImageBase.h"
#include <common/ImageBase.cpp>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <algorithm>

glm::vec3 camera_position   = glm::vec3(0.0f, 0.8f,  0.f);
glm::vec3 camera_target = glm::vec3(1.f, 0.8f, 0.f);
glm::vec3 camera_up    = glm::vec3(0.f,1.0f,  0.f);
glm::vec3 camera_front = glm::normalize(camera_target - camera_position);



int mode = 0;
float cameraSpeed;
float angle = 0.;
float zoom = 1.;
float theta = 1.;
glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));


float deltaTime = 0.0f; // Temps écoulé entre la frame actuelle et la précédente
float lastFrame = 0.0f;

int longueur = 512;
int hauteur = 512;
ImageBase heightMap;

struct Mesh{
    std::vector<std::vector<unsigned int> > triangles;
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

Mesh terrain;
Mesh boat;
Mesh soleil;
Mesh lune;
Mesh terre;
Mesh mars;

struct Node{
    Mesh* mesh;
    std::vector<Node*> enfants;
    glm::mat4 transformation;
    GLuint textureID;
    int mode;
    Node(){
        mesh = nullptr;
        mode = 0;
        textureID = 0;
        transformation = glm::mat4();
    }
};


struct SceneGraph{
    Node* racine;
};

SceneGraph Planete;
Node NodeSoleil;
Node NodeLune;
Node NodeTerrain;
Node NodeTerre;
Node NodeMars;



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


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
    glm::mat4 modelMatrix = transformationParent * node->transformation;

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
            
            mesh.indexed_vertices.push_back(vec3(((float) i / longueur) - 0.5, y, ((float) j / hauteur) - 0.5));

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
            mesh.indices.push_back(j * longueur + i + 1);
            mesh.indices.push_back((j + 1) * longueur + i + 1);

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

    // Cull triangles which normal is not towards the camera
    glDisable(GL_CULL_FACE);

    // charger les shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");


    heightMap.load("Assets/Heightmap_Mountain.pgm");
    Planete.racine = &NodeTerrain;


    //associer mesh au noeud
    NodeSoleil.mesh = &soleil;
    NodeLune.mesh = &lune;
    NodeTerrain.mesh = &terrain;
    NodeTerre.mesh = &terre;
    NodeMars.mesh = &mars;
    
    //ajouter les enfants
    NodeTerrain.enfants.push_back(&NodeSoleil);
    NodeSoleil.enfants.push_back(&NodeTerre);
    NodeTerre.enfants.push_back(&NodeLune);
    NodeSoleil.enfants.push_back(&NodeMars);

    
    float angleSoleil = 0.0f;
    float angleTerre  = 0.0f;
    float angleLune   = 0.0f;

    //creer les mesh
    sphere(mars,0.075,20);
    sphere(soleil,0.2,20);
    sphere(lune,0.05,20);
    sphere(terre,0.1,20);
    world(terrain);

    //setup des mesh
    setupMesh(soleil);
    setupMesh(lune);
    setupMesh(terre);
    setupMesh(terrain);
    setupMesh(mars);

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
    NodeSoleil.mode = 0;
    NodeTerre.mode = 0;
    NodeLune.mode = 0;
    NodeMars.mode = 0;
    NodeSoleil.textureID = TextureIDSoleil;
    NodeTerre.textureID = TextureIDTerre;
    NodeLune.textureID = TextureIDLune;
    NodeMars.textureID = TextureIDMars;



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
        glm::mat4 projectionMatrix = glm::perspective (glm::radians (45.f), ratio, 0.1f, 100.f);

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

        NodeSoleil.transformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.5f, 0.0f))
                                * glm::rotate(glm::mat4(1.0f), angleSoleil, glm::vec3(0, 1, 0));

        NodeLune.transformation = glm::rotate(glm::mat4(1.0f), angleLune, glm::vec3(0, 1, 0)) 
                             * glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));

        NodeTerre.transformation = glm::rotate(glm::mat4(1.0f), angleTerre, glm::vec3(0, 1, 0)) // vitesse de rotation autour du soleil
                                * glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) // translation
                                * glm::rotate(glm::mat4(1.0f), angleTerre, glm::vec3(0, 1, 0)); // rotation autour d'elle meme


        NodeMars.transformation = glm::rotate(glm::mat4(1.0f), angleTerre*0.8f, glm::vec3(0, 1, 0)) // vitesse de rotation autour du soleil
                                * glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, 0.0f, 0.0f)) // translation
                                * glm::rotate(glm::mat4(1.0f), angleTerre*0.8f, glm::vec3(0, 1, 0)); // rotation autour d'elle meme


        // choix des shaders a utiliser
        //glUseProgram(programID);

        // On trouve où est la variable "MVP" dans le Vertex Shader, et on lui envoie notre calcul
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        //glUniformMatrix4fv(glGetUniformLocation(programID,"MVP"),1,false ,glm::value_ptr(MVP));


        SceneRender(Planete.racine, glm::mat4(1.0f), MatrixID, viewProj, programID);

       










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
            cameraSpeed = 2.5 * deltaTime;
            mode = 1;
            camera_target = glm::vec3(0.f, 0.f, 0.f);
            camera_position   = glm::vec3(0.0f, 4.f,  -3.f);
            camera_up    = normalize(glm::vec3(0.f,-1.0f,  1.f));
        } else {
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

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
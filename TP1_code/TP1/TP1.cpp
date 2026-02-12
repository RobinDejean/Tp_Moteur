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

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.8f,  0.f);
glm::vec3 camera_target = glm::vec3(1.f, 0.8f, 0.f);
glm::vec3 camera_up    = glm::vec3(0.f,1.0f,  0.f);
glm::vec3 camera_front = glm::normalize(camera_target - camera_position);

int longueur = 256;
int hauteur = 256;
ImageBase heightMap;
std::vector<unsigned int> indices; //Triangles concaténés dans une liste

std::vector<std::vector<unsigned int> > triangles;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> uvs;
GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint elementbuffer;
GLuint uvbuffer;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float cameraSpeed;


int mode = 0;

//rotation

float angle = 0.;
float zoom = 1.;
float theta = 1.;
glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));

struct Mesh{
    std::vector<std::vector<unsigned int> > triangles;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    GLuint VAO = 0;
    GLuint indexed_vertices_vbo = 0;
    GLuint uvs_vbo = 0;
    GLuint indices_vbo = 0;
};

Mesh terrain;
Mesh boat;


struct Node{
    Mesh mesh;
    std::vector<Node> enfants;
    glm::mat4 transformation;
    Node(){
        transformation = glm::mat4();
    }
};

struct SceneGraph{
    Node racine;
};




/*******************************************************************************/

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

            // 🔥 Triangulation automatique (fan triangulation)
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

void clear(Mesh &mesh){
    

    glDeleteBuffers(1, &mesh.indexed_vertices_vbo);
    glDeleteBuffers(1, &mesh.indices_vbo);
    glDeleteBuffers(1, &mesh.uvs_vbo);
    glDeleteVertexArrays(1, &mesh.VAO);
    // glDeleteProgram(programID);
}

void world(Mesh &mesh){
    mesh.indexed_vertices.clear();
    mesh.indices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
        for(unsigned int i = 0; i < longueur; i++){
        for(unsigned int j = 0; j < hauteur; j++){
            float y = (float)heightMap[j][i]/255.;
            
            // if(i == 0 || i == longueur - 1 || j == 0 || j == hauteur - 1){
            //     y = 0.;
            // }
            mesh.indexed_vertices.push_back(vec3(((float) i / longueur) - 0.5, y, ((float) j / hauteur) - 0.5));

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
            mesh.indices.push_back(j * longueur + i + 1);
            mesh.indices.push_back((j + 1) * longueur + i + 1);

            mesh.triangles.push_back({j * longueur + i, (j + 1) * longueur + i, (j + 1) * longueur + i + 1});
            
            mesh.indices.push_back(j * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i);
            mesh.indices.push_back((j + 1) * longueur + i + 1);
        }
    }
}

void drawMesh(Mesh& mesh)
{
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.indexed_vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.indexed_vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.indexed_vertices.size() * sizeof(glm::vec3), mesh.indexed_vertices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.indexed_vertices_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &mesh.uvs_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.uvs_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(glm::vec2), mesh.uvs.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.uvs_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &mesh.indices_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indices_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void render(Mesh &mesh){
    glBindVertexArray(mesh.VAO);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(),GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
}

void framebuffer(){
    world(terrain);
    clear(terrain);
    drawMesh(terrain);
    
    render(terrain);
}



int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
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

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glDisable(GL_CULL_FACE);

    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/
    

    //Chargement du fichier de maillage
    std::string filename("Assets/chair.off");
    //loadOFF(filename, indexed_vertices, indices, triangles );
    
    heightMap.load("Assets/Heightmap_Mountain.pgm");
    

    
    

    // // Load it into a VBO

    
    // glGenBuffers(1, &vertexbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // glBufferData(GL_ARRAY_BUFFER, terrain.indexed_vertices.size() * sizeof(glm::vec3), terrain.indexed_vertices.data(), GL_DYNAMIC_DRAW);

    // // Generate a buffer for the indices as well
    
    // glGenBuffers(1, &elementbuffer);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain.indices.size() * sizeof(unsigned int), terrain.indices.data() , GL_DYNAMIC_DRAW);

    
    // glGenBuffers(1,&uvbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    // glBufferData(GL_ARRAY_BUFFER, terrain.uvs.size() * sizeof(glm::vec2), terrain.uvs.data() , GL_DYNAMIC_DRAW);

    GLuint TextureID1 = loadDDS("Assets/rock.dds");
    GLuint TextureUniform1 = glGetUniformLocation(programID,"myRockSampler");
    GLuint TextureID2 = loadDDS("Assets/grass.dds");
    GLuint TextureUniform2 = glGetUniformLocation(programID,"myGrassSampler");
    GLuint TextureID3 = loadDDS("Assets/snowrocks.dds");
    GLuint TextureUniform3 = glGetUniformLocation(programID,"mySnowRocksSampler");



    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    world(terrain);
    openOBJ("Assets/airboat.obj",boat);

    drawMesh(terrain);
    drawMesh(boat);


    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do{

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        // Model matrix : an identity matrix (model will be at the origin) then change
        glm::mat4 modelMatrix = glm::mat4();
                                            
        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        glm::mat4 viewMatrix = glm::lookAt(
            camera_position,
            camera_target,
            camera_up
        );
        // glm::mat4 depthModelMatrix = glm::mat4(1.0);
        // depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        glm::mat4 projectionMatrix = glm::perspective (glm::radians (45.), 4./3., 0.1, 100.);

        glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        glUniformMatrix4fv(glGetUniformLocation(programID,"MVP"),1,false ,glm::value_ptr(MVP));

        /****************************************/




        // // 1rst attribute buffer : vertices
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // glVertexAttribPointer(
        //             0,                  // attribute
        //             3,                  // size
        //             GL_FLOAT,           // type
        //             GL_FALSE,           // normalized?
        //             0,                  // stride
        //             (void*)0            // array buffer offset
        //             );

        // // Index buffer
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);


        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID1);
        glUniform1i(TextureUniform1, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TextureID2);
        glUniform1i(TextureUniform2, 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TextureID3);
        glUniform1i(TextureUniform3, 2);

        // Position
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // // // UVs
        // glEnableVertexAttribArray(1);
        // glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


        // Draw the triangles !
        // glDrawElements(
        //             GL_TRIANGLES,      // mode
        //             terrain.indices.size(),    // count
        //             GL_UNSIGNED_INT,   // type
        //             (void*)0           // element array buffer offset
        //             );

        render(terrain);
        render(boat);

        // glDisableVertexAttribArray(0);
        // glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    clear(boat);
    clear(terrain);

    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
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
            camera_position   = glm::vec3(0.0f, 30.f,  -2.f);
            camera_up    = normalize(glm::vec3(0.f,-1.0f,  1.f));
        } else {
            mode = 0;
            cameraSpeed = 0.25;
            camera_position   = glm::vec3(0.0f, 0.8f,  0.f);
            camera_target = glm::vec3(1.f, 0.8f, 0.f);
            camera_up    = glm::vec3(0.f,1.0f,  0.f);
            camera_front = glm::normalize(camera_target - camera_position);
            rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0));
        }
    }
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    glm::vec3 camera_right = glm::cross(camera_front, camera_up);

    

    glfwSetKeyCallback(window, keyCallback);
    

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
            framebuffer();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS){
        if (longueur > 6 && hauteur > 6){
            longueur -= 5   ;
            hauteur -= 5;
            framebuffer();
            
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

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
#include "Node.hpp"
#include "../TP1/globals.hpp"
#include <utility>


#ifndef MAP_HPP
#define MAP_HPP

class Map{
    std::vector<std::vector<std::vector<std::vector<Node *>>>> blocks;

    public:
        //CONSTRUCTORS
        Map(int width = 10, int height = 10, int depth = 10);
        

        //DESTRUCTORS
        ~Map();

        //GETTERS & SETTERS
        std::vector<std::vector<std::vector<std::vector<Node *>>>> getBlocks() const { return blocks; }
        void addNode(int x, int y, int z, Node * node);

        //RENDER
        void render(GLuint MatrixID, glm::mat4 viewProj, GLuint programID);

};

#endif
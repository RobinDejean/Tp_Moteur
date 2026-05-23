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

class Block {
    public:
        int x, y, z, n;
        Block() : x(0), y(0), z(0), n(0) {}
        Block(int x, int y, int z, int n);
};

class Map{
    std::vector<std::vector<std::vector<std::vector<Node *>>>> blocks; // z x y n
    std::vector<Block> checkPoints;
    std::vector<double> times;
    Block start;
    double startTime = -1.0;
    Block finish;
    double finishTime = -1.0;
    int currentCP = 0;

    public:
        //CONSTRUCTORS
        Map(int width = 10, int height = 10, int depth = 10);
        

        //DESTRUCTORS
        ~Map();

        //GETTERS & SETTERS
        std::vector<std::vector<std::vector<std::vector<Node *>>>> getBlocks() const { return blocks; }
        void addNode(int x, int y, int z, Node * node);
        void addCheckPoint(int x, int y, int z, int n) { checkPoints.push_back(Block(x, y, z, n)); }
        void setStart(int x, int y, int z, int n) { start = Block(x, y, z, n); }
        void setFinish(int x, int y, int z, int n) { finish = Block(x, y, z, n); }
        Block getFinish() const { return finish; }
        void setFinishTime(double time) { finishTime = time; }
        double getFinishTime() const { return finishTime; }
        void setStartTime(double time) { startTime = time; }
        double getStartTime() const { return startTime; }
        std::vector<Block> getCheckPoints() const { return checkPoints; }
        void addTime(double time) {
            times.push_back(time);
            currentCP++;
        }
        std::vector<double> getTimes() const { return times; }
        int getCurrentCP() const { return currentCP; }


        //RENDER
        void render(GLuint MatrixID, glm::mat4 viewProj, GLuint programID);

};

#endif
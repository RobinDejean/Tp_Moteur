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


#ifndef CAR_HPP
#define CAR_HPP

// vitesse
// statMoteur
// adherence
// collisions

class Car{
    Node* node;
    glm::vec3 vitesse;
    float puissance;
    float adherence;

    public:
        //CONSTRUCTORS
        Car(Node* node, float puissance);
        

        //DESTRUCTORS
        ~Car();

        //COLLISION
        static std::pair<glm::vec3,glm::vec3> cylinderPlan(glm::vec3 C, glm::vec3 u, double h, double r, glm::vec3 P, glm::vec3 n);
        void collision();
        void calculVitesse(float dt);
};

#endif
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
};

#endif
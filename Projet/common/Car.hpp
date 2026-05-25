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
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>
#include "common/ImageBase.h"
#include "Node.hpp"
#include "../TP1/globals.hpp"
#include <utility>
#include "Map.hpp"
#include "Mesh.hpp"
#include "../TP1/fonctions.hpp"



#ifndef CAR_HPP
#define CAR_HPP

// vitesse
// statMoteur
// adherence
// collisions

class Car{
    Node* node;
    float puissance;
    float adherence;
    glm::vec3 normalCollision;
    std::vector<bool> collisionEnCours;

    glm::vec3 speedCheckpoints = glm::vec3(0.f);
    Transformation transformationCheckpoints = Transformation();

    public:
        //CONSTRUCTORS
        Car(Node* node, float puissance);
        

        //DESTRUCTORS
        ~Car();

        //COLLISION
        static std::pair<glm::vec3,glm::vec3> cylinderPlan(glm::vec3 C, glm::vec3 u, double h, double r, glm::vec3 P, glm::vec3 n);
        void collision();
        void calculVitesse(float dt);
        void calculPosition(float dt, float acceleration, float freinage);
        float getPuissance() { return puissance; }
        void solver(double _delta_time, Map &map);
        glm::vec3 closestPointOnSegment(const glm::vec3& A, const glm::vec3& B, const glm::vec3& P);
        glm::vec3 closestPointOnTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P);
        void reset(float &steeringAngle) {
            node->transformation.setTranslation(glm::vec3(-10.f, 2.f, -10.f));
            node->transformation.setEulerAngles(glm::vec3(0.f, glm::radians(-90.f), 0.f));
            node->setVitesse(glm::vec3(0.f));
            for (auto roues : node->getEnfants()){
                roues->setVitesse(glm::vec3(0.f));
            }
            steeringAngle = 0.f;
        }
        void resetToCheckpoint(std::vector<double> times, float &steeringAngle) {
            if (times.size() == 0) {
                reset(steeringAngle);
                return;
            }else{
                node->transformation = transformationCheckpoints;
                node->setVitesse(speedCheckpoints);
                for (auto roues : node->getEnfants()){
                    roues->setVitesse(speedCheckpoints);
                }
                steeringAngle = 0.f;
            }
            
        }
};

#endif
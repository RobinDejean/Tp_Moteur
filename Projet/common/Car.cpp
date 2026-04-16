#include "Car.hpp"


//CONSTRUCTORS
Car::Car(Node* node, float puissance) {
    this->node = node;
    this->puissance = puissance;
    vitesse = glm::vec3(0.);
    adherence = 1;
}

//DESTRUCTORS
Car::~Car() {}
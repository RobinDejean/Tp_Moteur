#include "Node.hpp"
#include "Mesh.hpp"
#include "Transformation.hpp"
#include "../TP1/globals.hpp"


//CONSTRUCTORS
Node::Node() {
    //mesh = nullptr;
    volume = 1;
    textureID = 0;
    mode = 0;
    vitesse = glm::vec3(0.0f);
    masse = 1.0;
    friction_statique = 0.5;
    friction_cinetique = 0.3;
    rebond = 0.8;
    transformation = Transformation();
}

//DESTRUCTORS
Node::~Node() {}

//GETTERS
Mesh* Node::getMesh() const {
    return mesh;
}
double Node::getVolume() const {
    return volume;
}
GLuint Node::getTextureID() const {
    return textureID;
}
int Node::getMode() const {
    return mode;
}
glm::vec3 Node::getVitesse() const {
    return vitesse;
}
double Node::getMasse() const {
    return masse;
}
double Node::getFrictionStatique() const {
    return friction_statique;
}
double Node::getFrictionCinetique() const {
    return friction_cinetique;
}
double Node::getRebond() const {
    return rebond;
}


//SETTERS
void Node::setMesh(Mesh* mesh) {
    this->mesh = mesh;
}
void Node::setVolume(double volume) {
    this->volume = volume;
}
void Node::setTextureID(GLuint textureID) {
    this->textureID = textureID;
}
void Node::setMode(int mode) {
    this->mode = mode;
}
void Node::setVitesse(glm::vec3 vitesse) {
    this->vitesse = vitesse;
}
void Node::setMasse(double masse) {
    this->masse = masse;
}
void Node::setCoefficients(double fs, double fc, double r) {
    this->friction_statique = fs;
    this->friction_cinetique = fc;
    this->rebond = r;
}

//METHODS

void Node::updatePosition(){
    glm::vec3 a = glm::vec3(0,-9.81f,0);
    //glm::vec3 a = glm::vec3(0,0,0);
    double b = 0.1;
    double G = 6.67430e-11;
    double d_air = 1;
    double d_eau = 1000;

    
    // double volume = node.volume*node.scale.x*node.scale.y*node.scale.z;
    if (volume <= 1e-8) return;
    double densite = masse / volume;

    glm::vec3 flotaison = glm::vec3(0,(d_eau / densite) * 9.81 * volume,0);

    
    if(transformation.getTranslation().y < 0.){
        a -= (densite / d_eau) * a * volume;
    }
    else{
        //a -= (densite / d_air) * a * volume;
        flotaison *= 0;
        b = 0.0000001;
    }

    for(Node* gravite : gravite){
        glm::vec3 dir = gravite->transformation.getTranslation() - transformation.getTranslation();
        float dist = glm::length(dir);
        if(dist > 0.01f){
            dir = glm::normalize(dir);
            a += (G * (gravite->masse * masse) / (dist * dist)) * dir;
        }
    }

    for(Ressort* ressort : ressorts){
        glm::vec3 dir = ressort->autre->transformation.getTranslation() - transformation.getTranslation();
        float dist = glm::length(dir);
        if(dist > 0.01f){
            dir = glm::normalize(dir);
            float force = ressort->raideur * (dist - ressort->longueurRepos);
            a += force * dir;
        }
        glm::vec3 FC = -ressort->amortissement * (vitesse - ressort->autre->vitesse);
        a+= FC;
    }
    vitesse += (a + flotaison) * deltaTime; 
    vitesse -= b * vitesse * deltaTime;
    transformation.setTranslation(transformation.getTranslation() + vitesse * deltaTime);
}
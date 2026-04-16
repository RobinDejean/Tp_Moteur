#include "Transformation.hpp"
#include "Mesh.hpp"
#include "../TP1/globals.hpp"
#include <vector>

#ifndef NODE_HPP
#define NODE_HPP

struct Ressort;

class Node{
    Mesh* mesh;
    double volume; // avoir 
    std::vector<Node*> enfants;
    std::vector<Node*> gravite;
    std::vector<Ressort*> ressorts;
    
    
    
    GLuint textureID;
    int mode;
    glm::vec3 vitesse;
    double masse;
    double friction_statique;
    double friction_cinetique;
    double rebond;
    
    
    public:

        Transformation transformation;
        //CONSTRUCTORS
        Node();
        

        //DESTRUCTORS
        ~Node();

        //GETTERS
        Mesh* getMesh() const;
        double getVolume() const;
        GLuint getTextureID() const;
        int getMode() const;
        glm::vec3 getVitesse() const;
        double getMasse() const;
        double getFrictionStatique() const;
        double getFrictionCinetique() const;
        double getRebond() const;
        std::vector<Node*> getEnfants() const { return enfants; }
        Transformation getTransformation() const { return transformation; }
        glm::vec3 getCarCenter(float taille) {
            return transformation.getTranslation() + glm::vec3(taille, 0., 0.5 * taille);
        }




        //SETTERS
        void setMesh(Mesh* mesh);
        void setVolume(double volume);
        void setTextureID(GLuint textureID);
        void setMode(int mode);
        void setVitesse(glm::vec3 vitesse);
        void setMasse(double masse);
        void setCoefficients(double fs, double fc, double r);


        //METHODS
        void addEnfant(Node* enfant) { enfants.push_back(enfant); }
        void addGravite(Node* autre) { gravite.push_back(autre); }
        void addRessort(Ressort* ressort) { ressorts.push_back(ressort);}
        void updatePosition();


};

#endif
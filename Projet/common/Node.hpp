#include "Transformation.hpp"
#include "Mesh.hpp"
#include "../TP1/globals.hpp"

#include <vector>

#ifndef NODE_HPP
#define NODE_HPP
enum class TypeSurface;

struct Ressort;

class Node{
    Mesh* mesh;
    double volume; // avoir 
    std::vector<Node*> enfants;
    std::vector<Node*> gravite;
    std::vector<Ressort*> ressorts;
    
    
    
    GLuint *textureID;
    GLuint *textureNormalID;
    GLuint *textureRoughnessID;
    int mode;
    glm::vec3 vitesse;
    double masse;
    double friction_statique;
    double friction_cinetique;
    double rebond;
    double adherence;
    
    
    public:

        Transformation transformation;
        //CONSTRUCTORS
        Node();
        Node(Mesh* mesh, TypeSurface surface);
        

        //DESTRUCTORS
        ~Node();

        //GETTERS
        Mesh* getMesh() const;
        double getVolume() const;
        GLuint* getTextureID() const;
        int getMode() const;
        glm::vec3 getVitesse() const;
        double getMasse() const;
        double getFrictionStatique() const;
        double getFrictionCinetique() const;
        double getRebond() const;
        std::vector<Node*> getEnfants() const { return enfants; }
        Transformation getTransformation() const { return transformation; }
        glm::vec3 getCarCenter() ;
        double getAdherence() const { return adherence; }
        GLuint* getTextureNormalID() const { return textureNormalID; }
        GLuint* getTextureRoughnessID() const { return textureRoughnessID; }




        //SETTERS
        void setMesh(Mesh* mesh);
        void setVolume(double volume);
        void setTextureID(GLuint *textureID);
        void setMode(int mode);
        void setVitesse(glm::vec3 vitesse);
        void setMasse(double masse);
        void setAdherence(double adherence) { this->adherence = adherence; };
        void setCoefficients(double fs, double fc, double r);
        void setTextureNormalID(GLuint *textureNormalID) { this->textureNormalID = textureNormalID; }
        void setTextureRoughnessID(GLuint *textureRoughnessID) { this->textureRoughnessID = textureRoughnessID; }


        //METHODS
        void addEnfant(Node* enfant) { enfants.push_back(enfant); }
        void addGravite(Node* autre) { gravite.push_back(autre); }
        void addRessort(Ressort* ressort) { ressorts.push_back(ressort);}
        void updatePosition();


};

#endif
#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
//#include "common/image_ppm.h"

struct Mesh;
struct Node;

void openOBJ(const std::string& filename, Mesh& mesh);
void sphere(Mesh &mesh, float radius, int nblignes);
void setupCube(Mesh &mesh, float taille);

void setupMesh(Mesh& mesh);
void render(Mesh& mesh);

void world(Mesh &mesh);
void worldFlat(Mesh &mesh);
void worldPenche(Mesh &mesh, double pourcentage);

void updateTerrain();
void updatePos(Node &Node);
void collisionTerrain(Node &Node);
void SceneRender(Node* node, glm::mat4 transformationParent, GLuint MatrixID, glm::mat4 viewProj, GLuint programID);
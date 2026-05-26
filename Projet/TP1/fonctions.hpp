#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "common/ImageBase.h"
#include "common/Mesh.hpp"
#include "common/Node.hpp"
#include "common/Transformation.hpp"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

//#include "common/image_ppm.h"

/* struct Mesh;
struct Node;*/

void openOBJ(const std::string& filename, std::map<std::string, Mesh>& meshes);
glm::vec3 calculerCentreMesh(const Mesh& mesh);
void recentrerMesh(Mesh& mesh, glm::vec3 centre);
/*
void sphere(Mesh &mesh, float radius, int nblignes);
void setupCube(Mesh &mesh, float taille);

void setupMesh(Mesh& mesh);
void render(Mesh& mesh);

void world(Mesh &mesh);
void worldFlat(Mesh &mesh);
void worldPenche(Mesh &mesh, double pourcentage);

void updateTerrain();
void updatePos(Node &Node);
void collisionTerrain(Node &Node); */
void SceneRender(Node* node, glm::mat4 transformationParent, GLuint MatrixID, glm::mat4 viewProj, GLuint programID);
std::vector<double> loadBestTime(const std::string& filename = "best_time.txt");
void saveBestTime(Map& map, const std::string& filename = "best_time.txt");
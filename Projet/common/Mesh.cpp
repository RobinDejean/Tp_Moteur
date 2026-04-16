#include "Mesh.hpp"

//CONSTRUCTORS
Mesh::Mesh() {
    VAO = 0;
    indexed_vertices_vbo = 0;
    uvs_vbo = 0;
    noise_vbo = 0;
    indices_vbo = 0;
}

//DESTRUCTORS
Mesh::~Mesh() {}

//GETTERS
const std::vector<std::vector<unsigned int>>& Mesh::getTriangles() const {
    return triangles;
}
const std::vector<glm::vec3>& Mesh::getIndexedVertices() const {
    return indexed_vertices;
}
const std::vector<glm::vec2>& Mesh::getUvs() const {
    return uvs;
}
const std::vector<float>& Mesh::getNoise() const {
    return noise;
}

//SETTERS
void Mesh::setTriangles(const std::vector<std::vector<unsigned int>>& triangles) {
    this->triangles = triangles;
}
void Mesh::setIndexedVertices(const std::vector<glm::vec3>& indexed_vertices)   {
    this->indexed_vertices = indexed_vertices;
}
void Mesh::setUvs(const std::vector<glm::vec2>& uvs) {
    this->uvs = uvs;
}
void Mesh::setNoise(const std::vector<float>& noise) {
    this->noise = noise;
}

//METHODS
void Mesh::addTriangle(const std::vector<unsigned int>& triangle) {
    triangles.push_back(triangle);
}
void Mesh::addIndexedVertex(const glm::vec3& vertex) {
    indexed_vertices.push_back(vertex);
}
void Mesh::addUv(const glm::vec2& uv) {
    uvs.push_back(uv);
}
void Mesh::addNoise(float noiseValue) {
    noise.push_back(noiseValue);
}
void Mesh::addIndice(unsigned int indice) {
    indices.push_back(indice);
}

void Mesh::setupMesh() {
    // On crée et on "bind" le VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Création du VBO pour les positions
    glGenBuffers(1, &indexed_vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, indexed_vertices_vbo);
    // on copie les positions
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), indexed_vertices.data(), GL_STATIC_DRAW);

    // lecture VAO
    glEnableVertexAttribArray(0);
    // 0,3 = canal 0 vertex shader, 3 = nb float a lire
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // creation ebo pour les indices
    glGenBuffers(1, &indices_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // creation uvs vbo
    if (!uvs.empty()) {
        glGenBuffers(1, &uvs_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        // 1,2 = canal 1 vertex shader, 2 = nb float a lire
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    //creation noise vbo
    if (!noise.empty()) {
        glGenBuffers(1, &noise_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, noise_vbo);
        glBufferData(GL_ARRAY_BUFFER, noise.size() * sizeof(float), noise.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(2); // Canal 2 !
        // 2 = Canal 2, 1 = un seul float a lire
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // On debind
    glBindVertexArray(0);
}

void Mesh::render() {
    //bind vao
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::deleteBuffers() {
    glDeleteBuffers(1, &indexed_vertices_vbo);
    glDeleteBuffers(1, &indices_vbo);
    glDeleteBuffers(1, &uvs_vbo);
    glDeleteBuffers(1,&noise_vbo);
    glDeleteVertexArrays(1, &VAO);
}








// CREATE MESHES

void Mesh::sphere(float radius, int nblignes)
{
    this->indexed_vertices.clear();
    this->indices.clear();
    this->triangles.clear();
    this->uvs.clear();
    this->noise.clear();

    deleteBuffers();

    // Génération des sommets
    for (unsigned int j = 0; j <= nblignes; j++)
    {
        float v = (float)j / nblignes;
        float theta = v * glm::pi<float>(); // latitude [0, PI]

        for (unsigned int i = 0; i <= nblignes; i++)
        {
            float u = (float)i / nblignes;
            float phi = u * glm::two_pi<float>(); // longitude [0, 2PI]

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            this->indexed_vertices.push_back(glm::vec3(x, y, z));
            this->uvs.push_back(glm::vec2(u,v));
            this->noise.push_back(0.0f);
        }
    }

    // Génération des indices
    for (unsigned int j = 0; j < nblignes; j++)
    {
        for (unsigned int i = 0; i < nblignes; i++)
        {
            unsigned int first  = j * (nblignes + 1) + i;
            unsigned int second = first + nblignes + 1;

            // Triangle 1
            this->indices.push_back(first);
            this->indices.push_back(second);
            this->indices.push_back(first + 1);

            // Triangle 2
            this->indices.push_back(second);
            this->indices.push_back(second + 1);
            this->indices.push_back(first + 1);

            this->triangles.push_back({ first, second, first + 1 });
            this->triangles.push_back({ second, second + 1, first + 1 });
        }
    }
    setupMesh();
}

void Mesh::cube(float taille)
{
    this->indexed_vertices.clear();
    this->indices.clear();
    this->triangles.clear();
    this->uvs.clear();
    this->noise.clear();

    deleteBuffers();

    this->indexed_vertices.push_back(glm::vec3(0,0,0));
    this->indexed_vertices.push_back(glm::vec3(taille,0,0));
    this->indexed_vertices.push_back(glm::vec3(taille,0,taille));
    this->indexed_vertices.push_back(glm::vec3(0,0, taille));
    this->indexed_vertices.push_back(glm::vec3(0,taille,0));
    this->indexed_vertices.push_back(glm::vec3(taille,taille,0));
    this->indexed_vertices.push_back(glm::vec3(taille,taille,taille));
    this->indexed_vertices.push_back(glm::vec3(0,taille, taille));
    this->uvs.push_back(glm::vec2(0,0));
    this->uvs.push_back(glm::vec2(1,0));
    this->uvs.push_back(glm::vec2(0,1));
    this->uvs.push_back(glm::vec2(1,1));
    this->uvs.push_back(glm::vec2(0,0));
    this->uvs.push_back(glm::vec2(1,0));
    this->uvs.push_back(glm::vec2(0,1));
    this->uvs.push_back(glm::vec2(1,1));


    this->indices.push_back(0);
    this->indices.push_back(1);
    this->indices.push_back(2);
    
    this->indices.push_back(0);
    this->indices.push_back(2);
    this->indices.push_back(3);

    this->indices.push_back(4);
    this->indices.push_back(5);
    this->indices.push_back(6);

    this->indices.push_back(4);
    this->indices.push_back(6);
    this->indices.push_back(7);

    this->indices.push_back(0);
    this->indices.push_back(1);
    this->indices.push_back(5);
    
    this->indices.push_back(0);
    this->indices.push_back(5);
    this->indices.push_back(4);

    this->indices.push_back(1);
    this->indices.push_back(2);
    this->indices.push_back(6);

    this->indices.push_back(1);
    this->indices.push_back(6);
    this->indices.push_back(5);

    this->indices.push_back(2);
    this->indices.push_back(3);
    this->indices.push_back(7);

    this->indices.push_back(2);
    this->indices.push_back(7);
    this->indices.push_back(6);

    this->indices.push_back(3);
    this->indices.push_back(0);
    this->indices.push_back(4);

    this->indices.push_back(3);
    this->indices.push_back(4);
    this->indices.push_back(7);

    setupMesh();
}


void Mesh::world(int longueur, int hauteur, ImageBase& heightMap) {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    for(unsigned int i = 0; i < longueur; i++){
        for(unsigned int j = 0; j < hauteur; j++){
            float y = (float)heightMap[j][i]/255. - 0.5f;
            
            indexed_vertices.push_back(vec3(((float) i / (longueur-1)) - 0.5, y, ((float) j / (hauteur-1)) - 0.5));

            uvs.push_back(
                vec2((float)i / (longueur - 1),
                    (float)j / (hauteur - 1))
            );

            float random_val = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;
            noise.push_back(random_val);
        }
    }

    for(unsigned int i = 0; i < longueur - 1; i++){
        for(unsigned int j = 0; j < hauteur - 1; j++){
            triangles.push_back({j * longueur + i, j * longueur + i + 1, (j + 1) * longueur + i + 1});

            indices.push_back(j * longueur + i);
            indices.push_back((j + 1) * longueur + i + 1);
            indices.push_back(j * longueur + i + 1);

            triangles.push_back({j * longueur + i, (j + 1) * longueur + i, (j + 1) * longueur + i + 1});
            
            indices.push_back(j * longueur + i);
            indices.push_back((j + 1) * longueur + i);
            indices.push_back((j + 1) * longueur + i + 1);
        }
    }
    setupMesh();
}


void Mesh::worldPenche(int longueur, int hauteur, double pourcentage){
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();
        for(unsigned int i = 0; i < longueur; i++){
        for(unsigned int j = 0; j < hauteur; j++){
            
            indexed_vertices.push_back(vec3(((float) i / (longueur-1)) - 0.5, (((float) i / (longueur-1)) - 0.5) * pourcentage, ((float) j / (hauteur-1)) - 0.5));

            uvs.push_back(
                vec2((float)i / (longueur - 1),
                    (float)j / (hauteur - 1))
            );
        }
    }

    for(unsigned int i = 0; i < longueur - 1; i++){
        for(unsigned int j = 0; j < hauteur - 1; j++){
            triangles.push_back({j * longueur + i, j * longueur + i + 1, (j + 1) * longueur + i + 1});

            indices.push_back(j * longueur + i);
            indices.push_back((j + 1) * longueur + i + 1);
            indices.push_back(j * longueur + i + 1);

            triangles.push_back({j * longueur + i, (j + 1) * longueur + i, (j + 1) * longueur + i + 1});
            
            indices.push_back(j * longueur + i);
            indices.push_back((j + 1) * longueur + i);
            indices.push_back((j + 1) * longueur + i + 1);
        }
    }
    setupMesh();
}



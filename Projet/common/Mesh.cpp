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
std::vector<glm::vec3>& Mesh::getIndexedVertices() {
    return indexed_vertices;
}
const std::vector<glm::vec2>& Mesh::getUvs() const {
    return uvs;
}
const std::vector<float>& Mesh::getNoise() const {
    return noise;
}
std::vector<unsigned int>& Mesh::getIndices() {
    return indices;
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

void Mesh::car(float taille){
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    this->indexed_vertices.push_back(glm::vec3(0,0,0));
    this->indexed_vertices.push_back(glm::vec3(taille * 2,0,0));
    this->indexed_vertices.push_back(glm::vec3(taille * 2,0,taille));
    this->indexed_vertices.push_back(glm::vec3(0,0, taille));
    this->indexed_vertices.push_back(glm::vec3(taille / 2,taille,0));
    this->indexed_vertices.push_back(glm::vec3(taille * 1.5,taille,0));
    this->indexed_vertices.push_back(glm::vec3(taille * 1.5,taille,taille));
    this->indexed_vertices.push_back(glm::vec3(taille / 2,taille, taille));
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

void Mesh::createWheel(float radius, float width, int segments)
{
    float halfWidth = width / 2.0f;

    this->indexed_vertices.clear();
    this->indices.clear();
    this->triangles.clear();
    this->uvs.clear();
    this->noise.clear();

    deleteBuffers();

    // Générer les vertices
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        // cercle avant
        indexed_vertices.push_back({x, y, -halfWidth});
        // cercle arrière
        indexed_vertices.push_back({x, y, halfWidth});
    }

    // Générer les faces (côtés du cylindre)
    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;

        int i0 = i * 2;
        int i1 = i * 2 + 1;
        int i2 = next * 2;
        int i3 = next * 2 + 1;

        // triangle 1
        indices.push_back(i0);
        indices.push_back(i2);
        indices.push_back(i1);

        // triangle 2
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }
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

// ---------------------------------------- ROUTES ----------------------------------------

void Mesh::road_line() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    //sol

    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.0f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3(blockSize / 2.0f, 0.0f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3(blockSize / 2.0f, 0.0f, blockSize / 2.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.0f, blockSize / 2.0f));

    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 1.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);

    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(3);

    //bordures extérieures

    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.5f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3(blockSize / 2.0f, 0.5f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3(blockSize / 2.0f, 0.5f, blockSize / 2.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.5f, blockSize / 2.0f));

    uvs.push_back(vec2(0.05f, 0.0f));
    uvs.push_back(vec2(0.95f, 0.0f));
    uvs.push_back(vec2(0.95f, 1.0f));
    uvs.push_back(vec2(0.05f, 1.0f));

    indices.push_back(0);
    indices.push_back(7);
    indices.push_back(3);

    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(4);

    indices.push_back(1);
    indices.push_back(6);
    indices.push_back(2);

    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(1);

    setupMesh();
}

void Mesh::road_quarterpipe() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    // Configuration des dimensions
    const float flatLength = 5.0f; // Longueur des plats en mètres à chaque extrémité
    const int segmentsCurve = 12;  // Nombre de segments pour adoucir la courbe du milieu
    
    float halfSize = blockSize / 2.0f;

    // La taille disponible pour la courbe est la taille totale moins les deux plats
    float curveSize = blockSize - flatLength;
    
    // Sécurité : si le plat est trop grand pour la taille du bloc, on le limite
    if (curveSize < 0.0f) {
        curveSize = 0.0f;
    }

    // --- 1. PREMIER BOUT : LE PLAT HORIZONTAL (Au sol) ---
    // Sommet arrière gauche (départ de la route)
    indexed_vertices.push_back(vec3(-halfSize, -halfSize, -halfSize));
    uvs.push_back(vec2(0.0f, 0.0f));
    // Sommet arrière droit
    indexed_vertices.push_back(vec3(halfSize, -halfSize, -halfSize));
    uvs.push_back(vec2(1.0f, 0.0f));

    // Sommet avant gauche du plat (juste avant que la courbe commence)
    indexed_vertices.push_back(vec3(-halfSize, -halfSize, -halfSize + flatLength));
    uvs.push_back(vec2(0.0f, 0.15f)); // V arbitraire pour le plat, ajustable
    // Sommet avant droit du plat
    indexed_vertices.push_back(vec3(halfSize, -halfSize, -halfSize + flatLength));
    uvs.push_back(vec2(1.0f, 0.15f));


    // --- 2. LE MILIEU : LA COURBE (QUARTER PIPE) ---
    // On génère la courbe. Elle commence là où le premier plat s'arrête
    for (int i = 1; i < segmentsCurve; ++i) {
        float t = (float)i / (float)segmentsCurve;
        float angle = t * (3.14159265f / 2.0f); 

        // La courbe d'adapte à curveSize
        float z = (-halfSize + flatLength) + (sin(angle) * curveSize);
        float y = (1.0f - cos(angle)) * curveSize -halfSize;

        // Sommet Gauche
        indexed_vertices.push_back(vec3(-halfSize, y, z));
        uvs.push_back(vec2(0.0f, 0.15f + t * 0.70f));

        // Sommet Droit
        indexed_vertices.push_back(vec3(halfSize, y, z));
        uvs.push_back(vec2(1.0f, 0.15f + t * 0.70f));
    }


    // --- 3. DEUXIÈME BOUT : LE PLAT VERTICAL (Le mur) ---
    // Sommet bas gauche du mur (fin de la courbe)
    indexed_vertices.push_back(vec3(-halfSize, curveSize - halfSize, halfSize));
    uvs.push_back(vec2(0.0f, 0.85f));
    // Sommet bas droit du mur
    indexed_vertices.push_back(vec3(halfSize, curveSize - halfSize, halfSize));
    uvs.push_back(vec2(1.0f, 0.85f));

    // Sommet tout en haut à gauche (fin de la route)
    indexed_vertices.push_back(vec3(-halfSize, halfSize, halfSize));
    uvs.push_back(vec2(0.0f, 1.0f));
    // Sommet tout en haut à droit
    indexed_vertices.push_back(vec3(halfSize, halfSize, halfSize));
    uvs.push_back(vec2(1.0f, 1.0f));


    // --- 4. GÉNÉRATION DES TRIANGLES ---
    // Le nombre total de "tranches" de deux sommets est :
    // 2 (pour le premier plat) + (segmentsCurve - 1) (milieu) + 2 (plat vertical)
    int totalSteps = 2 + (segmentsCurve - 1) + 2;

    for (int i = 0; i < totalSteps - 1; ++i) {
        int topLeft     = i * 2;
        int topRight    = i * 2 + 1;
        int bottomLeft  = (i + 1) * 2;
        int bottomRight = (i + 1) * 2 + 1;

        // Triangle 1
        indices.push_back(topLeft);
        indices.push_back(bottomRight);
        indices.push_back(topRight);

        // Triangle 2
        indices.push_back(topLeft);
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
    }

    setupMesh();
}

void Mesh::road_line_penche() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.0f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * 1.0f, 0.0f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * 1.0f, 2.0f, (blockSize / 2.0f) * 1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 2.0f, (blockSize / 2.0f) * 1.0f));

    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 1.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);

    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(3);

    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.5f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * 1.0f, 0.5f, (blockSize / 2.0f) * -1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * 1.0f, 2.5f, (blockSize / 2.0f) * 1.0f));
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 2.5f, (blockSize / 2.0f) * 1.0f));

    uvs.push_back(vec2(0.05f, 0.0f));
    uvs.push_back(vec2(0.95f, 0.0f));
    uvs.push_back(vec2(0.95f, 1.0f));
    uvs.push_back(vec2(0.05f, 1.0f));

    indices.push_back(0);
    indices.push_back(7);
    indices.push_back(3);

    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(4);

    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(6);

    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(1);

    setupMesh();
}

void Mesh::road_corner() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    const int segments = 16;       // qualité du virage
    const float radiusOuter = blockSize;     // rayon extérieur du virage
    const float height = 0.5f;

    // interieur
    indexed_vertices.push_back(vec3((blockSize / 2.0f) * -1.0f, 0.0f, (blockSize / 2.0f) * -1.0f));
    uvs.push_back(vec2(0.0f, 0.0f));

    // ===== BASE (y = 0) =====
    for (int i = 0; i <= segments; i++) {
        float angle = (glm::half_pi<float>() * i) / segments;

        float cosA = cos(angle);
        float sinA = sin(angle);

        // extérieur
        indexed_vertices.push_back(vec3(radiusOuter * cosA - (blockSize / 2.0f), 0.0f, radiusOuter * sinA - (blockSize / 2.0f)));
        uvs.push_back(vec2(radiusOuter * cosA / 10., radiusOuter * sinA / 10.));

        indexed_vertices.push_back(vec3(radiusOuter * cosA - (blockSize / 2.0f), 0.5f, radiusOuter * sinA - (blockSize / 2.0f)));
        uvs.push_back(vec2(radiusOuter * cosA / 10. - 0.05f, radiusOuter * sinA / 10.));
    }

    // triangles base
    for (int i = 0; i < segments; i++) {
        int i1 = i * 2 + 1;
        int i2 = i * 2 + 2;
        int i3 = i * 2 + 3;
        int i4 = i * 2 + 4;

        indices.push_back(0);
        indices.push_back(i3);
        indices.push_back(i1);

        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i4);

        indices.push_back(i4);
        indices.push_back(i3);
        indices.push_back(i1);
    }

    setupMesh();
}

// ---------------------------------------- OBSTACLES ----------------------------------------

void Mesh::pillar() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    int segments = 32;       // Plus ce chiffre est haut, plus le cylindre est rond
    float radius = 1.0f;     // Rayon du cylindre
    float height = 3.0f;     // Hauteur du cylindre


    // ==========================================
    // 1. GÉNÉRATION DU CORPS (LES PAROIS)
    // ==========================================
    // On fait une boucle jusqu'à "segments" inclus pour fermer proprement la texture UV
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / (float)segments;
        float angle = t * 2.0f * 3.14159265f; // Tour complet (0 à 360°)

        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Sommet du bas
        indexed_vertices.push_back(vec3(x, 0., z));
        uvs.push_back(vec2(t, 0.0f));

        // Sommet du haut
        indexed_vertices.push_back(vec3(x, height, z));
        uvs.push_back(vec2(t, 1.0f));
    }

    // Indices pour les parois
    for (int i = 0; i < segments; ++i) {
        int bottomLeft  = 2 * i;
        int topLeft     = 2 * i + 1;
        int bottomRight = 2 * (i + 1);
        int topRight    = 2 * (i + 1) + 1;

        // Triangle 1
        indices.push_back(bottomRight);
        indices.push_back(bottomLeft);
        indices.push_back(topLeft);

        // Triangle 2
        indices.push_back(bottomRight);
        indices.push_back(topLeft);
        indices.push_back(topRight);
    }

    // ==========================================
    // 2. GÉNÉRATION DES COUVERCLES (BAS ET HAUT)
    // ==========================================
    // Pour éviter des problèmes de textures et de normales, on recrée des sommets 
    // dédiés aux couvercles. On utilise une structure en "disque de triangles" (Triangle Fan).

    // Index de départ pour les couvercles
    int baseIndex = indexed_vertices.size();

    // --- Couvercle du Bas ---
    int centerBottomIndex = baseIndex;
    indexed_vertices.push_back(vec3(0.0f, 0., 0.0f)); // Centre bas
    uvs.push_back(vec2(0.5f, 0.5f));

    for (int i = 0; i <= segments; ++i) {
        float angle = ((float)i / (float)segments) * 2.0f * 3.14159265f;
        indexed_vertices.push_back(vec3(radius * cos(angle), 0., radius * sin(angle)));
        uvs.push_back(vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)));
        
        if (i < segments) {
            indices.push_back(centerBottomIndex);
            indices.push_back(centerBottomIndex + 1 + i + 1); // Sens inverse des aiguilles pour faire face vers le bas
            indices.push_back(centerBottomIndex + 1 + i);
        }
    }

    // --- Couvercle du Haut ---
    int centerTopIndex = indexed_vertices.size();
    indexed_vertices.push_back(vec3(0.0f, height, 0.0f)); // Centre haut
    uvs.push_back(vec2(0.5f, 0.5f));

    for (int i = 0; i <= segments; ++i) {
        float angle = ((float)i / (float)segments) * 2.0f * 3.14159265f;
        indexed_vertices.push_back(vec3(radius * cos(angle), height, radius * sin(angle)));
        uvs.push_back(vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)));
        
        if (i < segments) {
            indices.push_back(centerTopIndex);
            indices.push_back(centerTopIndex + 1 + i);
            indices.push_back(centerTopIndex + 1 + i + 1); // Face vers le haut
        }
    }

    setupMesh();
}
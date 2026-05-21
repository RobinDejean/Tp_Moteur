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

    indexed_vertices.push_back(vec3(-5.0f, 0.0f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 0.0f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 0.0f, 5.0f));
    indexed_vertices.push_back(vec3(-5.0f, 0.0f, 5.0f));

    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 1.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    indexed_vertices.push_back(vec3(-5.0f, 0.5f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 0.5f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 0.5f, 5.0f));
    indexed_vertices.push_back(vec3(-5.0f, 0.5f, 5.0f));

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

void Mesh::road_quarterpipe() {
    indexed_vertices.clear();
    indices.clear();
    triangles.clear();
    uvs.clear();
    noise.clear();
    deleteBuffers();

    int segments = 20;       // Fluidité de la courbe
    float radius = 10.0f;     // Rayon de la rampe
    float lengthZ = 10.0f;   // Longueur sur l'axe Z
    float thickness = 0.5f;  // Épaisseur de la bordure (comme ton 0.5f d'origine)

    // On calcule les coordonnées Z pour le côté gauche et droit
    float z1 = -lengthZ / 2.0f;
    float z2 = lengthZ / 2.0f;

    // ==========================================
    // 1. GÉNÉRATION DES VERTICES ET DES UVS
    // ==========================================
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / (float)segments;
        float angle = t * (3.14159265f / 2.0f);

        // --- Surface de la route (Piste intérieure) ---
        float x = -5.0f + radius * (1.0f - cos(angle)); 
        float y = -5.0f + radius * (1.0f - sin(angle));

        // --- Surface externe (Le "dos" ou l'épaisseur de la structure) ---
        // On pousse les sommets vers l'extérieur en suivant le vecteur de la courbe
        float ext_x = x + thickness * cos(angle);
        float ext_y = y + thickness * sin(angle);

        // AJOUT DES VERTICES (L'ordre ici est CRUCIAL pour calculer les indices facilement)
        // [0] Piste - Côté Z-
        indexed_vertices.push_back(vec3(x, y, z1));
        uvs.push_back(vec2(t, 0.0f));

        // [1] Piste - Côté Z+
        indexed_vertices.push_back(vec3(x, y, z2));
        uvs.push_back(vec2(t, 1.0f));

        // [2] Extérieur - Côté Z-
        indexed_vertices.push_back(vec3(ext_x, ext_y, z1));
        uvs.push_back(vec2(t, 0.05f)); // Léger décalage UV comme ton code initial

        // [3] Extérieur - Côté Z+
        indexed_vertices.push_back(vec3(ext_x, ext_y, z2));
        uvs.push_back(vec2(t, 0.95f));
    }

    // ==========================================
    // 2. GÉNÉRATION DES TRIANGLES (INDICES)
    // ==========================================
    for (int i = 0; i < segments; ++i) {
        // Pour chaque étape 'i', on a généré 4 sommets.
        // On calcule l'index de départ pour l'étape actuelle (i) et la suivante (next)
        int curr = i * 4;
        int next = (i + 1) * 4;

        // Repérage des points de la tranche actuelle (i)
        int p_zMin = curr + 0; // Piste Z-
        int p_zMax = curr + 1; // Piste Z+
        int e_zMin = curr + 2; // Extérieur Z-
        int e_zMax = curr + 3; // Extérieur Z+

        // Repérage des points de la tranche suivante (i+1)
        int np_zMin = next + 0; // Next Piste Z-
        int np_zMax = next + 1; // Next Piste Z+
        int ne_zMin = next + 2; // Next Extérieur Z-
        int ne_zMax = next + 3; // Next Extérieur Z+

        // --- FACE 1 : La Piste Intérieure (Où roulent les véhicules) ---
        indices.push_back(p_zMin);
        indices.push_back(np_zMin);
        indices.push_back(p_zMax);

        indices.push_back(p_zMax);
        indices.push_back(np_zMin);
        indices.push_back(np_zMax);

        // --- FACE 2 : La Bordure Latérale Droite (Flanc Z-) ---
        indices.push_back(e_zMin);
        indices.push_back(p_zMin);
        indices.push_back(ne_zMin);

        indices.push_back(ne_zMin);
        indices.push_back(p_zMin);
        indices.push_back(np_zMin);

        // --- FACE 3 : La Bordure Latérale Gauche (Flanc Z+) ---
        indices.push_back(p_zMax);
        indices.push_back(e_zMax);
        indices.push_back(np_zMax);

        indices.push_back(np_zMax);
        indices.push_back(e_zMax);
        indices.push_back(ne_zMax);
        
        // Note : Si tu as besoin de fermer le "dos" de la rampe (la face extérieure cachée),
        // dis-le moi, on peut aussi générer les triangles entre e_zMin/e_zMax et ne_zMin/ne_zMax.
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

    indexed_vertices.push_back(vec3(-5.0f, 0.0f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, .0f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 2.0f, 5.0f));
    indexed_vertices.push_back(vec3(-5.0f, 2.0f, 5.0f));

    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(1.0f, 1.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    indexed_vertices.push_back(vec3(-5.0f, 0.5f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 0.5f, -5.0f));
    indexed_vertices.push_back(vec3(5.0f, 2.5f, 5.0f));
    indexed_vertices.push_back(vec3(-5.0f, 2.5f, 5.0f));

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
    const float radiusOuter = 10.0f;
    const float radiusInner = 6.0f;
    const float height = 0.5f;

    // interieur
    indexed_vertices.push_back(vec3(-5.0f, 0.0f, -5.0f));
    uvs.push_back(vec2(0.0f, 0.0f));

    // ===== BASE (y = 0) =====
    for (int i = 0; i <= segments; i++) {
        float angle = (glm::half_pi<float>() * i) / segments;

        float cosA = cos(angle);
        float sinA = sin(angle);

        // extérieur
        indexed_vertices.push_back(vec3(radiusOuter * cosA - 5.0f, 0.0f, radiusOuter * sinA - 5.0f));
        uvs.push_back(vec2(radiusOuter * cosA / 10., radiusOuter * sinA / 10.));

        indexed_vertices.push_back(vec3(radiusOuter * cosA - 5.0f, 0.5f, radiusOuter * sinA - 5.0f));
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
        indices.push_back(i3);
        indices.push_back(i4);

        indices.push_back(i4);
        indices.push_back(i2);
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
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

//COLLISION

std::pair<glm::vec3,glm::vec3> Car::cylinderPlan(glm::vec3 C, glm::vec3 u, double h, double r, glm::vec3 P, glm::vec3 n){
    // Distance signée du centre au plan
    double d = glm::dot(n, C - P);

    // Projection de la demi-hauteur sur la normale
    double h_n = (h * 0.5) * std::abs(glm::dot(n, u));

    // Contribution du rayon
    double dotNU = glm::dot(n, u);
    double r_n = r * std::sqrt(std::max(0.0, 1.0 - dotNU * dotNU));

    // Test de collision
    if (std::abs(d) > h_n + r_n) {
        return {glm::vec3(0.0f), glm::vec3(0.0f)}; // pas de collision
    }

    // --- Point de contact approximé ---

    // Projection du centre sur le plan
    glm::vec3 contact = C - (float)d * n;

    // Optionnel : déplacer vers la surface du cylindre
    // direction tangentielle
    glm::vec3 radial = glm::normalize(glm::cross(u, glm::cross(n, u)));

    if (glm::length(radial) > 1e-6f) {
        contact -= (float)r * radial;
    }

    return {contact, n};
}

void Car::collision(){
    glm::vec3 P = glm::vec3(0.0f);
    glm::vec3 n = VEC_UP;

    // matrice monde du chassis
    glm::mat4 M_car = node->transformation.computeTransformationMatrix();

    for (auto& enfant : node->getEnfants()) {

        // matrice locale roue
        glm::mat4 M_wheel = enfant->transformation.computeTransformationMatrix();

        // matrice monde roue
        glm::mat4 M = M_car * M_wheel;

        // --- POSITION DU CENTRE ---
        glm::vec3 C = applyTransformation(glm::vec3(0.f), 1.f, M);

        // --- AXE DU CYLINDRE ---
        // axe local Z (roue classique)
        glm::vec3 u = applyTransformation(VEC_FRONT, 0.f, M);
        u = glm::normalize(u);

        // Collision
        auto result = cylinderPlan(C, u, widthRoue, rayonRoue, P, n);

        if (result.second == VEC_ZERO)
            continue;

        // --- calcul pénétration ---
        float d = glm::dot(n, C - P);

        float dotNU = glm::dot(n, u);
        float h_n = (widthRoue * 0.5f) * std::abs(dotNU);
        float r_n = rayonRoue * std::sqrt(std::max(0.0f, 1.0f - dotNU * dotNU));

        float penetration = (h_n + r_n) - std::abs(d);

        if (penetration > 0.0f) {

            glm::vec3 correction = n * penetration;

            // appliquer UNIQUEMENT au chassis
            node->transformation.setTranslation(
                node->transformation.getTranslation() + correction
            );
        }
    }
}

void Car::calculVitesse(float dt) {
    glm::vec3 v = node->getVitesse();
    float speed = glm::length(v);
    float masse = node->getMasse();

    if (speed < 0.1f) speed = 0.1f;

    float force_moteur = puissance / speed;

    float force_max = adherence * masse * 9.81f;

    float force = glm::min(force_moteur, force_max);

    glm::vec3 direction = glm::normalize(v);

    // accélération
    glm::vec3 acceleration = (force / masse) * direction;

    v += acceleration * dt;

    node->setVitesse(v);
}
#include "Car.hpp"


//CONSTRUCTORS
Car::Car(Node* node, float puissance) {
    this->node = node;
    this->puissance = puissance;
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

void Car::calculPosition(float dt, float accelerationInput) {
    // 1. On récupère la vitesse actuelle (norme)
    glm::vec3 v_vec = node->getVitesse();
    glm::mat4 rotationMatrix = node->transformation.getRotationMatrix();
    // On considère que l'avant de ta voiture est l'axe X (1,0,0) d'après ton code précédent
    glm::vec3 directionChassis = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    float speed = glm::dot(v_vec, directionChassis); 
    float abs_speed = std::abs(speed);
    float masse = node->getMasse();
    // 2. Calcul de la force (Scalaire)
    float force;
    float force_max = adherence * masse * 9.81f;
    if (abs_speed == 0.0f ){
        force = force_max;
    }else{
        float force_moteur = puissance * std::abs(accelerationInput) / abs_speed;
        force = glm::min(force_moteur, force_max);

    }
    std::cout << "force: " << force << std::endl;

    // 4. Application des forces sur la Vitesse Signée
    if (accelerationInput > 0.0f) {
        // ACCÉLÉRATION (Marche avant)
        speed += (force / masse) * dt;
        
    } else if (accelerationInput < 0.0f) {
        // TOUCHE RECULER/FREINER ENFONCÉE
        if (speed > 0.1f) {
            // Cas A : On avance, donc c'est un FREINAGE
            speed -= 0.4f * speed * dt + 15.0f * dt; // Freine fort
            if (speed < 0.0f) speed = 0.0f; // On s'arrête net sans repartir en arrière
        } else {
            // Cas B : On est à l'arrêt ou on recule déjà, c'est la MARCHE ARRIÈRE
            speed -= (force / masse) * dt; // La vitesse devient de plus en plus négative
        }
        
    } else {
        // AUCUNE TOUCHE : Friction naturelle
        speed -= 0.1f * speed * dt;
        // Arrêt complet si la vitesse est très faible pour éviter de glisser infiniment
        if (abs_speed < 0.05f) speed = 0.0f;
    }

    
    // Ajoute une petite friction pour que la voiture s'arrête si on n'accélère plus
    //speed *= 0.99f; 
    std::cout << "speed: " << speed << std::endl;

    // 4. Calcul de la rotation du châssis (Lacet)
    // On utilise l'angle des roues (attention : vérifie si c'est .y ou .z selon ton axe vertical)
    float anglesRoues = node->getEnfants()[0]->transformation.getEulerAngles().y; 
    float empattement = glm::distance(node->getEnfants()[0]->transformation.getTranslation(), 
                                      node->getEnfants()[2]->transformation.getTranslation());
    
    float rotationChassis = (speed * tan(anglesRoues) / empattement) * dt;
    rotationMatrix = node->transformation.getRotationMatrix();
    directionChassis = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    node->transformation.addEulerAngles(glm::vec3(0, rotationChassis, 0));

    // 5. DETERMINATION DU NOUVEAU VECTEUR VITESSE
    // Maintenant que le châssis a tourné, la vitesse pointe vers l'avant du châssis !
    
    glm::vec3 nouveauV = directionChassis * speed;
    node->setVitesse(nouveauV);

    // 6. Mise à jour de la position
    glm::vec3 translation = node->transformation.getTranslation();
    translation += nouveauV * dt;
    node->transformation.setTranslation(translation);
}
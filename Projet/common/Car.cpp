#include "Car.hpp"



//CONSTRUCTORS
Car::Car(Node* node, float puissance) {
    this->node = node;
    this->puissance = puissance;
    adherence = 1.f;
    collisionEnCours = {false, false, false, false}; // 4 roues
}

//DESTRUCTORS
Car::~Car() {}

//COLLISION

void Car::calculPosition(float dt, float acceleration, float freinage) {
    // 1. On récupère la vitesse actuelle (norme)
    int numCollisions = 0;
    for (int i = 0; i < collisionEnCours.size(); i++) {
        if (collisionEnCours[i]) {
            numCollisions++;
        }
    }
    glm::vec3 v_vec = node->getVitesse();
    glm::mat4 rotationMatrix = node->transformation.getRotationMatrix();
    // On considère que l'avant de ta voiture est l'axe X (1,0,0) d'après ton code précédent
    glm::vec3 directionChassis = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    float speed = glm::dot(v_vec, directionChassis);
    float abs_speed = std::abs(speed);
    float airResistance = 0.005f * abs_speed * speed;
    float masse = node->getMasse();


    glm::vec3 axeAvant  = directionChassis;            // Axe X local
    glm::vec3 axeHaut   = glm::normalize(glm::vec3(rotationMatrix[1])); // Axe Y local
    glm::vec3 axeDroite = glm::normalize(glm::vec3(rotationMatrix[2]));
    float anglesRoues = node->getEnfants()[0]->transformation.getEulerAngles().y; 
    float empattement = glm::distance(node->getEnfants()[0]->transformation.getTranslation(), 
                                      node->getEnfants()[2]->transformation.getTranslation());

    if (numCollisions != 0) {
        
       
        float ancienSpeed = speed;
        // 2. Calcul de la force (Scalaire)
        float force;
        float force_max = adherence * masse * 9.81f;
        if (abs_speed == 0.0f ){
            force = force_max;
        }else if (acceleration > 0.0f) {
            float force_moteur = puissance * std::abs(acceleration) / abs_speed;
            force = glm::min(force_moteur, force_max);
        }else   if (freinage > 0.0f) {
            float force_frein = puissance * std::abs(freinage) / abs_speed;
            force = glm::min(force_frein, force_max);
        }else {
            force = 0.0f;
        }

        // 4. Application des forces sur la Vitesse Signée
        if (acceleration > 0.0f) {
            // ACCÉLÉRATION (Marche avant)
            speed += ((force - airResistance) / masse)* 5 * dt;
            
        } else if (freinage > 0.0f) {
            // TOUCHE RECULER/FREINER ENFONCÉE
            if (speed > 0.1f) {
                // Cas A : On avance, donc c'est un FREINAGE
                speed -= 0.6f * speed * dt + 15.0f * dt; // Freine fort
                if (speed < 0.0f) speed = 0.0f; // On s'arrête net sans repartir en arrière
            } else {
                // Cas B : On est à l'arrêt ou on recule déjà, c'est la MARCHE ARRIÈRE
                speed -= ((force + airResistance) / masse) * dt; // La vitesse devient de plus en plus négative
            }
            
        } else {
            // AUCUNE TOUCHE : Friction naturelle
            speed -= (0.4f * speed + (airResistance/masse)) * dt;
            // Arrêt complet si la vitesse est très faible pour éviter de glisser infiniment
            if (abs_speed < 0.05f) speed = 0.0f;
        }
        
        float rotationChassis = (speed * tan(anglesRoues) / empattement) * dt;
        rotationMatrix = node->transformation.getRotationMatrix();
        glm::vec3 axeHautLocal = glm::vec3(rotationMatrix[1]);
        glm::mat4 matriceVirage = glm::rotate(glm::mat4(1.0f), rotationChassis, axeHautLocal);
        
        // On applique ce virage à la matrice actuelle du châssis
        glm::mat4 nouvelleRotation = matriceVirage * rotationMatrix;
        
        // On utilise ta fonction modifiée pour remettre à jour les Euler proprement
        node->transformation.setRotationFromMatrix(nouvelleRotation);
        
        // La nouvelle direction Avant (Axe X) est la première colonne fraîchement tournée
        directionChassis = glm::normalize(glm::vec3(nouvelleRotation[0]));

        
        // Ajoute une petite friction pour que la voiture s'arrête si on n'accélère plus
        //speed *= 0.99f; 
        //std::cout << "speed: " << speed << std::endl;
    }else{
        speed -= (0.2f * speed + (airResistance/masse)) * dt;

        float airTurnSpeed = 1.f * dt;
        glm::mat4 matriceAir = glm::mat4(1.0f);

        if (acceleration > 0.0f) {
            float pitchForce = -acceleration * airTurnSpeed;
            matriceAir = glm::rotate(matriceAir, pitchForce, axeDroite);
        }else if (freinage > 0.0f) {
            float pitchForce = freinage * airTurnSpeed;
            matriceAir = glm::rotate(matriceAir, pitchForce, axeDroite);
        }
        if (std::abs(anglesRoues) > 0.05f) {
            float directionRotation = (anglesRoues > 0.0f) ? 1.0f : -1.0f;
            matriceAir = glm::rotate(matriceAir, directionRotation * airTurnSpeed, axeHaut);
        }

        // On applique les rotations aériennes à la matrice actuelle
        glm::mat4 nouvelleRotation = matriceAir * rotationMatrix;
        node->transformation.setRotationFromMatrix(nouvelleRotation);
        
        // On met à jour la nouvelle direction
        directionChassis = glm::normalize(glm::vec3(nouvelleRotation[0]));
    }

    // 4. Calcul de la rotation du châssis (Lacet)
    // On utilise l'angle des roues (attention : vérifie si c'est .y ou .z selon ton axe vertical)
/*     glm::mat4 rotMat  = node->transformation.getRotationMatrix();
    glm::vec3 chassisPos = node->transformation.getTranslation();
    glm::vec3 wheelWorld = chassisPos
            + glm::vec3(rotMat * glm::vec4(wheel->transformation.getTranslation(), 0.f));
    glm::vec3 diag1 = wheelWorldPositions[3] - wheelWorldPositions[0];
    glm::vec3 diag2 = wheelWorldPositions[2] - wheelWorldPositions[1];
    glm::vec3 terrainNormal = glm::normalize(glm::cross(diag1, diag2)); */


    //node->transformation.addEulerAngles(glm::vec3(0, rotationChassis, 0));

    // 5. DETERMINATION DU NOUVEAU VECTEUR VITESSE
    // Maintenant que le châssis a tourné, la vitesse pointe vers l'avant du châssis !
    // 5. DETERMINATION DU NOUVEAU VECTEUR VITESSE (Façon Trackmania)
    
    // a. On sépare les deux vitesses
   // 1. Calcul de base
    float angleAbsolu = std::abs(anglesRoues);
    float forceGrip = 15.0f; // Grip normal (rails)
    float multiplicateurSurvirage = 1.0f;

    // --- APPLICATION ROTATION CHÂSSIS ---
    float rotationChassis = (speed * tan(anglesRoues) / empattement) * dt;
    rotationChassis *= multiplicateurSurvirage; 
    glm::mat4 matriceVirage = glm::rotate(glm::mat4(1.0f), rotationChassis, axeHaut);
    glm::mat4 nouvelleRotation = matriceVirage * rotationMatrix;
    
    node->transformation.setRotationFromMatrix(nouvelleRotation);
    directionChassis = glm::normalize(glm::vec3(nouvelleRotation[0]));

    // 4. Calcul des vitesses avec le glissement
    // --- 4. ANALYSE DU DRIFT ---
    
    // On regarde à quelle vitesse la voiture glisse DÉJÀ
    glm::vec3 vitesseAvantBrute = directionChassis * speed;
    glm::vec3 vitesseLateraleBrute = v_vec - vitesseAvantBrute;
    float vitesseGlissement = glm::length(vitesseLateraleBrute);

    // NOUVELLE CONDITION : 
    // On drift SI (on tourne fort à haute vitesse) OU SI (on glisse déjà pas mal !)
    std::cout << "adherence: " << adherence << std::endl;
    bool isDrifting = (abs_speed > 15.0f && std::abs(anglesRoues) > (0.2f* adherence)) || (vitesseGlissement > 3.0f) || (freinage == 1.0f && acceleration == 1.0f);

    if (isDrifting) {
        forceGrip = 1.5f; // Grip faible = la glissade continue en douceur
        multiplicateurSurvirage = 2.0f; // On amplifie la rotation du capot (survirage)
    }


    
    node->transformation.setRotationFromMatrix(nouvelleRotation);
    directionChassis = glm::normalize(glm::vec3(nouvelleRotation[0]));

    // --- 5. NOUVELLES VITESSES ---
    // On recalcule avec la NOUVELLE direction
    glm::vec3 vitesseAvant = directionChassis * speed;
    glm::vec3 vitesseLaterale = v_vec - vitesseAvant;

    float frictionLaterale = std::max(0.0f, 1.0f - (forceGrip * dt));
    vitesseLaterale *= frictionLaterale;
    
    if (glm::length(vitesseLaterale) < 0.1f) vitesseLaterale = glm::vec3(0.0f);

    glm::vec3 nouveauV = vitesseAvant + vitesseLaterale;

    // --- L'ASTUCE ARCADE : NE PAS PERDRE DE VITESSE ---
    float energieInitiale = glm::length(v_vec);
    float energieNouvelle = glm::length(nouveauV);
    
    if (energieNouvelle > 0.1f && energieInitiale > energieNouvelle) {
        // Au lieu de perdre la vitesse, on la redirige vers l'avant !
        // 0.95f = on garde 95% de l'élan de la voiture pendant les virages/drifts. 
        float energieRedirigee = energieNouvelle + (energieInitiale - energieNouvelle) * 0.5f;
        nouveauV = glm::normalize(nouveauV) * energieRedirigee;
    }

    // -------------------------------------------------------------------
    // On applique aux roues
    auto roues = node->getEnfants();
    for (int i = 0; i < (int)roues.size(); i++) {
        if (collisionEnCours[i] == true) {
            roues[i]->setVitesse(nouveauV);
        }
    }

    // 6. Mise à jour de la position
    /* glm::vec3 translation = node->transformation.getTranslation();
    translation += nouveauV * dt;

    node->transformation.setTranslation(translation); */
}

void Car::solver(double dt, Map& map)
{
    const float restitution = 0.0f;
    const glm::vec3 gravity(0.f, -9.81f, 0.f);

    std::vector<Node*> roues = node->getEnfants();
    auto blocks = map.getBlocks();

    glm::mat4 rotMat  = node->transformation.getRotationMatrix();
    glm::vec3 chassisPos = node->transformation.getTranslation();

    std::vector<glm::vec3> wheelWorldPositions(roues.size());

    // ═══════════════════════════════════════════════
    // ÉTAPE 1 : Traitement individuel de chaque roue
    // ═══════════════════════════════════════════════
    
    for (int i = 0; i < (int)roues.size(); i++)
    {
        Node* wheel = roues[i];

        // Position mondiale actuelle de la roue
        glm::vec3 wheelWorld = chassisPos
            + glm::vec3(rotMat * glm::vec4(wheel->transformation.getTranslation(), 0.f));

        // 1. Gravité sur la vitesse de la roue
        
        glm::vec3 wheelVel = wheel->getVitesse();
        wheelVel += gravity * (float)dt;

        // 2. Position prédite
        glm::vec3 predicted = wheelWorld + wheelVel * (float)dt;

        // 3. Test de toutes les collisions pour cette roue
        float bestPenetration = 0.f;
        glm::vec3 bestNormal(0.f, 1.f, 0.f);
        bool hasCollision = false;
        glm::vec3 accumulatedNormal(0.0f);

        for (int bi = 0; bi < (int)blocks.size(); bi++)
        for (int bj = 0; bj < (int)blocks[bi].size(); bj++)
        for (int bk = 0; bk < (int)blocks[bi][bj].size(); bk++)
        {
            Node* nodeMap = blocks[bi][bj][bk];
            if (!nodeMap) continue;

            const auto& verts   = nodeMap->getMesh()->getIndexedVertices();
            const auto& indices = nodeMap->getMesh()->getIndices();
            Transformation mapTransfo = nodeMap->getTransformation();
            mapTransfo.addTranslation(glm::vec3(blockSize*bj - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, 0., blockSize*bi - (mapHeight * blockSize) / 2.0f + blockSize / 2.0f));
            glm::mat4 T = mapTransfo.computeTransformationMatrix();

            for (int t = 0; t < (int)indices.size(); t += 3)
            {
                glm::vec3 v0 = applyTransformation(verts[indices[t  ]], 1.f, T);
                glm::vec3 v1 = applyTransformation(verts[indices[t+1]], 1.f, T);
                glm::vec3 v2 = applyTransformation(verts[indices[t+2]], 1.f, T);


                /* glm::vec3 normal = glm::normalize(glm::cross(v1-v0, v2-v0));
                //std::cout << "Normal triangle " << t/3 << " : " << normal.x << " " << normal.y << " " << normal.z << std::endl;
                //if (normal.y < 0.f) normal = -normal;

                // Distance signée sphère → plan (on teste predicted, pas wheelWorld)
                float dist = glm::dot(predicted - v0, normal);
               

                // Projection sur le plan
                
                glm::vec3 projected = predicted - normal * dist;
                // Test barycentrique
                glm::vec3 v0v1 = v1 - v0;
                glm::vec3 v0v2 = v2 - v0;
                glm::vec3 v0p  = projected - v0;

                float d00 = glm::dot(v0v1, v0v1);
                float d01 = glm::dot(v0v1, v0v2);
                float d11 = glm::dot(v0v2, v0v2);
                float d20 = glm::dot(v0p , v0v1);
                float d21 = glm::dot(v0p , v0v2);

                float denom = d00 * d11 - d01 * d01;

                if (std::abs(denom) < 1e-8f)
                    continue;

                float v = (d11 * d20 - d01 * d21) / denom;
                float w = (d00 * d21 - d01 * d20) / denom;
                float u = 1.0f - v - w;
                const float EPSILON = 0.0005f;
                if (u < -EPSILON || v < -EPSILON || w < -EPSILON)
                    continue;

                float penetration = rayonRoue - dist;
                if (penetration > 0.f && std::abs(dist) < rayonRoue) // debug
                {
                    predicted += normal * penetration;
                    accumulatedNormal += normal;
                    hasCollision = true;
                    /* bestPenetration = penetration;
                    bestNormal      = normal; */
                
                glm::vec3 closest = closestPointOnTriangle(v0, v1, v2, predicted);

                // 2. On calcule la distance entre ce point et le centre de la roue
                glm::vec3 axeCollision = predicted - closest;
                float distSq = glm::dot(axeCollision, axeCollision); // Distance au carré
                float rayonSq = rayonRoue * rayonRoue;

                // 3. Si la distance est plus petite que le rayon : COLLISION !
                if (distSq < rayonSq && distSq > 0.0001f)
                {
                    float dist = std::sqrt(distSq);
                    float penetration = rayonRoue - dist;
                    glm::vec3 normal = axeCollision / dist; // La vraie direction de repousse

                    // CORRECTION IMMÉDIATE (Pour gérer murs + sol simultanément)
                    predicted += normal * penetration;
                    accumulatedNormal += normal;
                    hasCollision = true;
                    adherence = nodeMap->getAdherence();
                            
                }
            }
        }

        // 4. Correction position et vitesse si collision
        //std::cout << "Roues " << i << " : penetration = " << bestPenetration << std::endl;
        if (hasCollision)
        {
            /* predicted += bestNormal * bestPenetration;
            normalCollision = bestNormal; */
            glm::vec3 finalNormal = glm::normalize(accumulatedNormal);
            normalCollision = finalNormal;
            collisionEnCours[i] = true;

            float vn = glm::dot(wheelVel, finalNormal);
            if (vn < 0.f){ // on s'enfonce
                wheelVel -= (1.f + restitution) * vn * finalNormal;
                /* glm::vec3 axeDroite = glm::vec3(rotMat[2]); 
                
                // On calcule à quelle vitesse la roue dérape sur le côté
                float vitesseLaterale = glm::dot(wheelVel, axeDroite);
                
                // On annule presque tout ce dérapage (98% d'adhérence)
                // Cela force la roue à ne rouler que vers l'avant/l'arrière
                wheelVel -= axeDroite * (vitesseLaterale * 0.5f);*/
            }
        }else{
            collisionEnCours[i] = false;
        }

        wheel->setVitesse(wheelVel);
        wheelWorldPositions[i] = predicted;
        
    }

    // ═══════════════════════════════════════════════
    // ÉTAPE 2 : Reconstruire la transformation du châssis
    // ═══════════════════════════════════════════════

    // Nouvelle position = moyenne des roues + hauteur châssis
    glm::vec3 newCenter(0.f);
    for (auto& wp : wheelWorldPositions) newCenter += wp;
    newCenter /= (float)wheelWorldPositions.size();

    // Nouvelle orientation = plan formé par les 4 roues
    // Convention attendue : [0]=AvantGauche [1]=AvantDroit [2]=ArriereGauche [3]=ArriereDroit
    glm::vec3 diag1 = wheelWorldPositions[3] - wheelWorldPositions[0];
    glm::vec3 diag2 = wheelWorldPositions[2] - wheelWorldPositions[1];
    glm::vec3 terrainNormal = glm::normalize(glm::cross(diag1, diag2));
    //if (terrainNormal.y < 0.f) terrainNormal = -terrainNormal;

    glm::mat4 currentRot = node->transformation.getRotationMatrix(); 

    // 2. Extraire l'axe X (qui correspond à ton "Avant" dans ton moteur)
    glm::vec3 forward = glm::vec3(currentRot[0]);
    glm::vec3 right   = glm::vec3(currentRot[2]);

    /* // 3. Annuler l'inclinaison pour ne conserver que la direction (Yaw)
    forward.y = 0.f; 
    if (glm::length(forward) < 0.001f) forward = glm::vec3(1.f, 0.f, 0.f); 
    forward = glm::normalize(forward); */

    forward = glm::normalize(glm::cross(terrainNormal, right));
    right   = glm::normalize(glm::cross(forward, terrainNormal));

    // 4. Re-orthogonalisation par rapport à la pente du terrain
    /* glm::vec3 right   = glm::normalize(glm::cross(forward, terrainNormal));
    forward           = glm::normalize(glm::cross(terrainNormal, right));
 */
    // 5. Construire la nouvelle matrice parfaitement alignée
    glm::mat4 newRot(1.f); // Initialiser à l'identité
    newRot[0] = glm::vec4(forward,       0.f); // axe X = avant
    newRot[1] = glm::vec4(terrainNormal, 0.f); // axe Y = haut
    newRot[2] = glm::vec4(right,         0.f); // axe Z = droite

    // 6. Appliquer la nouvelle position et rotation
    newCenter += glm::vec3(newRot * glm::vec4(-1.f, 0.f, -0.5f, 0.f)); 
    node->transformation.setTranslation(newCenter);
    //std::cout << " New rotation : " << std::endl;
    //std::cout << newRot[0][0] << " " << newRot[0][1] << " " << newRot[0][2] << std::endl;
    //std::cout << newRot[1][0] << " " << newRot[1][1] << " " << newRot[1][2] << std::endl;
    //std::cout << newRot[2][0] << " " << newRot[2][1] << " " << newRot[2][2] << std::endl;

    node->transformation.setRotationFromMatrix(newRot);
    // ═══════════════════════════════════════════════
    // ÉTAPE 3 : Remettre les roues en local
    // ═══════════════════════════════════════════════
    glm::mat4 newRotInv = glm::transpose(newRot); // matrice orthogonale → transpose = inverse
    

    /* for (int i = 0; i < (int)roues.size(); i++)
    {
        // Position locale = rotation inverse * (mondiale - centre châssis)
        glm::vec3 localPos = glm::vec3(newRotInv * glm::vec4(wheelWorldPositions[i] - newCenter, 0.f));

        // On ne touche qu'au Y local pour préserver l'empattement/voie
        glm::vec3 origLocal = roues[i]->transformation.getTranslation();
        origLocal.y = localPos.y;
        roues[i]->transformation.setTranslation(origLocal);
    } */
    roues[2]->transformation.setTranslation(glm::vec3(0.2, 0., 1.));
    roues[3]->transformation.setTranslation(glm::vec3(0.2, 0., 0.));
    roues[0]->transformation.setTranslation(glm::vec3(1.8, 0., 1.));
    roues[1]->transformation.setTranslation(glm::vec3(1.8, 0., 0.));

    // Vitesse du châssis = moyenne des vitesses des roues
    glm::vec3 chassisVel(0.f);
    for (auto* w : roues) chassisVel += w->getVitesse();
    chassisVel /= (float)roues.size();
    node->setVitesse(chassisVel);
}


glm::vec3 Car::closestPointOnSegment(const glm::vec3& A, const glm::vec3& B, const glm::vec3& P) {
    glm::vec3 AB = B - A;
    float t = glm::clamp(glm::dot(P - A, AB) / glm::dot(AB, AB), 0.0f, 1.0f);
    return A + t * AB;
}

// 2. Trouve le point le plus proche sur un Triangle (Face, Arête ou Sommet)
glm::vec3 Car::closestPointOnTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P) {
    glm::vec3 AB = B - A, AC = C - A, AP = P - A;
    glm::vec3 n = glm::cross(AB, AC);
    
    // Test si la projection est dans le triangle
    glm::vec3 pProj = P - n * (glm::dot(AP, n) / glm::dot(n, n));
    glm::vec3 v0p = pProj - A;
    
    float d00 = glm::dot(AB, AB), d01 = glm::dot(AB, AC), d11 = glm::dot(AC, AC);
    float d20 = glm::dot(v0p, AB), d21 = glm::dot(v0p, AC);
    float denom = d00 * d11 - d01 * d01;
    
    if (std::abs(denom) > 1e-8f) {
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        if (v >= 0.f && w >= 0.f && (v + w) <= 1.f) return pProj; // Sur la face
    }
    
    // Si on est en dehors de la face, on regarde quelle arête est la plus proche !
    glm::vec3 p1 = closestPointOnSegment(A, B, P);
    glm::vec3 p2 = closestPointOnSegment(B, C, P);
    glm::vec3 p3 = closestPointOnSegment(C, A, P);
    
    float d1 = glm::dot(P - p1, P - p1);
    float d2 = glm::dot(P - p2, P - p2);
    float d3 = glm::dot(P - p3, P - p3);
    
    if (d1 <= d2 && d1 <= d3) return p1;
    if (d2 <= d1 && d2 <= d3) return p2;
    return p3;
}


/* void Car::solver(double dt, Map& map)
{
    const glm::vec3 gravity(0.0, 0.0, 0.0);
    const double restitution = 0.2;
    const double friction = 0.3;
    
    std::vector<Node*> roues = car.node->getEnfants();
    auto blocks = map.getBlocks();

    for (int i = 0; i < roues.size(); i++)
    {
        Node* wheel = roues[i];

        // 1️⃣ GRAVITÉ
        wheel->setVitesse(wheel->getVitesse() + gravity * dt);

        // 2️⃣ PREDICTION
        glm::vec3 predicted = wheel->transformation.getTranslation() + wheel->getVitesse() * dt;
        glm::vec3 predictedGlobal = node->transformation.getTranslation() + glm::vec3(node->transformation.getRotationMatrix() * glm::vec4(wheel->transformation.getTranslation(),0.)) + wheel->getVitesse() * dt;

        // 3️⃣ COLLISION ROUTE (triangles statiques)
        for (int bi = 0; bi < blocks.size(); bi++)
        {
            for (int bj = 0; bj < blocks[bi].size(); bj++)
            {
                for (int bk = 0; bk < blocks[bi][bj].size(); bk++)
                {
                    Node* nodeMap = blocks[bi][bj][bk];
                    if (!nodeMap) continue;

                    const auto& verts = nodeMap->getMesh()->getIndexedVertices();
                    const auto& indices = nodeMap->getMesh()->getIndices();
                    Transformation mapTransfo = nodeMap->getTransformation();
                    mapTransfo.addTranslation(glm::vec3(10.*bj - 45, 0., 10.*bi - 45));
                    glm::mat4 T = mapTransfo.computeTransformationMatrix();
                    for (int t = 0; t < indices.size(); t += 3)
                    {
                        glm::vec3 v0 = applyTransformation(verts[indices[t]], 1.f, T);
                        glm::vec3 v1 = applyTransformation(verts[indices[t + 1]], 1.f, T);
                        glm::vec3 v2 = applyTransformation(verts[indices[t + 2]], 1.f, T);

                        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                        if (normal.y < 0) normal = - normal;

                        // distance plan
                        double dist = glm::dot(predictedGlobal - v0, normal);

                        // projection sur plan
                        glm::vec3 projected = predictedGlobal - normal * dist;

                        // test barycentrique simple
                        glm::vec3 c0 = glm::cross(v1 - v0, projected - v0);
                        glm::vec3 c1 = glm::cross(v2 - v1, projected - v1);
                        glm::vec3 c2 = glm::cross(v0 - v2, projected - v2);

                        if (glm::dot(c0, normal) >= 0 &&
                            glm::dot(c1, normal) >= 0 &&
                            glm::dot(c2, normal) >= 0)
                        {
                            // collision sphère-plan
                            double penetration = rayonRoue - dist;

                            if (penetration > 0.0)
                            {   
                                std::cout << "Normal : " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
                                std::cout << "Collision detected! Penetration: " << penetration << std::endl;
                                // 4️⃣ PROJECTION
                                predicted += normal * penetration;

                                // 5️⃣ CORRECTION VITESSE
                                double vn = glm::dot(wheel->getVitesse(), normal);
                                glm::vec3 vN = vn * normal;
                                glm::vec3 vT = wheel->getVitesse() - vN;

                                vN *= -restitution;
                                //vT *= (1.0 - friction);
                                wheel->setVitesse(vN + vT);
                            }
                        }
                    }
                }
            }
        }

        // 6️⃣ UPDATE FINAL
        //wheel.setVitesse((predicted - wheel.getTransformation().getTranslation()) / dt);
        wheel->transformation.setTranslation(glm::vec3(predicted));
        

        // 7️⃣ APPLIQUER AU NODE
        //roues[i]->getTransformation().setTranslation(glm::vec3(wheel.position));
    }

    // 8️⃣ POSITION VOITURE = MOYENNE DES ROUES
    glm::dvec3 center(0.0);
    for (auto& w : roues)
        center += node->transformation.getTranslation() + glm::vec3(node->transformation.getRotationMatrix() * glm::vec4(w->transformation.getTranslation(),0.));

    center /= (double)roues.size();
    center += glm::vec3(-1, 0, -0.5); // relever un peu le centre pour éviter les collisions immédiates
    std::cout << "Car center: " << center.x << ", " << center.y << ", " << center.z << std::endl;
    node->transformation.setTranslation(glm::vec3(center));

} */

/* bool Car::solver(double _delta_time, Map map) {
    std::vector<Node*> Roues = car.node->getEnfants();
    std::vector<std::vector<std::vector<Node *>>> blocks = map.getBlocks();

    std::vector<std::vector<glm::dvec3>> new_positions(Roues.size());
    std::vector<std::vector<glm::dvec3>> m_positions(Roues.size()); // p_i
    std::vector<Transformation> newTransformations = {/&car.node->transformation, Roues[0]->transformation, Roues[1]->transformation, Roues[2]->transformation, Roues[3]->transformation};
    std::vector<Transformation> oldTransformations = { car.node->transformation,  Roues[0]->transformation, Roues[1]->transformation, Roues[2]->transformation, Roues[3]->transformation};
    // (5) external forces (gravity, etc...) (for now, just gravity)
    for (uint pj = 0; pj < Roues.size(); pj++)
        Roues[pj]->setVitesse(Roues[pj]->getVitesse() + _delta_time * glm::dvec3(0., -9.807, 0.));


    // (7)
    for (uint pj = 0; pj < Roues.size(); pj++)
        newTransformations[pj]->addTranslation(_delta_time * Roues[pj]->getVitesse());

    for (uint pj = 0; pj < Roues.size(); pj++) {
        for(uint nbPoints = 0; nbPoints < Roues[pj]->getMesh()->getIndexedVertices().size(); nbPoints++){
            new_positions[pj].push_back(applyTransformation(Roues[pj]->getMesh()->getIndexedVertices()[nbPoints], 1.f, newTransformations[pj]->computeTransformationMatrix()));
            m_positions[pj].push_back(applyTransformation(Roues[pj]->getMesh()->getIndexedVertices()[nbPoints], 1.f, oldTransformations[pj]->computeTransformationMatrix()));
        }
    }
   

    // (8)
    std::unordered_map<size_t, glm::dvec3> colliding_vertices;
    for (uint pj = 0; pj < Roues.size(); pj++) {
        for(uint nbPoints = 0; nbPoints < m_positions[pj].size(); nbPoints++){
            glm::dvec3 origin = m_positions[pj][nbPoints];
            glm::dvec3 direction = new_positions[pj][nbPoints] - m_positions[pj][nbPoints];

            for (int i = 0; i < blocks.size(); i++) {
                for (int j = 0; j < blocks[i].size(); j++) {
                    for (int k =0; k < blocks[i][j].size(); k++) {
                        Node* node = blocks[i][j][k];
                        if (node == nullptr) continue;
                        const std::vector<glm::vec3> &mapPositions = node->getMesh()->getIndexedVertices();
                        /* const std::vector<glm::vec3> &mesh_normals = static_body.m_mesh->vertexNormals();
                        const std::vector<glm::vec3> &mapTriangles = node->getMesh()->getIndices();
                        glm::mat4 mapTransformation = node->getTransformation().computeTransformationMatrix()+glm::dvec3(10.*j - 45, 0., 10.*i - 45);

                        double min_t = DBL_MAX, max_t = -DBL_MAX, t;
                        glm::dvec3 closest_intersection, closest_normal, intersection, barycentrics;

                        double min_dist = DBL_MAX, dist;
                        glm::dvec3 closest_surface, closest_surface_normal, surface;

                        for (int t = 0; t < Roues[pj].getMesh().getIndices().size(); t += 3) {
                            glm::dvec3 v0 = applyTransformation(mapPositions[mapTriangles[t].x], 1.f, mapTransformation);
                            glm::dvec3 v1 = applyTransformation(mapPositions[mapTriangles[t].y], 1.f, mapTransformation);
                            glm::dvec3 v2 = applyTransformation(mapPositions[mapTriangles[t].z], 1.f, mapTransformation);

                           /*  glm::dvec3 n0 = applyTransformation(mesh_normals[triangle[0]], 0.f, mapTransformation);
                            glm::dvec3 n1 = applyTransformation(mesh_normals[triangle[1]], 0.f, mapTransformation);
                            glm::dvec3 n2 = applyTransformation(mesh_normals[triangle[2]], 0.f, mapTransformation);
                            glm::dvec3 normal = glm::cross(v1 - v0, v2 - v0);

                            // ray intersections
                            if (!rayTriangleIntersection(origin, direction, v0, v1, v2, normal, t, intersection, barycentrics)) {
                                continue;
                            }
                            if (t < min_t) {
                                min_t = t;
                                closest_intersection = intersection;
                                closest_normal = barycentrics[0] * n0 + barycentrics[1] * n1 + barycentrics[2] * n2;
                            }

                            // closest surface
                            glm::dvec3 project_on_plane = glm::cross(normal, glm::cross(new_positions[pj] - v0, normal));
                            computeBarycentrics(v0, v1, v2, normal, project_on_plane, barycentrics);

                            // https://www.desmos.com/calculator/eeqkstj2ck
                            const auto fallback_in_triangle = [project_on_plane](const glm::dvec3 &p1, const glm::dvec3 &p2) {
                                glm::dvec3 direction = p2 - p1;
                                double n_squared = std::pow(glm::distance(p2, p1), 2);
                                double dot = glm::dot(direction, project_on_plane - p1);
                                double dot_over_one = dot / n_squared;
                                double r = std::max(0., std::min(1., dot_over_one));
                                return p1 + direction * r;
                            };

                            surface = barycentrics[0] < 0.   ? fallback_in_triangle(v1, v2)
                                    : barycentrics[1] < 0. ? fallback_in_triangle(v2, v0)
                                    : barycentrics[2] < 0. ? fallback_in_triangle(v0, v1)
                                                            : project_on_plane;

                            dist = glm::distance(new_positions[pj], surface);

                            if (dist < min_dist) {
                                min_dist = dist;
                                closest_surface = surface;
                                closest_surface_normal = n0 * barycentrics[0] + n1 * barycentrics[1] + n2 * barycentrics[2];
                            }
                        }

                        if (0. <= min_t && min_t <= 1.) {
                            // WILL ENTER THE OBJECT
                            addCollisionConstraint(pj, closest_intersection, glm::normalize(closest_normal), 1.);
                            colliding_vertices.insert({pj, glm::normalize(closest_normal)});
                        } else if (min_t < 0. && max_t > 1.) {
                            // COMPLETLY INSIDE THE OBJECT
                            addCollisionConstraint(pj, closest_surface, glm::normalize(closest_surface_normal), 1.);
                            colliding_vertices.insert({pj, glm::normalize(closest_surface_normal)});
                        }
                    }
                }
            }
        }
    }
    for (const glm::uvec2 &edge : m_lines) {
        uint e0 = edge[0];
        uint e1 = edge[1];

        glm::dvec3 origin = new_positions[e0];
        glm::dvec3 direction = new_positions[e1] - new_positions[e0];

        for (const StaticBody &static_body : static_bodies) {
            const std::vector<glm::vec3> &mesh_positions = static_body.m_mesh->vertexPositions();
            const std::vector<glm::vec3> &mesh_normals = static_body.m_mesh->vertexNormals();
            const std::vector<glm::uvec3> &mesh_triangles = static_body.m_mesh->triangleIndices();
            glm::mat4 transformation = static_body.m_transformation->computeTransformationMatrix();

            double min_t = DBL_MAX, t;
            glm::dvec3 closest_intersection, closest_normal, intersection, barycentrics;
            bool hit = false;

            for (const glm::uvec3 &triangle : mesh_triangles) {
                glm::dvec3 v0 = applyTransformation(mesh_positions[triangle[0]], 1.f, transformation);
                glm::dvec3 v1 = applyTransformation(mesh_positions[triangle[1]], 1.f, transformation);
                glm::dvec3 v2 = applyTransformation(mesh_positions[triangle[2]], 1.f, transformation);

                glm::dvec3 n0 = applyTransformation(mesh_normals[triangle[0]], 0.f, transformation);
                glm::dvec3 n1 = applyTransformation(mesh_normals[triangle[1]], 0.f, transformation);
                glm::dvec3 n2 = applyTransformation(mesh_normals[triangle[2]], 0.f, transformation);
                glm::dvec3 face_normal = glm::cross(v1 - v0, v2 - v0);

                if (!rayTriangleIntersection(origin, direction, v0, v1, v2, face_normal, t, intersection, barycentrics)) {
                    continue;
                }

                if (t >= 0.0 && t <= 1.0 && t < min_t) {
                    min_t = t;
                    closest_intersection = intersection;
                    closest_normal = glm::normalize(barycentrics[0] * n0 + barycentrics[1] * n1 + barycentrics[2] * n2);
                    hit = true;
                }
            }

            if (hit) {
                addEdgeCollisionConstraint(e0, e1, min_t, closest_intersection, closest_normal, 1.0);
            }
        }
    }

    // (9)-(11)
    for (uint i = 0; i < SOLVER_ITERATIONS; i++) {
        for (uint ci = 0; ci < M + Mcoll; ci++) {
            // gather function input (and total weight)
            std::vector<glm::dvec3> affected_points(m_cardinalities[ci]);
            double total_weigths = 0.;
            for (uint i = 0; i < m_cardinalities[ci]; i++) {
                uint pj = m_indices[ci][i];
                affected_points[i] = new_positions[pj];
                total_weigths += m_weights[pj];
            }
            if (total_weigths != total_weigths || total_weigths == 0.) {
                std::cerr << "invalid total weights : " << total_weigths << std::endl;
                return false;
            }

            double function_value = m_functions[ci](affected_points);
            if (m_types[ci] == INEQUALITY_CONSTRAINT && function_value >= 0.) {
                // The constraint is already satisfied so we don't project it
                continue;
            }

            // Determine S
            std::vector<glm::dvec3> gradients = m_gradients[ci](affected_points);
            double denominator = 0.;
            for (uint i = 0; i < m_cardinalities[ci]; i++) {
                denominator += length2(gradients[i]);
            }
            if (denominator != denominator || denominator == 0.) {
                std::cerr << "invalid denominator : " << denominator << std::endl;
                return false;
            }
            double s = function_value / denominator;

            // add the deltas
            for (uint i = 0; i < m_cardinalities[ci]; i++) {
                uint pj = m_indices[ci][i];
                glm::dvec3 delta_pj = -s * (double(m_cardinalities[ci]) * m_weights[pj] / total_weigths) * gradients[i];
                double k_prime = 1. - std::pow(1. - m_stiffnesses[ci], 1. / SOLVER_ITERATIONS);
                new_positions[pj] += k_prime * delta_pj;
            }
        }
    }

    // (12)-(15)
    for (uint pj = 0; pj < N; pj++) {
        m_velocities[pj] = (new_positions[pj] - m_positions[pj]) / _delta_time; // (13)
        m_positions[pj] = new_positions[pj];                                    // (14)
    }

    // (16)
    for (auto [pj, collision_normal] : colliding_vertices) {
        // Decompose velocity into normal and tangential components
        double v_dot_n = glm::dot(m_velocities[pj], collision_normal);
        glm::dvec3 v_normal = v_dot_n * collision_normal;
        glm::dvec3 v_tangent = m_velocities[pj] - v_normal;

        // Apply restitution (reflection in the direction of collision normal)
        // Negative sign because we reflect away from surface
        v_normal *= -m_restitution_coefficient;

        // Apply friction (dampen velocity perpendicular to collision normal)
        v_tangent *= (1. - m_friction_coefficient);

        // Reconstruct velocity
        m_velocities[pj] = v_normal + v_tangent;
    }

    // cancel collision constraitns
    Mcoll = 0;
    m_cardinalities.resize(M);
    m_indices.resize(M);
    m_stiffnesses.resize(M);
    m_types.resize(M);
    m_functions.resize(M);
    m_gradients.resize(M);

    applyTearing(4.f);

    return true;
}
 */
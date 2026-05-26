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


void Car::calculPosition(float dt, float acceleration, float freinage) {
    int numCollisions = 0;
    for (int i = 0; i < collisionEnCours.size(); i++) {
        if (collisionEnCours[i]) {
            numCollisions++;
        }
    }
    glm::vec3 v_vec = node->getVitesse();
    glm::mat4 rotationMatrix = node->transformation.getRotationMatrix();
    // avant = (1, 0, 0)
    glm::vec3 directionChassis = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    float speed = glm::dot(v_vec, directionChassis);
    float abs_speed = std::abs(speed);
    float airResistance = 0.005f * abs_speed * speed;
    float masse = node->getMasse();
    float speedAdditionnel = 0.0f;
    if (abs_speed < 100.0f) {
        speedAdditionnel = 28.f * dt;
    }else if (abs_speed < 160.0f) {
        speedAdditionnel = 16.f * dt;
    }else if (abs_speed < 230.0f) {
        speedAdditionnel = 9.f * dt;
    }else{
        speedAdditionnel = 5.f * dt;
    }


    glm::vec3 axeAvant  = directionChassis;
    glm::vec3 axeHaut   = glm::normalize(glm::vec3(rotationMatrix[1])); // Axe Y local
    glm::vec3 axeDroite = glm::normalize(glm::vec3(rotationMatrix[2]));
    float anglesRoues = node->getEnfants()[0]->transformation.getEulerAngles().y; 
    float empattement = glm::distance(node->getEnfants()[0]->transformation.getTranslation(), 
                                      node->getEnfants()[2]->transformation.getTranslation());

    if (numCollisions != 0) {
        
       
        float ancienSpeed = speed;
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

        if (acceleration > 0.0f) {
            speed += speedAdditionnel * adherence;
            
        } else if (freinage > 0.0f) {
            if (speed > 0.1f) {
                speed -= 1.f * speed * dt + 50.0f * dt;
                if (speed < 0.0f) speed = 0.0f;
            } else {
                speed -= ((force + airResistance) / masse) * dt;
            }
            
        } else {
            // AUCUNE TOUCHE : Friction naturelle
            speed -= (0.4f * speed + (airResistance/masse)) * dt;
            if (abs_speed < 0.1f) speed = 0.0f;
        }
        
        
    }else{
        //speed -= (0.2f * speed + (airResistance/masse)) * dt;
        //speed += speedAdditionnel * adherence;
        
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
        
        glm::mat4 nouvelleRotation = matriceAir * rotationMatrix;
        node->transformation.setRotationFromMatrix(nouvelleRotation);
        
        directionChassis = glm::normalize(glm::vec3(nouvelleRotation[0]));
    }
    
    
    float angleAbsolu = std::abs(anglesRoues);
    float forceGrip = 15.0f; // Grip normal (rails)
    float multiplicateurSurvirage = 1.0f;
    
    
    glm::vec3 vitesseAvantBrute = directionChassis * speed;
    glm::vec3 vitesseLateraleBrute = v_vec - vitesseAvantBrute;
    float vitesseGlissement = glm::length(vitesseLateraleBrute);
    
    
    bool isDrifting = (abs_speed > 30.0f * adherence && std::abs(anglesRoues) > 0.8f * adherence) || ((vitesseGlissement > adherence * abs_speed *0.5) && abs_speed > 2.0f) || (freinage == 1.0f && acceleration == 1.0f);
    if(isDrifting) {
    }
    
    float speedFactor = glm::clamp(abs_speed / 40.0f, 0.2f, 1.0f);

    float turnSpeed = anglesRoues * 2.5f * speedFactor;

    if (isDrifting)
        turnSpeed *= 1.5f;
    
    float directionMultiplier = (speed >= 0.0f) ? 1.0f : -1.0f;
    if (speed > -0.1f && speed < 0.1f) {
        directionMultiplier = 0.f;
    }

    float rotationChassis = turnSpeed * directionMultiplier * dt;

    glm::mat4 matriceVirage =
        glm::rotate(glm::mat4(1.0f),
                    rotationChassis,
                    axeHaut);

    glm::mat4 nouvelleRotation =
        matriceVirage * rotationMatrix;

    node->transformation.setRotationFromMatrix(nouvelleRotation);

    directionChassis =
        glm::normalize(glm::vec3(nouvelleRotation[0]));

    glm::vec3 targetVelocity =
        directionChassis * speed;

    float grip =
        isDrifting ? 2.0f : 12.0f;

    float gripLerp = 1.0f - glm::exp(-grip * dt);
    glm::vec3 nouveauV = glm::mix(v_vec, targetVelocity, gripLerp);
    

    // -------------------------------------------------------------------
    auto roues = node->getEnfants();
    for (int i = 0; i < (int)roues.size(); i++) {
        if (collisionEnCours[i] == true) {
            roues[i]->setVitesse(nouveauV);
        }
    }
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
    glm::vec3 axeHaut   = glm::normalize(glm::vec3(rotMat[1]));
    
    for (int i = 0; i < (int)roues.size(); i++)
    {
        Node* wheel = roues[i];

        // Position mondiale actuelle de la roue
        glm::vec3 wheelWorld = chassisPos
            + glm::vec3(rotMat * glm::vec4(wheel->transformation.getTranslation(), 0.f));

        
        glm::vec3 wheelVel = wheel->getVitesse();
        wheelVel += gravity * (float)dt;

        glm::vec3 predicted = wheelWorld + wheelVel * (float)dt;

        float bestPenetration = 0.f;
        glm::vec3 bestNormal(0.f, 1.f, 0.f);
        bool hasCollision = false;
        glm::vec3 accumulatedNormal(0.0f);
        float slow = 1.f;
        glm::vec3 PoisitonCar = node->transformation.getTranslation();
        int xmap = (int)std::floor((PoisitonCar.x + (mapWidth * blockSize) / 2.0f ) / blockSize);
        int zmap = (int)std::floor((PoisitonCar.z + (mapDepth * blockSize) / 2.0f) / blockSize);
        int ymap = (int)std::floor((PoisitonCar.y + (mapHeight * blockSize) / 2.0f ) / blockSize);

        for (int bi = std::max(0, zmap-1); bi < std::min((int)blocks.size(), zmap+2); bi++)
        for (int bj = std::max(0, xmap-1); bj < std::min((int)blocks[bi].size(), xmap+2); bj++)
        for (int bk = std::max(0, ymap-1); bk < std::min((int)blocks[bi][bj].size(), ymap+2); bk++)
        for (int bl = 0; bl < (int)blocks[bi][bj][bk].size(); bl++)
        {
            Node* nodeMap = blocks[bi][bj][bk][bl];
            if (!nodeMap) continue;

            const auto& verts   = nodeMap->getMesh()->getIndexedVertices();
            const auto& indices = nodeMap->getMesh()->getIndices();
            Transformation mapTransfo = nodeMap->getTransformation();
            mapTransfo.addTranslation(glm::vec3(blockSize*bj - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, blockSize*bk - (mapHeight * blockSize) / 2.0f, blockSize*bi - (mapDepth * blockSize) / 2.0f + blockSize / 2.0f));
            glm::mat4 T = mapTransfo.computeTransformationMatrix();

            for (int t = 0; t < (int)indices.size(); t += 3)
            {
                glm::vec3 v0 = applyTransformation(verts[indices[t  ]], 1.f, T);
                glm::vec3 v1 = applyTransformation(verts[indices[t+1]], 1.f, T);
                glm::vec3 v2 = applyTransformation(verts[indices[t+2]], 1.f, T);

                glm::vec3 closest = closestPointOnTriangle(v0, v1, v2, predicted);

                glm::vec3 axeCollision = predicted - closest;
                float distSq = glm::dot(axeCollision, axeCollision); // Distance au carré
                float rayonSq = rayonRoue * rayonRoue;

                if (distSq < rayonSq && distSq > 0.0001f)
                {
                    float dist = std::sqrt(distSq);
                    float penetration = rayonRoue - dist;
                    glm::vec3 normal = axeCollision / dist; // La vraie direction de repousse
                    
                    if (glm::dot(glm::normalize(normal), glm::normalize(axeHaut)) < -0.5f) {
                        slow = 0.f;
                    }else if (glm::dot(glm::normalize(normal), glm::normalize(axeHaut)) < 0.5f) {
                        slow = 0.6f;
                    }else {
                        slow = 1.0f;
                    }

                    predicted += normal * penetration;
                    accumulatedNormal += normal;
                    hasCollision = true;
                    adherence = nodeMap->getAdherence();

                    //std::cout << "Collision roue " << i << " avec bloc (" << bj << "," << bk << "," << bi << "," << bl << ")"<< std::endl;
                    if(map.getStartTime() == -1.0){
                        map.setStartTime(glfwGetTime());
                        std::cout << "Course commencée !" << std::endl;
                    }
                    
                }
            }
        }

        if (hasCollision)
        {
            glm::vec3 finalNormal = glm::normalize(accumulatedNormal);
            normalCollision = finalNormal;
            collisionEnCours[i] = true;

            float vn = glm::dot(wheelVel, finalNormal);
            if (vn < 0.f){ // on s'enfonce
                wheelVel -= (1.f + restitution) * vn * finalNormal;
            }
        }else{
            collisionEnCours[i] = false;
        }

        wheel->setVitesse(wheelVel * slow);
        wheelWorldPositions[i] = predicted;
        
    }

    bool isFinishLine = (map.getCurrentCP() >= map.getCheckPoints().size());
    auto cp = isFinishLine ? map.getFinish() : map.getCheckPoints()[map.getCurrentCP()];

    glm::vec3 cpWorldTranslation = glm::vec3(
        blockSize * cp.x - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f,
        blockSize * cp.y - (mapHeight * blockSize) / 2.0f,
        blockSize * cp.z - (mapDepth * blockSize) / 2.0f + blockSize / 2.0f
    );

    Transformation cpTransfo = cp.node->getTransformation();
    cpTransfo.addTranslation(cpWorldTranslation);

    glm::mat4 cpMatrix = cpTransfo.computeTransformationMatrix();
    glm::mat4 inverseCpMatrix = glm::inverse(cpMatrix);

    float width = blockSize;
    float height = 6.0f;
    float depth = 0.5f;

    glm::vec3 minAABB = glm::vec3(-width / 2.0f, 0.0f,   -depth / 2.0f);
    glm::vec3 maxAABB = glm::vec3( width / 2.0f, height,  2.f);

    for (const auto& wp : wheelWorldPositions) {
        
        // Passage de la roue en espace local du checkpoint
        glm::vec4 localWheelPos = inverseCpMatrix * glm::vec4(wp, 1.0f);
        bool isInside = (localWheelPos.x >= minAABB.x && localWheelPos.x <= maxAABB.x) &&
                        (localWheelPos.y >= minAABB.y && localWheelPos.y <= maxAABB.y) &&
                        (localWheelPos.z >= minAABB.z && localWheelPos.z <= maxAABB.z);

        if (isInside) {
            if (!isFinishLine) {
                map.addTime(glfwGetTime() - map.getStartTime());
                speedCheckpoints = node->getVitesse(); // Attention, utilise 'cp.node' et pas juste 'node'
                transformationCheckpoints = node->getTransformation();
                std::cout << "Checkpoint " << map.getCurrentCP() << " reached! Time: " << map.getTimes().back() << " seconds" << std::endl;
                
                // map.setCurrentCP(map.getCurrentCP() + 1); // Ne pas oublier de passer au CP suivant
            } else if (map.getFinishTime() < 0.0f) {
                double finishTime = glfwGetTime() - map.getStartTime();
                map.setFinishTime(finishTime);
                std::cout << "Finish reached! Total time: " << finishTime << " seconds" << std::endl;
            }
            
            break;
        }
    }


    // Nouvelle position = moyenne des roues + hauteur châssis
    glm::vec3 newCenter(0.f);
    for (auto& wp : wheelWorldPositions) newCenter += wp;
    newCenter /= (float)wheelWorldPositions.size();

    // Nouvelle orientation = plan formé par les 4 roues
    glm::vec3 diag1 = wheelWorldPositions[3] - wheelWorldPositions[0];
    glm::vec3 diag2 = wheelWorldPositions[2] - wheelWorldPositions[1];
    glm::vec3 terrainNormal = glm::normalize(glm::cross(diag1, diag2));
    //if (terrainNormal.y < 0.f) terrainNormal = -terrainNormal;
    glm::vec3 hautVoiture = glm::vec3(rotMat[1]);

    if (glm::dot(terrainNormal, hautVoiture) < 0.0f) {
        terrainNormal = -terrainNormal;
    }
    glm::mat4 currentRot = node->transformation.getRotationMatrix(); 

    glm::vec3 forward = glm::vec3(currentRot[0]);
    glm::vec3 right   = glm::vec3(currentRot[2]);

    forward = glm::normalize(glm::cross(terrainNormal, right));
    right   = glm::normalize(glm::cross(forward, terrainNormal));

    glm::mat4 newRot(1.f);
    newRot[0] = glm::vec4(forward,       0.f); // axe X = avant
    newRot[1] = glm::vec4(terrainNormal, 0.f); // axe Y = haut
    newRot[2] = glm::vec4(right,         0.f); // axe Z = droite

    newCenter += glm::vec3(newRot * glm::vec4(offsetChassis, 0.f));
    node->transformation.setTranslation(newCenter);

    node->transformation.setRotationFromMatrix(newRot);
    glm::mat4 newRotInv = glm::transpose(newRot); // matrice orthogonale → transpose = inverse
    

    for (int i = 0; i < (int)roues.size(); i++)
    {
        glm::vec3 localPos = glm::vec3(newRotInv * glm::vec4(wheelWorldPositions[i] - newCenter, 0.f));

        glm::vec3 stableLocal = positionsInitialesRoues[i];
        //stableLocal.y = localPos.y;
        roues[i]->transformation.setTranslation(stableLocal);
    }

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

glm::vec3 Car::closestPointOnTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P) {
    glm::vec3 AB = B - A, AC = C - A, AP = P - A;
    glm::vec3 n = glm::cross(AB, AC);
    
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
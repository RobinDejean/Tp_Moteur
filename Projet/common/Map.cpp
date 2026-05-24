#include "Map.hpp"

Block::Block(int x, int y, int z, int n, Node* node) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->n = n;
    this->node = node;
}

Map::Map(int width, int height, int depth) {
    blocks.resize(depth);
    for (auto& layer : blocks) {
        layer.resize(height);
        for (auto& row : layer) {
            row.resize(width);
        }
    }
}

Map::~Map() {
    
}

void Map::addNode(int x, int y, int z, Node * node) {
    if (z >= 0 && z < blocks.size() && x >= 0 && x < blocks[z].size() && y >= 0 && y < blocks[z][x].size()) {
        blocks[z][x][y].push_back(node);
    } else {
        std::cerr << "Error: Coordinates out of bounds in addNode(" << x << ", " << y << ", " << z << ")\n";
    }
}

void Map::render(GLuint MatrixID, glm::mat4 viewProj, GLuint programID) {
    auto checkpoints = getCheckPoints();
     for (const auto& checkpoint : checkpoints) {
        Transformation t = Transformation();
        t.setTranslation(glm::vec3(checkpoint.x * blockSize - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, checkpoint.y * blockSize - (mapHeight * blockSize) / 2.0f, checkpoint.z * blockSize - (mapDepth * blockSize) / 2.0f + blockSize / 2.0f));
        glm::mat4 transformationParent = t.computeTransformationMatrix();
        if (checkpoint.node->getMesh() != nullptr){
            glm::mat4 modelMatrix = transformationParent * checkpoint.node->getTransformation().computeTransformationMatrix();
            glm::mat4 MVP = viewProj * modelMatrix;

            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));

            glUniform1i(glGetUniformLocation(programID, "mode"), checkpoint.node->getMode());

            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, *(checkpoint.node->getTextureID()));

            (*(checkpoint.node->getMesh())).render();
        }
    }

    auto finish = getFinish();
    Transformation t = Transformation();
    t.setTranslation(glm::vec3(finish.x * blockSize - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, finish.y * blockSize - (mapHeight * blockSize) / 2.0f, finish.z * blockSize - (mapDepth * blockSize) / 2.0f + blockSize / 2.0f));
    glm::mat4 transformationParent = t.computeTransformationMatrix();
    if (finish.node->getMesh() != nullptr){
        glm::mat4 modelMatrix = transformationParent * finish.node->getTransformation().computeTransformationMatrix();
        glm::mat4 MVP = viewProj * modelMatrix;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));

        glUniform1i(glGetUniformLocation(programID, "mode"), finish.node->getMode());

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, *(finish.node->getTextureID())); 

        (*(finish.node->getMesh())).render();
    }
    for (int z = 0; z < blocks.size(); ++z) {
        for (int x = 0; x < blocks[z].size(); ++x) {
            for (int y = 0; y < blocks[z][x].size(); ++y) {
                Transformation t = Transformation();
                t.setTranslation(glm::vec3(x * blockSize - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, y * blockSize - (mapHeight * blockSize) / 2.0f, z * blockSize - (mapDepth * blockSize) / 2.0f + blockSize / 2.0f));
                glm::mat4 transformationParent = t.computeTransformationMatrix();
                for (int n = 0; n < blocks[z][x][y].size(); ++n) {
                    Node *node = blocks[z][x][y][n];
                    if (node->getMesh() != nullptr){
                        glm::mat4 modelMatrix = transformationParent * node->getTransformation().computeTransformationMatrix();
                        glm::mat4 MVP = viewProj * modelMatrix;
        
                        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));

                        glUniform1i(glGetUniformLocation(programID, "mode"), node->getMode());

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, *(node->getTextureID())); 

                        (*(node->getMesh())).render();
                    }
                }
            }
        }
    }
}
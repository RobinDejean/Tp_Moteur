#include "Map.hpp"

Map::Map(int width, int height) {
    blocks.resize(height);
    for (auto& row : blocks) {
        row.resize(width);
    }
}

Map::~Map() {
    
}

void Map::addNode(int x, int y, Node * node) {
    if (y >= 0 && y < blocks.size() && x >= 0 && x < blocks[y].size()) {
        blocks[y][x].push_back(node);
    } else {
        std::cerr << "Error: Coordinates out of bounds in addNode(" << x << ", " << y << ")\n";
    }
}

void Map::render(GLuint MatrixID, glm::mat4 viewProj, GLuint programID) {
    for (int y = 0; y < blocks.size(); ++y) {
        for (int x = 0; x < blocks[y].size(); ++x) {
            Transformation t = Transformation();
            t.setTranslation(glm::vec3(x * blockSize - (mapWidth * blockSize) / 2.0f + blockSize / 2.0f, 0, y * blockSize - (mapHeight * blockSize) / 2.0f + blockSize / 2.0f));
            glm::mat4 transformationParent = t.computeTransformationMatrix();
            for (Node *node : blocks[y][x]) {
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
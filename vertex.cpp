#include "vertex.h"

int Vertex::counter = 0;

Vertex::Vertex()
    :pos(0, 0, 0), edge(nullptr), iD(counter), bound(false)
{
    QListWidgetItem::setText(QString::number(iD));
    counter++;
}

Vertex::Vertex(glm::vec3 iPos, HalfEdge &iEdge)
    :pos(iPos), edge(&iEdge), iD(counter), bound(false)
{
    QListWidgetItem::setText(QString::number(iD));
    counter++;
}

void Vertex::resetCounter()
{
    counter = 0;
}

// Finds the closest joints in the skeleton and stores pointers and influence values for them
void Vertex::bindJoints(Joint *root)
{
    skin.clear();
    influence.clear();
    std::vector<Joint *> joints;
    retrieveJoints(root, joints);
    std::array<Joint *, 2> final;
    std::array<float, 2> finalDist;
    int maxIndex = 0;
    float maxLen = glm::length(glm::vec4(pos, 1) - root->getOverallTransformation() * glm::vec4(0, 0, 0, 1));
    final[0] = root;
    finalDist[0] = maxLen;
    for (int i = 1; i < 2; i++){
        final[i] = joints[i];
        glm::vec4 d = joints[i]->getOverallTransformation() * glm::vec4(0, 0, 0, 1);
        float len = glm::length(glm::vec4(pos, 1) - d);
        finalDist[i] = len;
        if (len > maxLen){
            maxIndex = i;
            maxLen = len;
        }
    }
    for (int i = 2; i < joints.size(); i++){
        glm::vec4 d = joints[i]->getOverallTransformation() * glm::vec4(0, 0, 0, 1);
        float len = glm::length(glm::vec4(pos, 1) - d);
        if (len < maxLen){
            final[maxIndex] = joints[i];
            finalDist[maxIndex] = len;
            maxLen = len;
            for (int j = 0; j < 2; j++){
                if (finalDist[j] >= maxLen){
                    maxIndex = j;
                    maxLen = finalDist[j];
                }
            }
        }
    }
    float sum = finalDist[0] + finalDist[1];
    for (int i = 0; i < 2; i++){
        skin.push_back(final[i]);
        influence.push_back(1.0 - (finalDist[i]/sum));
    }
    bound = true;
}

#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <halfedge.h>
#include <QListWidget>
#include <joint.h>

// Forward declaration to deal with circular dependency
class HalfEdge;

class Vertex : public QListWidgetItem
{
private:

    // iD counter, used to assign sequentially increasing
    // numbers to faces
    static int counter;
public:

    //Default constructor for vertex
    Vertex();
    Vertex(glm::vec3 iPos, HalfEdge &iEdge);

    glm::vec3 pos;

    HalfEdge *edge;

    bool bound;

    // iD never changes so it's been made const
    const unsigned int iD;

    std::vector<Joint *> skin;
    std::vector<float> influence;

    void bindJoints(Joint *root);

    static void resetCounter();
};

#endif // VERTEX_H

#ifndef FACE_H
#define FACE_H

#include <glm/glm.hpp>
#include <halfedge.h>
#include <QListWidget>

// Forward declaration to deal with circular dependency
class HalfEdge;

class Face : public QListWidgetItem
{
private:

    // iD counter, used to assign sequentially increasing
    // numbers to faces
    static int counter;
public:

    //Default constructor for face
    Face();
    Face(glm::vec3 iColour, HalfEdge &iEdge);

    glm::vec3 colour;

    HalfEdge *edge;

    // iD never changes so it's been made const
    const unsigned int iD;

    static void resetCounter();
};

#endif // FACE_H

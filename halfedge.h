#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <glm/glm.hpp>
#include <face.h>
#include <vertex.h>

// Forward declarations to deal with circular dependency
class Face;
class Vertex;

class HalfEdge : public QListWidgetItem
{
private:

    // iD counter, used to assign sequentially increasing
    // numbers to faces
    static int counter;
public:

    //Default constructor for half edges
    HalfEdge();
    HalfEdge(HalfEdge &iNext, HalfEdge &iSym, Face &iFace, Vertex &iVert);

    HalfEdge *next;
    HalfEdge *sym;

    Face *face;

    Vertex *vert;

    // iD never changes so it's been made const
    const unsigned int iD;

    static void resetCounter();
};

#endif // HALFEDGE_H

#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <face.h>
#include <vertex.h>
#include <halfedge.h>
#include <memory>
#include <vector>
#include <drawable.h>
#include <fstream>

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext *context);

    std::vector <std::unique_ptr <HalfEdge>> halfEdges;
    std::vector <std::unique_ptr <Vertex>> vertices;
    std::vector <std::unique_ptr <Face>> faces;

    // These functions retrieve raw C style pointers for the
    // component with the selected index (index in the vector)
    HalfEdge *edgePtr(int index);
    Vertex *vertPtr(int index);
    Face *facePtr(int index);

    void create() override;

    // This function creates a cube in the mesh instance
    void createCube();

    // Constructs a mesh from a given .obj file
    void createFromOBJ(std::string fileName);

    // Takes in face info from createFromOBJ() and
    // adds a face to the mesh
    void constructFace(std::vector<int> verts);

    // Collapses dummy edges created for the sym info
    // from the .obj file
    void mergeExtraneous();

    // Clears all the geometry in the mesh
    void resetMesh();

    ~Mesh();
};

#endif // MESH_H

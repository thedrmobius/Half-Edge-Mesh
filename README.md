# Half Edge Mesh
The mesh data structure I made for my mesh editor project in Qt

Structure Detail
----------------------
The Mesh structure contains a face, vertex, and half-edge. A half edge is like a normal edge, except it only goes in one direction. Each face points to a half-edge that coincides on that face, each vertex points to the half-edge that points to it, and each half-edge points to the face that it coincides on, the half-edge that comes after it, the half-edge that makes the other half of the edge; the one that goes in the opposite direction, and the vertex that it shares with the next half-edge. Each of those classes contain a static counter that assigns unique iDs to every piece of geometry created. The mesh data structure contains a vector of faces, a vector of half-edges, and a vector of vertices.

Face, HalfEdge, and Vertex all inherit QListWidgetItem so that they can be displayed on the Qt GUI. Mesh inherits Drawable, which is a class that works with the OpenGL framework of this project.


# Classes

**Face**
- int counter: static integer to assign unique iDs to each face created
- glm::vec3 colour: the colour of the face
- HalfEdge \*edge: a pointer to a half-edge that coincides on the face
- const unsigned int iD: the iD of the face

**Vertex**
- int counter: static integer to assign unique iDs to each vertex created
- glm::vec3 pos: the position of the vertex in space
- HalfEdge \*edge: a pointer to the half-edge that points to the vertex
- bool bound: a boolean value determining whether the vertex is already bound to the loaded skeleton
- const unsigned int iD: the iD of the vertex

**Half-Edge**
- int counter: static integer to assign unique iDs to each half-edge created
- HalfEdge \*next: a pointer to the half-edge that succeeds the current one in counterclockwise order
- HalfEdge \*sym: a pointer to the half-edge goes in the opposite direction (the other half of the whole edge)
- Face \*face: a pointer to the face the half-edge coincides on
- Vertex \*vert: a pointer to the vertex the half-edge points towards
- const unsigned int iD: the iD of the vertex

**Mesh**
- std::vector <std::unique_ptr <HalfEdge>> halfEdges: a vector of unique pointers pointing to all the half-edges in the mesh
- std::vector <std::unique_ptr <Face>> faces: a vector of unique pointers pointing to all the faces in the mesh
- std::vector <std::unique_ptr <Vertex>> vertices: a vector of unique pointers pointing to all the vertices in the mesh
- HalfEdge *edgePtr(int index): returns a c-style pointer to the half-edge in the given index
- Face *edgePtr(int index): returns a c-style pointer to the face in the given index
- Vertex *edgePtr(int index): returns a c-style pointer to the vertex in the given index
- void createCube(): populates the mesh data structure with a cube (hard-coded)
- void createFromOBJ(std::string fileName): populates the mesh data structure with a mesh from a .obj file
- void mergeExtraneous(): supplemental function to help populate the mesh with data from a .obj file
- void resetMesh(): clears all the geometry in the mesh

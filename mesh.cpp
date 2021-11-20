#include "mesh.h"
#include <iostream>
#include <smartpointerhelp.h>
#include <unordered_map>

Mesh::Mesh(OpenGLContext *context)
    : Drawable(context)
{}

HalfEdge *Mesh::edgePtr(int index)
{
    return halfEdges[index].get();
}

Vertex *Mesh::vertPtr(int index)
{
    return vertices[index].get();
}

Face *Mesh::facePtr(int index)
{
    return faces[index].get();
}

void Mesh::create()
{

    // The VBOs for position, normals, colour, and index, respectively
    std::vector <glm::vec4> pos;
    std::vector <glm::vec4> nor;
    std::vector <glm::vec4> col;
    std::vector <GLuint> idx;
    std::vector <int> jnt;
    std::vector <float> inf;
    bool vertsBound = false;


    // We loop through the faces in order to set the normals and colours per face
    // and in order to triangulate the indices per face.
    for (std::unique_ptr<Face> &f : faces){

        // The vector of vertices in a given face
        std::vector<Vertex> faceVerts;

        // the size of the position vector, used as an index offset for triangulation
        int vertIdx = pos.size();

        // used to loop through the face; when firstEdge == currentEdge, we know that
        // we've completed a loop around the face
        HalfEdge *firstEdge = f.get()->edge;
        HalfEdge *currentEdge = firstEdge;

        // This variable stores the normal for every vertex we go through
        glm::vec3 normal;
        do{
            Vertex &v = *currentEdge->vert;
            faceVerts.push_back(v);
            pos.push_back(glm::vec4(v.pos[0], v.pos[1], v.pos[2], 1));

            if (v.bound){
                vertsBound = true;
                jnt.push_back(v.skin[0]->iD);
                jnt.push_back(v.skin[1]->iD);

                inf.push_back(v.influence[0]);
                inf.push_back(v.influence[1]);
            }
            // a colour element is pushed back for every position since there must be a
            // 1:1 relationship between them
            col.push_back(glm::vec4(f.get()->colour, 1.f));
            currentEdge = (currentEdge->next);
        } while (currentEdge != firstEdge);
        normal = -glm::normalize(glm::cross(faceVerts[0].pos - faceVerts[1].pos,
                                           faceVerts[0].pos - faceVerts[faceVerts.size() - 1].pos));
        nor.push_back(glm::vec4(normal, 0));
        for (int i = 1; i < faceVerts.size() - 1; i++){
            normal = -glm::normalize(glm::cross(faceVerts[i].pos - faceVerts[i + 1 % faceVerts.size()].pos,
                                               faceVerts[i].pos - faceVerts[i - 1 % faceVerts.size()].pos));
            nor.push_back(glm::vec4(normal, 0));

            // The triangulation part
            idx.push_back(vertIdx);
            idx.push_back(vertIdx + i);
            idx.push_back(vertIdx + i + 1);
        }

        // The last normal is pushed back independently because to has to loop back to 0
        normal = -glm::normalize(glm::cross(faceVerts[faceVerts.size() - 1].pos - faceVerts[0].pos,
                                           faceVerts[faceVerts.size() - 1].pos - faceVerts[faceVerts.size() - 2].pos));
        nor.push_back(glm::vec4(normal, 0));
        faceVerts.clear();
    }

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

    if (vertsBound){
        generateJnt();
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJnt);
        mp_context->glBufferData(GL_ARRAY_BUFFER, jnt.size() * sizeof(GLuint), jnt.data(), GL_STATIC_DRAW);

        generateInf();
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufInf);
        mp_context->glBufferData(GL_ARRAY_BUFFER, inf.size() * sizeof(GLfloat), inf.data(), GL_STATIC_DRAW);
    }
}

Mesh::~Mesh()
{}

void Mesh::createCube()
{
    resetMesh();
    faces.push_back(mkU<Face>());
    vertices.push_back(mkU<Vertex>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(0)->face = facePtr(0);
    edgePtr(0)->vert = vertPtr(0);
    facePtr(0)->edge = edgePtr(0);
    vertPtr(0)->edge = edgePtr(0);
    for (int i = 1; i < 4; i++){
        vertices.push_back(mkU<Vertex>());
        halfEdges.push_back(mkU<HalfEdge>());
        edgePtr(i - 1)->next = edgePtr(i);
        edgePtr(i)->face = facePtr(0);
        edgePtr(i)->vert = vertPtr(i);
        vertPtr(i)->edge = edgePtr(i);
    }
    edgePtr(3)->next = edgePtr(0);
    vertPtr(0)->pos = glm::vec3(0.5, 0.5, 0.5);
    vertPtr(1)->pos = glm::vec3(-0.5, 0.5, 0.5);
    vertPtr(2)->pos = glm::vec3(-0.5, 0.5, -0.5);
    vertPtr(3)->pos = glm::vec3(0.5, 0.5, -0.5);


    faces.push_back(mkU<Face>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(4)->face = facePtr(1);
    facePtr(1)->edge = edgePtr(4);
    edgePtr(0)->sym = edgePtr(4);
    edgePtr(4)->sym = edgePtr(0);
    edgePtr(4)->vert = vertPtr(3);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(4)->next = edgePtr(5);
    edgePtr(5)->face = facePtr(1);
    vertices.push_back(mkU<Vertex>());
    edgePtr(5)->vert = vertPtr(4);
    vertPtr(4)->edge = edgePtr(5);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(5)->next = edgePtr(6);
    edgePtr(6)->face = facePtr(1);
    vertices.push_back(mkU<Vertex>());
    edgePtr(6)->vert = vertPtr(5);
    vertPtr(5)->edge = edgePtr(6);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(6)->next = edgePtr(7);
    edgePtr(7)->face = facePtr(1);
    edgePtr(7)->vert = vertPtr(0);
    edgePtr(7)->next = edgePtr(4);
    vertPtr(4)->pos = glm::vec3(0.5, -0.5, -0.5);
    vertPtr(5)->pos = glm::vec3(0.5, -0.5, 0.5);


    faces.push_back(mkU<Face>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(8)->face = facePtr(2);
    facePtr(2)->edge = edgePtr(8);
    edgePtr(8)->vert = vertPtr(5);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(8)->next = edgePtr(9);
    edgePtr(9)->face = facePtr(2);
    vertices.push_back(mkU<Vertex>());
    edgePtr(9)->vert = vertPtr(6);
    vertPtr(6)->edge = edgePtr(9);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(9)->next = edgePtr(10);
    edgePtr(10)->face = facePtr(2);
    edgePtr(10)->vert = vertPtr(1);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(10)->next = edgePtr(11);
    edgePtr(11)->face = facePtr(2);
    edgePtr(11)->vert = vertPtr(0);
    edgePtr(11)->next = edgePtr(8);
    vertPtr(6)->pos = glm::vec3(-0.5, -0.5, 0.5);


    faces.push_back(mkU<Face>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(12)->face = facePtr(3);
    facePtr(3)->edge = edgePtr(12);
    edgePtr(12)->vert = vertPtr(6);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(12)->next = edgePtr(13);
    edgePtr(13)->face = facePtr(3);
    vertices.push_back(mkU<Vertex>());
    edgePtr(13)->vert = vertPtr(7);
    vertPtr(7)->edge = edgePtr(13);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(13)->next = edgePtr(14);
    edgePtr(14)->face = facePtr(3);
    edgePtr(14)->vert = vertPtr(2);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(14)->next = edgePtr(15);
    edgePtr(15)->face = facePtr(3);
    edgePtr(15)->vert = vertPtr(1);
    edgePtr(15)->next = edgePtr(12);
    vertPtr(7)->pos = glm::vec3(-0.5, -0.5, -0.5);


    faces.push_back(mkU<Face>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(16)->face = facePtr(4);
    facePtr(4)->edge = edgePtr(16);
    edgePtr(16)->vert = vertPtr(7);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(16)->next = edgePtr(17);
    edgePtr(17)->face = facePtr(4);
    edgePtr(17)->vert = vertPtr(4);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(17)->next = edgePtr(18);
    edgePtr(18)->face = facePtr(4);
    edgePtr(18)->vert = vertPtr(3);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(18)->next = edgePtr(19);
    edgePtr(19)->face = facePtr(4);
    edgePtr(19)->vert = vertPtr(2);
    edgePtr(19)->next = edgePtr(16);


    faces.push_back(mkU<Face>());
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(20)->face = facePtr(5);
    facePtr(5)->edge = edgePtr(20);
    edgePtr(20)->vert = vertPtr(7);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(20)->next = edgePtr(21);
    edgePtr(21)->face = facePtr(5);
    edgePtr(21)->vert = vertPtr(6);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(21)->next = edgePtr(22);
    edgePtr(22)->face = facePtr(5);
    edgePtr(22)->vert = vertPtr(5);
    halfEdges.push_back(mkU<HalfEdge>());
    edgePtr(22)->next = edgePtr(23);
    edgePtr(23)->face = facePtr(5);
    edgePtr(23)->vert = vertPtr(4);
    edgePtr(23)->next = edgePtr(20);


    edgePtr(3)->sym = edgePtr(19);
    edgePtr(19)->sym = edgePtr(3);

    edgePtr(14)->sym = edgePtr(16);
    edgePtr(16)->sym = edgePtr(14);

    edgePtr(18)->sym = edgePtr(5);
    edgePtr(5)->sym = edgePtr(18);

    edgePtr(6)->sym = edgePtr(23);
    edgePtr(23)->sym = edgePtr(6);

    edgePtr(20)->sym = edgePtr(17);
    edgePtr(17)->sym = edgePtr(20);

    edgePtr(22)->sym = edgePtr(9);
    edgePtr(9)->sym = edgePtr(22);

    edgePtr(21)->sym = edgePtr(13);
    edgePtr(13)->sym = edgePtr(21);

    edgePtr(12)->sym = edgePtr(10);
    edgePtr(10)->sym = edgePtr(12);

    edgePtr(2)->sym = edgePtr(15);
    edgePtr(15)->sym = edgePtr(2);

    edgePtr(7)->sym = edgePtr(8);
    edgePtr(8)->sym = edgePtr(7);

    edgePtr(1)->sym = edgePtr(11);
    edgePtr(11)->sym = edgePtr(1);


    facePtr(0)->colour = glm::vec3(1.0, 0.0, 0.0);
    facePtr(1)->colour = glm::vec3(0.0, 0.0, 1.0);
    facePtr(2)->colour = glm::vec3(0.0, 1.0, 1.0);
    facePtr(3)->colour = glm::vec3(1.0, 1.0, 0.0);
    facePtr(4)->colour = glm::vec3(1.0, 1.0, 1.0);
    facePtr(5)->colour = glm::vec3(0.0, 0.0, 0.0);
}

// takes in the position data from every vertex line in the obj file and
// returns a vec3 containing the coordinates for that specific vertex
glm::vec3 extractVert(std::string vertData)
{
    std::string currSeg;
    std::vector<std::string> coords;
    glm::vec3 pos;
    for (int i = 2; i < vertData.length(); i++){
        if (vertData[i] == ' '){
            coords.push_back(currSeg);
            currSeg.clear();
        } else {
            currSeg += vertData[i];
        }
    }
    coords.push_back(currSeg);
    pos[0] = std::stof(coords[0]);
    pos[1] = std::stof(coords[1]);
    pos[2] = std::stof(coords[2]);
    return pos;
}

// Takes in the face lines from the obj and exctracts the vertex order
std::vector<int> extractFace(std::string faceData){
    std::vector<int> verts;
    std::string lastNum;
    bool vert = true;
    for (int i = 2; i < faceData.length(); i++){
        if (faceData[i] == ' '){
            vert = true;
            verts.insert(verts.begin(), std::stoi(lastNum));
            lastNum.clear();
        } else if (faceData[i] != '/' && vert){
            lastNum += faceData[i];
        } else if (faceData[i] == '/'){
            vert = false;
        }
    }
    verts.insert(verts.begin(), std::stoi(lastNum));
    return verts;
}

// Creates halfEdges and faces based on the vertices given in the obj
// also creates a dummy sym halfEdge for every halfEdge to later
// set the sym values
void Mesh::constructFace(std::vector<int> verts)
{
    faces.push_back(mkU<Face>());
    Face *f = facePtr(faces.size() - 1);
    HalfEdge *finalNext;
    for (int i = 0; i < verts.size(); i++){
        halfEdges.push_back(mkU<HalfEdge>());
        HalfEdge *bck = edgePtr(halfEdges.size() - 1);
        halfEdges.push_back(mkU<HalfEdge>());
        HalfEdge *fwd = edgePtr(halfEdges.size() - 1);
        fwd->face = f;
        fwd->sym = bck;
        bck->sym = fwd;
        fwd->vert = vertPtr(verts[(i + 1) % verts.size()] - 1);
        vertPtr(verts[(i + 1) % verts.size()] - 1)->edge = fwd;
        bck->vert = vertPtr(verts[i] - 1);
        if (i > 0){
            edgePtr(halfEdges.size() - 3)->next = fwd;
        } else {
            f->edge = fwd;
            finalNext = fwd;
        }
    }
    edgePtr(halfEdges.size() - 1)->next = finalNext;
    f->colour = glm::fract(glm::vec3(f->iD * 97.12918 * glm::sin(float(std::clock())),
                                     vertices.size() * 181.9123 * std::clock(),
                                     9812.129898321 * glm::cos(float(std::clock()))));
}


// After we're done adding faces and halfEdges, this sets the edges' syms
// and gets rid of the dummy edges that contain the sym value
void Mesh::mergeExtraneous()
{
    std::map<std::array<int, 2>, int> edges;
    std::vector<uPtr<HalfEdge>> remainingEdges;
    for (int i = 0; i < halfEdges.size(); i++){
        HalfEdge *e = edgePtr(i);
        std::array<int, 2> verts = {int(e->vert->iD), int(e->sym->vert->iD)};
        auto find = edges.find(verts);
        if (find != edges.end()){
            HalfEdge *eFound = edgePtr(find->second);
            if (eFound->next == nullptr){
                e->sym = eFound->sym;
                remainingEdges.push_back(std::move(halfEdges[e->iD]));
            } else {
                eFound->sym = e->sym;
                remainingEdges.push_back(std::move(halfEdges[eFound->iD]));
            }
        } else {
            edges.insert(std::make_pair(verts, e->iD));
        }
    }
    halfEdges.clear();
    for (int i = 0; i < remainingEdges.size(); i++){
        halfEdges.push_back(std::move(remainingEdges[i]));
    }
}


// Iterates through the obj files while calling other functions to
// create vertices and faces accordingly
void Mesh::createFromOBJ(std::string fileName)
{
    resetMesh();
    std::ifstream fileStream;
    fileStream.open(fileName);
    if (!fileStream) {
        std::cerr << "Unable to open file " << fileName << "\n";
        exit(1);   // call system to stop
    }
    std::string line;
    while (std::getline(fileStream, line)){
        if (line[0] == 'v' && line[1] == ' '){
            glm::vec3 vertPos = extractVert(line);
            vertices.push_back(mkU<Vertex>());
            vertPtr(vertices.size() - 1)->pos = vertPos;
        } else if (line[0] == 'f'){
            std::vector<int> faceData = extractFace(line);
            constructFace(faceData);
        }
    }
    mergeExtraneous();
}

// resets the face, halfEdge, and vertex vectors to create a new
// mesh
void Mesh::resetMesh()
{
    vertices.clear();
    halfEdges.clear();
    faces.clear();
    Vertex::resetCounter();
    HalfEdge::resetCounter();
    Face::resetCounter();
}

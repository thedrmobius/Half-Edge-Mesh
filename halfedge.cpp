#include "halfedge.h"

int HalfEdge::counter = 0;

HalfEdge::HalfEdge()
    :next(nullptr), sym(nullptr), face(nullptr), vert(nullptr), iD(counter)
{
    // Setting the name that would appear in the widget
    QListWidgetItem::setText(QString::number(iD));

    // The counter is always incremented after declaration in
    // order to ensure no two faces share the same iD
    counter++;
}

HalfEdge::HalfEdge(HalfEdge &iNext, HalfEdge &iSym, Face &iFace, Vertex &iVert)
    :next(&iNext), sym(&iSym), face(&iFace), vert(&iVert), iD(counter)
{
    QListWidgetItem::setText(QString::number(iD));
    counter++;
}

void HalfEdge::resetCounter()
{
    counter = 0;
}


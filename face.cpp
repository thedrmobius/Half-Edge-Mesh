#include "face.h"

int Face::counter = 0;

Face::Face()
    :colour(255, 255, 255), edge(nullptr), iD(counter)
{
    // Setting the name that would appear in the widget
    QListWidgetItem::setText(QString::number(iD));

    // The counter is always incremented after declaration in
    // order to ensure no two faces share the same iD
    counter++;
}

Face::Face(glm::vec3 iColour, HalfEdge &iEdge)
    :colour(iColour), edge(&iEdge), iD(counter)
{
    QListWidgetItem::setText(QString::number(iD));
    counter++;
}

void Face::resetCounter()
{
    counter = 0;
}


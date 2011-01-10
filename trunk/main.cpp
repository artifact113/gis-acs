#include <QtGui>
#include <QtDebug>
#include "graph.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel label("GIS Project");

	GIS::Graph *graph = new GIS::Graph;
	GIS::Vertex *va = graph->createVertex("A");
	GIS::Vertex *vb = graph->createVertex("B");
	va->connectTo(vb, 5);
	qDebug() << vb->connectedVertices().size();
	label.showMaximized();
	return app.exec();
}

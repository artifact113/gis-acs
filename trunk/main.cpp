#include <QtGui>
#include <QtDebug>
#include "graph.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel label("GIS Project");

	GIS::Graph *graph = new GIS::Graph;
//	GIS::Vertex *va = graph->createVertex("A");
//	GIS::Vertex *vb = graph->createVertex("B");
//	va->connectTo(vb, 5);
	qDebug() << "graph->readFromFile() returned:" << graph->readFromFile("test1.xml");
	qDebug() << "graph->vertices().size() =" << graph->vertices().size();
	label.showMaximized();
	return app.exec();
}

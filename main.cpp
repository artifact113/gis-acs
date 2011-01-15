#include <QtGui>
#include <QtDebug>
#include "graph.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel label("GIS Project");

	GIS::Graph *graph = new GIS::Graph;
    qDebug() << "graph->readFromFile() returned:" << graph->readFromFile("c://test_short_paths.xml");
	qDebug() << "graph->vertices().size() =" << graph->vertices().size();
	label.showMaximized();

        //graph->turnToCompleteGraph();
        graph->findShortestPaths();

        graph->printGraph();

        qDebug() << graph->vertex("A")->edgeTo(graph->vertex("E"))->weight();
        QList<GIS::Vertex*> test = graph->getPath(graph->vertex("A"), graph->vertex("E"));

        qDebug() << "rozmiar �cie�ki: " << test.size();

        foreach(GIS::Vertex* v, test)
        {
            qDebug() << v->label();
        }


	return app.exec();
}

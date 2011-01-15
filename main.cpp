#include <QtGui>
#include <QtDebug>
#include "graph.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel label("GIS Project");

	QString filename;
	GIS::Graph *graph = new GIS::Graph;
	if (app.arguments().contains("-f")) {
		int index = app.arguments().indexOf("-f");
		if (index == -1) {
			qDebug("'-f' but no '-f'...");
			return 1;
		}
		if (app.arguments().size() <= ++index) {
			qWarning("Wrong arguments list!");
			return 1;
		}
		filename = app.arguments().at(index);
	}
	qDebug() << "graph->readFromFile() returned:" << graph->readFromFile(filename);
	qDebug() << "graph->vertices().size() =" << graph->vertices().size();
	label.showMaximized();

	//graph->turnToCompleteGraph();
	graph->findShortestPaths();

	graph->printGraph();

	qDebug() << graph->vertex("A")->edgeTo(graph->vertex("E"))->weight();
	QList<GIS::Vertex*> test = graph->getPath(graph->vertex("A"), graph->vertex("E"));

	qDebug() << "rozmiar sciezki: " << test.size();

	foreach(GIS::Vertex* v, test)
	{
		qDebug() << v->label();
	}


	return app.exec();
}

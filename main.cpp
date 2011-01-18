#include <QtGui>
#include <QtDebug>
#include "graph.h"
#include "graphmodel.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow w;

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

	if (!filename.isEmpty()) {
		w.open(filename);
	}

//	qDebug() << "graph->readFromFile() returned:" << graph->readFromFile(filename);
//	qDebug() << "graph->vertices().size() =" << graph->vertices().size();

//	//graph->turnToCompleteGraph();
//	graph->findShortestPaths();

//	graph->printGraph();

//	qDebug() << graph->vertex("A")->edgeTo(graph->vertex("E"))->weight();
//	GIS::Path *test = graph->getPath(graph->vertex("A"), graph->vertex("E"));

//	qDebug() << "rozmiar sciezki: " << test->vertices().size();

//	foreach(GIS::Vertex* v, test->vertices())
//	{
//		qDebug() << v->label();
//	}

//	GIS::Path *shortestPath = graph->tpsPath();
//	if (shortestPath) {
//		qDebug() << "shortes path total cost =" << shortestPath->totalCost();
//		qDebug() << "path contains:";
//		foreach (GIS::Vertex *v, shortestPath->vertices()) {
//			qDebug() << v->label();
//		}
//	}

	w.showMaximized();

	return app.exec();
}

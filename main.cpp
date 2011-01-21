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
	w.showMaximized();

	return app.exec();
}

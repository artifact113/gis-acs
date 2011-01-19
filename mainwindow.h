#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "graphmodel.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
    ~MainWindow();
	void open(const QString &filename);
private slots:
	void open();
	void runAcs();
	void runBruteForce();
	void turnTuComplete();
	void generateGraph();
private:
    Ui::MainWindow *ui;
	GIS::Graph *m_graph;
	GIS::GraphModel *m_model;
};

#endif // MAINWINDOW_H

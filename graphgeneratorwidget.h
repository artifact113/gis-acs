#ifndef GRAPHGENERATORWIDGET_H
#define GRAPHGENERATORWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include "graph.h"

class GraphGeneratorWidget : public QWidget
{
    Q_OBJECT
public:
	GraphGeneratorWidget(QWidget *parent = 0);
public slots:
	void generateGraph();
	void saveGraph();
private:
	QString generateName();
	void resetNameGenerator();
private:
	QSpinBox *m_vertSpinBox;
	QSpinBox *m_lvlSpinBox;
	QLabel *m_statusLabel;
	QPushButton *m_generateButton;
	QPushButton *m_saveButton;
	GIS::Graph *m_graph;
	char *m_currentName;
};

#endif // GRAPHGENERATORWIDGET_H

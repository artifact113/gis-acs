#include "graphgeneratorwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

GraphGeneratorWidget::GraphGeneratorWidget(QWidget *parent)
	: QWidget(parent)
	, m_graph(0)
	, m_currentName(0)
{
	m_vertSpinBox = new QSpinBox(this);
	m_vertSpinBox->setMinimum(2);
	m_vertSpinBox->setMaximum(100000);
	m_vertSpinBox->setSingleStep(1);
	m_lvlSpinBox = new QSpinBox(this);
	m_lvlSpinBox->setMinimum(1);
	m_lvlSpinBox->setMaximum(100);
	m_lvlSpinBox->setSingleStep(1);
	m_saveButton = new QPushButton(tr("Save graph"), this);
	m_saveButton->setEnabled(false);
	m_generateButton = new QPushButton(tr("Generate graph"), this);
	m_statusLabel = new QLabel("...", this);
	QVBoxLayout *vlo = new QVBoxLayout(this);
	QHBoxLayout *lo = new QHBoxLayout;
	lo->addWidget(new QLabel(tr("Vertices:")));
	lo->addWidget(m_vertSpinBox);
	vlo->addLayout(lo);
	lo = new QHBoxLayout;
	lo->addWidget(new QLabel(tr("Level:")));
	lo->addWidget(m_lvlSpinBox);
	vlo->addLayout(lo);
	vlo->addWidget(m_statusLabel);
	vlo->addWidget(m_generateButton);
	vlo->addWidget(m_saveButton);
	setLayout(vlo);

	connect(m_generateButton, SIGNAL(clicked()), SLOT(generateGraph()));
	connect(m_saveButton, SIGNAL(clicked()), SLOT(saveGraph()));
}

void GraphGeneratorWidget::generateGraph()
{
	if (m_graph) {
		delete m_graph;
	}
	m_graph = new GIS::Graph;
	resetNameGenerator();

	int lvl = m_lvlSpinBox->value();
	int verts = m_vertSpinBox->value();
	if (lvl >= verts) {
		QMessageBox::warning(this, tr("Error"), tr("You must set higher number of vertices than level"));
		return;
	}
	for (int i = 0; i < lvl; ++i) {
		m_graph->createVertex(generateName());
	}
	verts -= lvl;
	while (verts--) {
		QList<GIS::Vertex *> vertices = m_graph->vertices();
		GIS::Vertex *v = m_graph->createVertex(generateName());
		for (int i = 0; i < lvl; ++i) {
			int index = qrand() % vertices.size();
            vertices[index]->connectTo(v, qrand() % 100 + 1);
			vertices.removeAt(index);
		}
	}
	m_saveButton->setEnabled(true);
}

void GraphGeneratorWidget::saveGraph()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save graph..."), QApplication::applicationDirPath(), "*.xml");
	if (filename.isEmpty()) {
		return;
	}
	m_graph->saveToFile(filename);
}

QString GraphGeneratorWidget::generateName()
{
		QString result(m_currentName);
		int len = strlen(m_currentName);
		char *tmp = new char[len+2];
		strcpy(tmp, m_currentName);
		if (*(tmp+len-1) < 'Z') {
				++*(tmp+len-1);
		} else {
				int i;
				for (i = len-1; i >= 0; --i) {
						if (*(tmp+i) < 'Z') {
								++*(tmp+i);
								break;
						}
				}
				int z = ++i ? 0 : 1;
				for (i; i < len+z; ++i) {
						*(tmp+i) = 'A';
				}
				*(tmp+len+z) = '\0';
		}
		delete[] m_currentName;
		m_currentName = new char[len+2];
		strcpy(m_currentName, tmp);
		delete[] tmp;
		return result;
}

void GraphGeneratorWidget::resetNameGenerator()
{
		if (m_currentName)
				delete m_currentName;
		m_currentName = new char[2];
		strcpy(m_currentName, "A");
}

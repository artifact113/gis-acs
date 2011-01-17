#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_graph(0)
{
    ui->setupUi(this);
	ui->bfSplitter->setStretchFactor(0, 1);
	ui->bfSplitter->setStretchFactor(1, 2);
	ui->acsSplitter->setStretchFactor(0, 1);
	ui->acsSplitter->setStretchFactor(1, 2);

	m_model = new GIS::GraphModel(this);
	ui->graphView->setModel(m_model);

	connect(ui->toCompleteButton, SIGNAL(clicked()), SLOT(turnTuComplete()));
	connect(ui->actionOpen_file, SIGNAL(triggered()), SLOT(open()));
	connect(ui->acsRunButton, SIGNAL(clicked()), SLOT(runAcs()));
	connect(ui->bfRunButton, SIGNAL(clicked()), SLOT(runBruteForce()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
	QString filename = QFileDialog::getOpenFileName(this,
													tr("Open file..."),
													QApplication::applicationDirPath(),
													"*.xml");
	if (filename.isEmpty())
		return;
	open(filename);
}

void MainWindow::open(const QString &filename)
{
	if (m_graph) {
		m_model->setGraph(0);
		delete m_graph;
		m_graph = 0;
	}
	m_graph = new GIS::Graph;
	if (!m_graph->readFromFile(filename)) {
		QMessageBox::critical(this, tr("Error"),
							  tr("Cannot open file: %1").arg(filename));
		return;
	}
	m_model->setGraph(m_graph);
}

void MainWindow::turnTuComplete()
{
	if (!m_graph) {
		QMessageBox::warning(this, tr("Error"), tr("Load a graph first!"));
		return;
	}
	if (!m_graph->isConnected()) {
		QMessageBox::critical(this, tr("Error"), tr("Graph is not connected!"));
		return;
	}
	m_model->setGraph(0);
	m_graph->findShortestPaths();
	m_model->setGraph(0);
}

void MainWindow::runAcs()
{
}

void MainWindow::runBruteForce()
{
}

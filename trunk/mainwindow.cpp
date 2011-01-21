#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "singletons.h"
#include "graphgeneratorwidget.h"

#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>
#include <QTime>
#include <QStandardItemModel>

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
	connect(ui->actionGenerate_graph, SIGNAL(triggered()), SLOT(generateGraph()));

	ui->bfConsole->setStyleSheet("font-family : \"Consolas\", \"monospace\", \"Courier New\"");
	ui->acsConsole->setStyleSheet("font-family : \"Consolas\", \"monospace\", \"Courier New\"");

	BFLogger::instance().setPlainTextEdit(ui->bfConsole);
	ACSLogger::instance().setPlainTextEdit(ui->acsConsole);
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
	m_model->setGraph(m_graph);
}

void MainWindow::runAcs()
{
	if (!m_graph) {
		QMessageBox::warning(this, tr("Error"), tr("Load a graph first!"));
		return;
	}
	if (!m_graph->isConnected()) {
		QMessageBox::critical(this, tr("Error"), tr("Graph is not connected!"));
		return;
	}
	QTime time;
	time.start();
	GIS::Path *shortestPath = m_graph->tspPath(GIS::Graph::ACS);
	int msec = time.elapsed();
	ACSLogger::instance().log(tr("Time elapsed: ") + QString::number((double)msec/1000.0, 'f', 3) + "s");
	ACSLogger::instance().log("-------------------------------");
	QStandardItemModel *model = 0;
	if (ui->acsResultView->model()) {
		model = static_cast<QStandardItemModel *>(ui->acsResultView->model());
		model->clear();
	} else {
		model = new QStandardItemModel(ui->acsResultView);
		ui->acsResultView->setModel(model);
	}
	foreach (GIS::Vertex *v, shortestPath->vertices()) {
		model->appendRow(new QStandardItem(v->label()));
	}
	ui->acsTimeLabel->setText(QString::number((double)msec/1000.0, 'f', 3) + "s");
	ui->acsTotalLabel->setText(QString::number(shortestPath->totalCost()));
}

void MainWindow::runBruteForce()
{
	if (!m_graph) {
		QMessageBox::warning(this, tr("Error"), tr("Load a graph first!"));
		return;
	}
	if (!m_graph->isConnected()) {
		QMessageBox::critical(this, tr("Error"), tr("Graph is not connected!"));
		return;
	}
	QTime time;
	time.start();
	GIS::Path *shortestPath = m_graph->tspPath(GIS::Graph::BruteForce);
	int msec = time.elapsed();
	BFLogger::instance().log(tr("Time elapsed: ") + QString::number((double)msec/1000.0, 'f', 3) + "s");
	BFLogger::instance().log("-------------------------------");
	QStandardItemModel *model = 0;
	if (ui->bfResultView->model()) {
		model = static_cast<QStandardItemModel *>(ui->bfResultView->model());
		model->clear();
	} else {
		model = new QStandardItemModel(ui->bfResultView);
		ui->bfResultView->setModel(model);
	}
	foreach (GIS::Vertex *v, shortestPath->vertices()) {
		model->appendRow(new QStandardItem(v->label()));
	}
	ui->bfTimeLabel->setText(QString::number((double)msec/1000.0, 'f', 3) + "s");
	ui->bfTotalLabel->setText(QString::number(shortestPath->totalCost()));
}

void MainWindow::generateGraph()
{
	QDialog dialog(this);
	dialog.setLayout(new QVBoxLayout);
	dialog.layout()->addWidget(new GraphGeneratorWidget(&dialog));
	dialog.exec();
}

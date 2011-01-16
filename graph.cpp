#include "graph.h"
#include <QFile>
#include <QDomDocument>
#include <QtDebug>


namespace GIS {

class ACS
{


public:
    ACS(Graph* g);

    Path acs();

private:
    void init();
    void iteration();
    void localUpdate();
    void globalUpdate();
    Path shortestPath();

    QList<Ant* > ants;
    Graph* graph;
    static const int N = 100;
};

class Ant
{
public:
    void step();
    void localUpdate();
    Ant(QList<Vertex*> &cities);

private:
    Vertex* homeCity;
    QList<Vertex* > remainingCities;
    QList<Vertex* > tour;
};

struct ACSData {

    QHash<Edge*, double> feromones;

};

struct BruteForceData {
	BruteForceData() {}
};

static const int infinity = 10000000;

/*!
\class Graph
*/
Graph::Graph()
	: m_acsData(0)
	, m_bfData(0)
{
}

// Implementation of Floyd-Warshall algorithm
void Graph::findShortestPaths()
{
	QList<QString> vertices_labels= m_vertices.keys();

	// init PI
	foreach(QString label_i, vertices_labels)
	{
		foreach(QString label_j, vertices_labels)
		{
			if(label_i != label_j && d(label_i, label_j) != NULL/*&& d(label_i, label_j)->weight()*/)
			{
				qDebug() << label_i << " " << label_j;
				m_vertices[label_j]->setPrevious(label_i, m_vertices[label_i]);
			}
		}
	}

    // find shortest paths and fill PI
    foreach(QString label_k, vertices_labels)
    {     
		foreach(QString label_i, vertices_labels)
		{
			if(label_i != label_k)
			{
				foreach(QString label_j, vertices_labels)
				{
					if(label_i != label_j && label_j != label_k)
					{
						Edge* d_ij = d(label_i, label_j);
						Edge* d_ik = d(label_i, label_k);
						Edge* d_kj = d(label_k, label_j);

						int d_ij_weight = d_ij == NULL ? infinity : d_ij->weight();
						int d_ik_weight = d_ik == NULL ? infinity : d_ik->weight();
						int d_kj_weight = d_kj == NULL ? infinity : d_kj->weight();

						if(d_ij_weight <= d_ik_weight + d_kj_weight && d_ij_weight != infinity)
						{
                            d(label_i, label_j)->setWeight(d_ij_weight);
						}
						else if(d_ij_weight > d_ik_weight + d_kj_weight)
						{
							if(d_ij_weight != infinity)
                            {
								d(label_i, label_j)->setWeight(d_ik_weight + d_kj_weight);
                                d(label_i, label_j)->turnToVirtual();
                                m_vertices[label_j]->setPrevious(label_i, m_vertices[label_j]->previous(label_k));
                                m_vertices[label_i]->setPrevious(label_j, m_vertices[label_i]->previous(label_k));
                            }
							else
                            {
                                //qDebug() << "Virtual: " << label_i << "-> " << label_j;
								vertex(label_i)->virtuallyConnectTo(vertex(label_j), d_ik_weight + d_kj_weight);

                                m_vertices[label_j]->setPrevious(label_i, m_vertices[label_j]->previous(label_k));
                                m_vertices[label_i]->setPrevious(label_j, m_vertices[label_i]->previous(label_k));
                            }

                            //m_vertices[label_j]->setPrevious(label_i, m_vertices[label_j]->previous(label_k));
						}
					}
				}
			}
		}
	}
}

void Graph::printGraph()
{
    foreach(Vertex* from, m_vertices)
    {
		qDebug() << "Wierzcholek: " << from->label();

        foreach(Vertex* to, from->connectedVertices())
        {
            QString path_str;
			Path *path = getPath(from, to);
			foreach(Vertex* path_elem, path->vertices())
            {
                path_str.append(" ").append(path_elem->label());
            }

            qDebug() << from->label()<< " " << to->label() << " : " << from->edgeTo(to)->weight() << " " << from->edgeTo(to)->isVirtual() << "\t" << path_str;
        }
    }
}

Path *Graph::getPath(Vertex* from, Vertex* to)
{
	if(from->label() == to->label())
	{
		Path *result_path = new Path(this);
		result_path->appendVertex(from);
		return result_path;
	}

	if(to->previous(from->label()) != NULL)
	{
		Path *result_path = getPath(from, to->previous(from->label()));
		result_path->appendVertex(to);
		return result_path;
	}

	return new Path(this);
}

Edge* Graph::d(QString label_i, QString label_j)
{
	return m_vertices.value(label_i)->edgeTo(m_vertices.value(label_j));
}

Vertex *Graph::createVertex(const QString &label)
{
	if (m_vertices.contains(label)) {
		return 0;
	}

	Vertex *vertex = new Vertex(this, label);
	m_vertices.insert(label, vertex);

	return vertex;
}

Vertex *Graph::vertex(const QString &label) const
{
	return m_vertices.value(label, 0);
}

bool Graph::isConnected() const
{
	if (m_vertices.isEmpty()) {
		return false;
	}
	m_visited.clear();
	dfsTraverseFrom(m_vertices.values().first());
	if (m_vertices.values().size() == m_visited.size()) {
		return true;
	}
	return false;
}

QList<Vertex *> Graph::vertices() const
{
	return m_vertices.values();
}

void Graph::dfsTraverseFrom(Vertex *v) const
{
	if (m_visited.contains(v))
		return;
	m_visited.append(v);
	QList<Vertex *> vertices = v->connectedVertices();
	for (int i = 0; i < vertices.size(); ++i) {
		dfsTraverseFrom(vertices.at(i));
	}
}

bool Graph::readFromFile(const QString &filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qDebug("Cannot open file!");
		return false;
	}

	QDomDocument doc;
	if (!doc.setContent(&file)) {
		qDebug("File content is not a well-formed XML");
		return false;
	}

	// clear vertices
	// TODO delete items
	m_vertices.clear();
	m_visited.clear();

	// XML parsing
	QDomElement rootElem = doc.documentElement();
	for (QDomElement edgeElem = rootElem.firstChildElement("edge"); !edgeElem.isNull(); edgeElem = edgeElem.nextSiblingElement("edge")) {
		QDomElement vertex1Elem = edgeElem.firstChildElement("vertex");
		if (vertex1Elem.isNull()) {
			return false;
		}
		QDomElement vertex2Elem = vertex1Elem.nextSiblingElement("vertex");
		if (vertex2Elem.isNull()) {
			return false;
		}
		QDomElement weightElem = edgeElem.firstChildElement("weight");
		if (weightElem.isNull()) {
			return false;
		}

		Vertex *v1 = vertex(vertex1Elem.text());
		if (!v1) {
			v1 = createVertex(vertex1Elem.text());
		}

		Vertex *v2 = vertex(vertex2Elem.text());
		if (!v2) {
			v2 = createVertex(vertex2Elem.text());
		}
		bool ok = false;
		int weight = weightElem.text().toInt(&ok);
		if (!ok) {
			return false;
		}
		v1->connectTo(v2, weight);
	}
	return true;
}

bool Graph::saveToFile(const QString &filename) const
{
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		qWarning("Cannot open file!");
		return false;
	}
	QSet<Edge *> set;
	QList<Vertex *> verts = vertices();
	foreach (Vertex *v, verts) {
		QList<Edge *> edges = v->edges();
		foreach (Edge *e, edges) {
			set.insert(e);
		}
	}

	QDomDocument doc;
	doc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>"
						   "<graph></graph>"));
	QDomElement graphElem = doc.documentElement();

	return false;
}

Path *Graph::tpsPath(TpsType type) const
{
	if (!m_vertices.size()) {
		qDebug() << Q_FUNC_INFO << "Graph is empty!";
		return 0;
	}

	switch (type) {
	case BruteForce:
		return const_cast<Graph *>(this)->tpsPath_BruteForce();
	case ACS:
		return const_cast<Graph *>(this)->tpsPath_ACS();
	}

	return 0;
}

Path *Graph::tpsPath_ACS()
{
	return 0;
}

Path *Graph::tpsPath_BruteForce()
{
	return 0;
}

/*!
\class Vertex
*/
Vertex::Vertex(Graph *parentGraph, const QString &label)
	: m_graph(parentGraph)
	, m_label(label)
{
}

Graph *Vertex::graph() const
{
	return m_graph;
}

QString Vertex::label() const
{
	return m_label;
}

QList<Vertex *> Vertex::connectedVertices() const
{
	return m_connectedVertices.keys();
}

Edge *Vertex::edgeTo(Vertex *v) const
{
	return m_connectedVertices.value(v, 0);
}

Edge *Vertex::connectTo(Vertex *v, int weight)
{
	if (v->m_graph != m_graph) {
		return 0;
	}
	if (m_connectedVertices.contains(v)) {
		return 0;
	}
	if (weight < 0) {
		return 0;
	}
	Edge *edge = new Edge(m_graph, this, v, weight);
	m_connectedVertices.insert(v, edge);
	v->m_connectedVertices.insert(this, edge);
	return edge;
}

Edge* Vertex::virtuallyConnectTo(Vertex *v, int weight)
{
	Edge* e = connectTo(v, weight);
	e->turnToVirtual();
	return e;
}

Vertex* Vertex::previous(const QString &from)
{
	return m_previous[from];
}

void Vertex::setPrevious(const QString &from, Vertex* previous)
{
	m_previous[from] = previous;
}

QList<Edge *> Vertex::edges() const
{
	return m_connectedVertices.values();
}

/*!
\class Edge
*/
Edge::Edge(Graph *parentGraph, Vertex *s, Vertex *e, int weight, bool virtual_)
	: m_graph(parentGraph)
	, m_start(s)
	, m_end(e)
	, m_weight(weight)
	, m_virtual(virtual_)
{
}

Vertex *Edge::startPoint() const
{
	return m_start;
}

Vertex *Edge::endPoint() const
{
	return m_end;
}

int Edge::weight() const
{
	return m_weight;
}

bool Edge::isVirtual() const
{
	return m_virtual;
}

void Edge::setWeight(int weight)
{
	m_weight = weight;
}

void Edge::turnToVirtual()
{
	m_virtual= true;
}

/*!
\class Path
*/
Path::Path(Graph *parentGraph)
	: m_graph(parentGraph)
	, m_total(0)
{
}

bool Path::appendVertex(Vertex *v)
{
	if (!m_vertices.size()) {
		m_vertices.append(v);
		return true;
	}
	Vertex *prev = m_vertices.last();
	if (prev == v) {
		qDebug("Cannot add same vertex in path just after itself");
		return false;
	}
	m_vertices.append(v);
	Edge *e = prev->edgeTo(v);
	if (!e) {
		qDebug("No such edge!");
		return false;
	}
	m_total += e->weight();
	return true;
}

bool Path::setVertices(const QList<Vertex *> &verts)
{
	m_total = 0;
	m_vertices.clear();
	foreach (Vertex *v, verts) {
		bool result = appendVertex(v);
		if (!result) {
			m_total = 0;
			m_vertices.clear();
			return false;
		}
	}
	return true;
}

QList<Vertex *> Path::vertices() const
{
	return m_vertices;
}

int Path::totalCost() const
{
	return m_total;
}

bool Path::isValid() const
{
	return !m_vertices.isEmpty();
}

} // namespace GIS

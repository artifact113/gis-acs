#include "graph.h"
#include <QFile>
#include <QDomDocument>
#include <QtDebug>


namespace GIS {

static const int infinity = 10000000;

/*!
\class Graph
*/
Graph::Graph()
{
}

//void Graph::turnToCompleteGraph()
//{
//    QList<QString> vertices_labels= m_vertices.keys();

//    // turn to graph complate
//    foreach (const QString &label_from, vertices_labels)
//    {
//        Vertex* v_from = m_vertices.value(label_from);

//        foreach (const QString &label_to, vertices_labels)
//        {
//            Vertex* v_to = m_vertices.value(label_to);

//            if(v_from->edgeTo(v_to) == NULL)
//            {
//                v_from->connectTo(v_to, infinity);
//            }
//        }
//    }
//}

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
								d(label_i, label_j)->setWeight(d_ik_weight + d_kj_weight);
							else
								vertex(label_i)->virtuallyConnectTo(vertex(label_j), d_ik_weight + d_kj_weight);

							m_vertices[label_j]->setPrevious(label_i, m_vertices[label_j]->previous(label_k));
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
		qDebug() << "Wierzchołek: " << from->label();

		foreach(Vertex* to, from->connectedVertices())
		{
			qDebug() << from->label()<< " " << to->label() << " : " << from->edgeTo(to)->weight() << " " << from->edgeTo(to)->isVirtual();
		}
	}
}

QList<Vertex*> Graph::getPath(Vertex* from, Vertex* to)
{
	QList<Vertex*> result_path;

	if(from->label() == to->label())
	{
		result_path.append(from);
	}
	else
	{
		if(to->previous(from->label()) == NULL)
		{
			qDebug() << "nie istnieje ścieżka z " << from->label() << " do " << to->label();
		}
		else
		{
			result_path = getPath(from, to->previous(from->label()));
			result_path.append(to);
		}
	}

	return result_path;
}

Edge* Graph::d(QString label_i, QString label_j)
{
	return m_vertices.value(label_i)->edgeTo(m_vertices.value(label_j));
}

void Graph::dijkstraVertex(Vertex *v) const
{
	/*    QList<Vertex*> remaining_vertexes = m_vertices;
QHash<Vertex*, QPair< qint32, QList<Edge*> > > paths;

// init to INT_MAX
foreach(Vertex* v, m_vertices)
{
paths.insert(v, QPair(INT_MAX, QList()));
}

// find shortest paths
while(!vertices.empty())
{
// pick the vertice from remaining group with shortest path;
Vertex* curr_vertex = remaining_vertexes.at(0);
int curr_vertex_index = 0;
for(int i = 0; i < remaining_vertexes.size(); ++i)
{
if(v->edgeTo(remaining_vertexes.at(i))->weight() < v->edgeTo(curr_vertex)->weight())
{
curr_vertex = remaining_vertexes.at(i);
}
}

// remove picked vertex from remaining list
remaining_vertexes.removeAt(curr_vertex_index);

//
}*/
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

} // namespace GIS

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

void Graph::turnToCompleteGraph()
{
    QList<QString> vertices_labels= m_vertices.keys();

    // turn to graph complate
    foreach (const QString &label_from, vertices_labels)
    {
        Vertex* v_from = m_vertices.value(label_from);

        foreach (const QString &label_to, vertices_labels)
        {
            Vertex* v_to = m_vertices.value(label_to);

            if(v_from->edgeTo(v_to) == NULL)
            {
                v_from->connectTo(v_to, infinity);
            }
        }
    }
}

// Implementation of Floyd-Warshall algorithm
void Graph::findShortestPaths()
{
    QList<QString> vertices_labels= m_vertices.keys();
    //QHash<QString, QHash<QString, QString> > PI;

    // init PI
    foreach(QString label_i, vertices_labels)
    {
        //PI.insert(label_i, QHash<QString, QString>());
        foreach(QString label_j, vertices_labels)
        {
            if(label_i != label_j && d(label_i, label_j)->weight() /*INT_MAX*/)
            {
                //PI.value(label_i)->insert(label_j, label_i);
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
            foreach(QString label_j, vertices_labels)
            {
                int d_ij_weight = d(label_i, label_j)->weight();
                int d_ik_weight = d(label_i, label_k)->weight();
                int d_kj_weight = d(label_k, label_j)->weight();

                if(d_ij_weight <= d_ik_weight + d_kj_weight)
                {
                    d(label_i, label_j)->setWeight(d_ij_weight);
                }
                else
                {
                    d(label_i, label_j)->setWeight(d_ik_weight + d_kj_weight);
                    //PI[label_i][label_j] = PI[label_k][label_j];
                    m_vertices[label_j]->setPrevious(label_i, m_vertices[label_j]->previous(label_k));
                }
            }
        }
    }
}

QList<Vertex*> Graph::getPath(Vertex* from, Vertex* to)
{
    QList<Vertex*> result_path;

    if(from->label() == to->label())
        result_path.append(from);
    else
    {
        if(to->previous(from->label()))
        {
            qDebug() << "nie istnieje œcie¿ka z " << from->label() << " do " << to->label();
            return result_path;
        }
        else
        {
            result_path = getPath(from, to->previous(from->label()));
            result_path.append(to);
            return result_path;
        }
    }
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
Vertex* Vertex::previous(QString from)
{
    return m_previous[from];
}

void Vertex::setPrevious(QString from, Vertex* previous)
{
    m_previous[from] = previous;
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

void Edge::setWeight(int weight)
{
    m_weight = weight;
}

void Edge::turnToVirtual()
{
    m_virtual= true;
}

} // namespace GIS

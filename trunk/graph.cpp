#include "graph.h"

namespace GIS {

/*!
  \class Graph
*/
Graph::Graph()
{
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
	// todo
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

} // namespace GIS

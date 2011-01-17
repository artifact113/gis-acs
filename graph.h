#ifndef GRAPH_H
#define GRAPH_H

#include <QString>
#include <QHash>

namespace GIS {

class Graph;
class Edge;
class ACSData;
class BruteForceData;
class ACS;

class Vertex
{
private:
	Vertex(Graph *parentGraph, const QString &label);
public:
	Graph *graph() const;
	QList<Vertex *> connectedVertices() const;
	QList<Edge *> edges() const;
	QString label() const;
	Edge *edgeTo(Vertex *v) const;
	Edge *connectTo(Vertex *v, int weight);
	Edge *virtuallyConnectTo(Vertex *v, int weight);
	void turnToVirtual();
	Vertex* previous(const QString &from);
	void setPrevious(const QString &from, Vertex* previous);

	friend class Graph;
private:
	QHash<QString, Vertex*> m_previous;
	Graph *m_graph;
	QString m_label;
	QHash<Vertex *, Edge *> m_connectedVertices;
};

class Edge
{
private:
	Edge(Graph *parentGraph, Vertex *s, Vertex *e, int weight, bool virtual_ = false);
public:
	Vertex *startPoint() const;
	Vertex *endPoint() const;
	int weight() const;
	void setWeight(int weight);
	bool isVirtual() const;
	void turnToVirtual();

	friend class Graph;
	friend class Vertex;
private:
	Graph *m_graph;
	Vertex *m_start;
	Vertex *m_end;
	int m_weight;
	bool m_virtual;
};

class Path
{
private:
	Path(Graph *parentGraph);
	bool appendVertex(Vertex *v);
	bool setVertices(const QList<Vertex *> &verts);
public:
	QList<Vertex *> vertices() const;
	int totalCost() const;
	bool isValid() const;

	friend class Graph;
private:
	Graph *m_graph;
	QList<Vertex *> m_vertices;
	int m_total;
};

class Graph
{
public:
	enum TpsType {
		BruteForce,
		ACS
	};

	Graph();
	Vertex *createVertex(const QString &label);
	Vertex *vertex(const QString &label) const;
	void findShortestPaths();
	Path *getPath(Vertex* from, Vertex* to);
	void printGraph();
	QList<Vertex *> vertices() const;
	bool isConnected() const;
	bool readFromFile(const QString &filename);
	bool saveToFile(const QString &filename) const;

	Path *tpsPath(TpsType type = BruteForce) const;
private:
	void dfsTraverseFrom(Vertex *v) const;
	Edge* d(QString label_i, QString label_j);
	Path *tpsPath_BruteForce();
	Path *tpsPath_ACS();
private:
	QHash<QString, Vertex *> m_vertices;
	mutable QList<Vertex *> m_visited;
    //ACSData *m_acsData;
	BruteForceData *m_bfData;
};

} // namespace GIS

#endif // GRAPH_H

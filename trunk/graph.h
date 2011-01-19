#ifndef GRAPH_H
#define GRAPH_H

#include <QString>
#include <QHash>

namespace GIS {

class ACS;
class Ant;
class Graph;
class Edge;
class ACSData;
class BruteForceData;
class Tour;

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
    QList<Vertex* > getFullPathRecursive(Vertex* from, Vertex* to);

public:
    Path(){}
    bool setVertices(const QList<Vertex *> &verts);
	QList<Vertex *> vertices() const;
	int totalCost() const;
	bool isValid() const;
    Path* getFullPath();

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

class ACS
{

public:
    ACS(Graph* g);
    Tour *acs();

private:

    void init();
    Tour* acsStep();
    void globalUpdate();
    Tour* shortestTour();

    QList<Ant* > m_ants;
    Graph* m_graph;
    ACSData* m_ACSData;

    //static const int ANT_N = 100;
    static const int ITER_N = 5;
    static const double ALPHA = 0.6;
};

class Tour
{
private:
    QList<Edge* > m_tour;
    double m_tourLength;
    Vertex* m_startPoint;

public:
    //Tour();
    Tour(Vertex* startPoint)
    {
        m_startPoint = startPoint;
    }

    void addStep(Edge* e);
    bool contains(Edge* e);
    Vertex* startPoint();
    double length();
    Edge* last();
    Path* toPath(Vertex* startVertex);
    Path* toFullPath(Vertex* startVertex);
};

class Ant
{
public:

    Ant(QList<Vertex*> vertices, ACSData* acsData);

    Tour* tour();

    int tourLength();

    void step();

    void reset(QList<Vertex*> vertices);

    void localUpdate();

private:

    double desirability(Edge* e);

    Edge* lastStep();

    Vertex* m_homeVertex;
    Vertex* m_currentVertex;
    QList<Vertex* > m_remainingVertices;
    Tour* m_tour;
    ACSData* m_ACSData;
};

} // namespace GIS

#endif // GRAPH_H

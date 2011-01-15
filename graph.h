#ifndef GRAPH_H
#define GRAPH_H

#include <QString>
#include <QHash>

namespace GIS {

	class Graph;
	class Edge;

	class Vertex
	{
	private:
		Vertex(Graph *parentGraph, const QString &label);
	public:
		Graph *graph() const;
		QList<Vertex *> connectedVertices() const;
		QString label() const;
		Edge *edgeTo(Vertex *v) const;
		Edge *connectTo(Vertex *v, int weight);
		Edge *virtuallyConnectTo(Vertex *v, int weight);
		void turnToVirtual();
                Vertex* previous(QString from);
                void setPrevious(QString from, Vertex* previous);

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

	class Graph
	{
	public:
		Graph();
		Vertex *createVertex(const QString &label);
		Vertex *vertex(const QString &label) const;
        void turnToCompleteGraph();
        void findShortestPaths();

		QList<Vertex *> vertices() const;
		bool isConnected() const;
		bool readFromFile(const QString &filename);
	private:
		void dfsTraverseFrom(Vertex *v) const;
        void dijkstraVertex(Vertex *v) const;
        Edge* d(QString label_i, QString label_j);
	private:
		QHash<QString, Vertex *> m_vertices;
		mutable QList<Vertex *> m_visited;
	};

} // namespace GIS

#endif // GRAPH_H

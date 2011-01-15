#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#include <QAbstractItemModel>
#include "graph.h"

namespace GIS {

class GraphModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	GraphModel(QObject *parent = 0);
	void setGraph(Graph *graph);

	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
private:
	struct Item {
		enum Type {
			Invalid,
			Graph,
			Vertex,
			Edge
		} type;
		Item *parent;
		Item(Type t = Invalid) : type(t), parent(0) {}
	};

	struct EdgeItem : public Item {
		EdgeItem() : Item(Item::Edge) {}
		GIS::Edge *edge;
	};
	struct VertexItem : public Item {
		VertexItem() : Item(Item::Vertex) {}
		GIS::Vertex *vertex;
		QList<EdgeItem *> edgeItems;
	};
	struct GraphItem : public Item {
		GraphItem() : Item(Item::Graph) {}
		GIS::Graph *graph;
		QList<VertexItem *> vertexItems;
	};

	GraphItem *m_graphItem;
};

} // namespace GIS

#endif // GRAPHMODEL_H

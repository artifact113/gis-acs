#include "graphmodel.h"

namespace GIS {

GraphModel::GraphModel(QObject *parent)
	: QAbstractItemModel(parent)
	, m_graphItem(0)
{
}

void GraphModel::setGraph(Graph *graph)
{
	m_graphItem = new GraphItem;
	m_graphItem->graph = graph;
	QList<Vertex *> verts = graph->vertices();
	foreach (Vertex *v, verts) {
		VertexItem *vitem = new VertexItem;
		vitem->parent = m_graphItem;
		vitem->vertex = v;
		m_graphItem->vertexItems.append(vitem);
		QList<Edge *> edges = v->edges();
		foreach (Edge *e, edges) {
			EdgeItem *eitem = new EdgeItem;
			eitem->parent = vitem;
			eitem->edge = e;
			vitem->edgeItems.append(eitem);
		}
	}

	reset();
}

int GraphModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

int GraphModel::rowCount(const QModelIndex &parent) const
{
	if (!m_graphItem) {
		return 0;
	}
	if (!parent.isValid()) {
		return 1;
	}
	Item *item = static_cast<Item *>(parent.internalPointer());

	switch (item->type) {
	case Item::Graph:
		{
			GraphItem *gi = static_cast<GraphItem *>(item);
			return gi->vertexItems.size();
		}
	case Item::Vertex:
		{
			VertexItem *vi = static_cast<VertexItem *>(item);
			return vi->edgeItems.size();
		}
	case Item::Edge:
		return 0;
	default:
		return 0;
	}
	return 0;
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid()) {
		if (row == 0 && column == 0) {
			return createIndex(row, column, (void *)m_graphItem);
		}
		return QModelIndex();
	}
	Item *item = static_cast<Item *>(parent.internalPointer());

	switch (item->type) {
	case Item::Graph:
		{
			GraphItem *gi = static_cast<GraphItem *>(item);
			if (row >= 0 && row < gi->vertexItems.size() && column == 0) {
				return createIndex(row, column, (void *)gi->vertexItems.at(row));
			}
			return QModelIndex();
		}
	case Item::Vertex:
		{
			VertexItem *vi = static_cast<VertexItem *>(item);
			if (row >= 0 && row < vi->edgeItems.size() && column >= 0 && column < 4) {
				return createIndex(row, column, (void *)vi->edgeItems.at(row));
			}
			return QModelIndex();
		}
	case Item::Edge:
	default:
		return QModelIndex();
	}
	return QModelIndex();
}

QModelIndex GraphModel::parent(const QModelIndex &child) const
{
	if (!child.isValid()) {
		return QModelIndex();
	}
	Item *item = static_cast<Item *>(child.internalPointer());
	switch (item->type) {
	case Item::Vertex:
		{
			VertexItem *vi = static_cast<VertexItem *>(item);
			int r = m_graphItem->vertexItems.indexOf(vi);
			return createIndex(r, 0, (void *)m_graphItem);
		}
	case Item::Edge:
		{
			EdgeItem *ei = static_cast<EdgeItem *>(item);
			VertexItem *vi = static_cast<VertexItem *>(ei->parent);
			int r = vi->edgeItems.indexOf(ei);
			return createIndex(r, 0, (void *)vi);
		}
	case Item::Graph:
	default:
		return QModelIndex();
	}
	return QModelIndex();
}

QVariant GraphModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if (role != Qt::DisplayRole) {
		return QVariant();
	}
	Item *item = static_cast<Item *>(index.internalPointer());
	switch (item->type) {
	case Item::Graph:
		{
			if (index.column() != 0) {
				return QVariant();
			}
			return QString("Graph");
		}
	case Item::Vertex:
		{
			if (index.column() != 0) {
				return QVariant();
			}
			VertexItem *vi = static_cast<VertexItem *>(item);
			return QString("Vertex \"%1\"").arg(vi->vertex->label());
		}
	case Item::Edge:
		{
			EdgeItem *ei = static_cast<EdgeItem *>(item);
			if (index.column() == 0) {
				return ei->edge->isVirtual() ? QString("Virtual edge") : QString("Edge");
			}
			if (index.column() == 1) {
				return QString("\"%1\"").arg(ei->edge->startPoint()->label());
			}
			if (index.column() == 2) {
				return QString("\"%1\"").arg(ei->edge->endPoint()->label());
			}
			if (index.column() == 3) {
				return QString::number(ei->edge->weight());
			}
		}
	default:
		return QVariant();
	}

	return QVariant();
}

} // namespace GIS

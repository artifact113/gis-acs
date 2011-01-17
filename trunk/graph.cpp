#include "graph.h"
#include <QFile>
#include <qmath.h>
#include <QDomDocument>
#include <QtDebug>


namespace GIS {

class Tour
{
private:
    QList<Edge* > m_tour;
    double m_tourLength;

public:
    void addStep(Edge* e)
    {
        m_tour.append(e);
        m_tourLength += e->weight();
    }

    bool contains(Edge* e)
    {
        return m_tour.contains(e);
    }

    double length()
    {
        return m_tourLength;
    }

    Edge* last()
    {
        return m_tour.last();
    }
};

class Ant
{
public:

    Ant(QList<Vertex*> vertices, ACSData* acsData)
    {
        m_homeVertex = vertices.takeAt(qrand() % vertices.size());
        m_currentVertex = m_homeVertex;
        m_remainingVertices = vertices;
        m_ACSData = acsData;
        m_tour = new Tour();
    }

    void step()
    {
        if(!m_remainingVertices.empty())
        {
            double totalDesirability;
            QList< QPair<double, Vertex*> > toGo;
            foreach(Vertex* v, m_remainingVertices)
            {
                toGo.append(QPair<double, Vertex*>(desirability(m_currentVertex->edgeTo(v)), v));
                totalDesirability += desirability(m_currentVertex->edgeTo(v));
            }

            double rand = (qrand() / RAND_MAX) * totalDesirability;

            bool stop = false;
            int curr = 0;
            int index = 0;
            while(!stop)
            {
                if(rand >= curr && rand < curr + toGo[index].first)
                {
                    Vertex* v = toGo[index].second;
                    m_tour->addStep(m_currentVertex->edgeTo(v));
                    m_currentVertex = toGo[index].second;
                    m_remainingVertices.removeOne(v);
                    stop = true;
                }
                else
                    ++index;
            }
        }
        else
        {
            m_tour->addStep(m_currentVertex->edgeTo(m_homeVertex));
            m_currentVertex = m_homeVertex;
        }
    }

    void localUpdate()
    {
        Edge* e = lastStep();
        double pheromoneUpdated = (1 - m_ACSData->PHI)*m_ACSData->pheromone(e) + (m_ACSData->PHI*m_ACSData->pheromone0);
        m_ACSData->setPheromone(e, pheromoneUpdated);
    }

private:

    double desirability(Edge* e)
    {
        return m_ACSData->pheromone(e)*qPow(1/e->weight(), BETA);
    }

    Edge* lastStep()
    {
        return m_tour->last();
    }

    Vertex* m_homeVertex;
    Vertex* m_currentVertex;
    QList<Vertex* > m_remainingVertices;
    Tour* m_tour;
    ACSData* m_ACSData;
};

class ACS
{

public:
    ACS(Graph* g)
    {
        m_ACSData = new ACSData();
        m_ACSData->setGraph(g);
    }

	Path *acs()
    {
        init();
        for(int i = 0; i < ITER_N; ++i)
        {
            acsStep();
            globalUpdate();
            updateBest();
        }
        return shortestPath();
    }

private:

    // init()
    //    For k:=1 to m do
    //        Let rk1 be the starting city for ant k
    //        Jk(rk1):= {1, ..., n} - rk1
    //        /* Jk(rk1) is the set of yet to be visited cities for
    //        ant k in city rk1 */
    //        rk:= rk1 /* rk is the city where ant k is located */
    //    End-fo
    void init()
    {
        // Create Ants
        for(int i = 0; i < ANT_N; ++i)
        {
			m_ants.append(new Ant(m_graph->vertices()));
        }
    }

    // acsStep():
    //    If i<n
    //    Then
    //        For k:=1 to m do
    //            Choose the next city sk according to Eq. (3) and Eq. (1)
    //            Jk(sk):= Jk(rk) - sk
    //            Tourk(i):=(rk ,sk)
    //        End-for
    //    Else
    //        For k:=1 to m do
    //            /* In this cycle all the ants go back to the initial city rk1 */
    //            sk := rk1
    //            Tourk(i):=(rk ,sk)
    //        End-for
    //    End-if
    //    /* In this phase local updating occurs and pheromone is
    //    updated using Eq. (5)*/
    //    For k:=1 to m do
    //        t(rk ,sk):=(1-r)t(rk ,sk)+ rt0
    //        rk := sk /* New city for ant k */
    //    End-for
    void acsStep()
    {
        for(int i = 0; i < N; ++i)
        {
            for(int k = 0; k < K; ++k)
            {
                m_ants[k]->step();
            }
            for(int k = 0; k < K; ++k)
            {
                m_ants[k]->localUpdate();
            }
        }
    }

    // globalUpdate()
    //    For k:=1 to m do
    //        Compute Lk /* Lk is the length of the tour done by ant k*/
    //    End-for
    //    Compute Lbest
    //    /*Update edges belonging to Lbest using Eq. (4) */
    //    For each edge (r,s)
    //        t(rk ,sk):=(1-a)t( rk ,sk)+ a (Lbest)-1
    //    End-for
    void globalUpdate()
    {
        int Lbest = INT_MAX;
        Tour* tourBest = NULL;

        // calculate best tour
        for(int k = 0; k < K; ++k)
        {
            int Lk = m_ants[k]->tourLength();
            if(Lk < Lbest)
            {
                Lbest = Lk;
                tourBest = m_ants[k]->tour();
            }
        }

        // update pheromone
        QList<Edge*> edges = m_pheromones.keys();

        foreach(Edge* e, edges)
        {
            m_ACSData->setPheromone(e, (1 - ALPHA)*m_ACSData->pheromone(e));
            if(tourBest->contains(e))
            {
                m_ACSData->setPheromone(e,  m_ACSData->pheromone(e)+ 1/tourBest->length());
            }
        }
    }

	Path *shortestPath();

    QList<Ant* > m_ants;
    Graph* m_graph;
    ACSData* m_ACSData;

    static const int ANT_N = 100;
    static const int ITER_N = 100;
    static const double ALPHA = 0.6;

    int N;
    int K;
};

class ACSData
{
private:
    QHash<Edge*, double> m_pheromones;
    Graph* m_graph;

public:

    void setGraph(Graph* g)
    {
        m_graph = g;

        N = g->vertices().size();

        // init ACSData
        foreach(Vertex* v, g->vertices())
        {
            QList<Edge*> edges = v->edges();

            foreach(Edge* e, edges)
            {
                addEdge(e);
            }
        }
    }

    QList<Edge*> edges()
    {
        return m_pheromones.keys();
    }

    void addEdge(Edge* e)
    {
        m_pheromones[e] = pheromone0;
    }

    void addEdge(Edge* e, double pheromone)
    {
        m_pheromones[e] = pheromone;
    }

    double pheromone(Edge* e)
    {
        return m_pheromones[e];
    }

    void setPheromone(Edge* e, double pheromone)
    {
        m_pheromones[e] = pheromone;
    }

    int N;
    static const int K = 6;
    static const double BETA = 0.6;
    static const int pheromone0 = 10;
    static const double PHI = 0.9;
};

struct BruteForceData {
	BruteForceData() {}
	QList<QList<Vertex *> > permutations;
	void addElement(Vertex *v) {
		if (permutations.isEmpty()) {
			permutations.append(QList<Vertex *>() << v);
			return;
		}
		if (permutations.size() == 1) {
			permutations.append(permutations.first());
			permutations.first().append(v);
			permutations.last().prepend(v);
			return;
		}
		int count = permutations.first().size() + 1;
		QList<QList<Vertex *> > newPerms;
		foreach (const QList<Vertex *> &perm, permutations) {
			for (int i = 0; i < count; ++i) newPerms.append(perm);
		}

		bool dir = true;
		for (int i = 0; i < newPerms.size(); ++i) {
			int index = dir ? (count - (i % count) - 1) : (i % count);
			newPerms[i].insert(index, v);
			if (!index && dir) {
				dir = false;
			}
			if (index == count - 1 && !dir) {
				dir = true;
			}
		}
		permutations = newPerms;
	}
};

bool pathLessThan(Path *p1, Path *p2) {
	return p1->totalCost() < p2->totalCost();
}

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
	if (m_bfData) {
		delete m_bfData;
	}

	m_bfData = new BruteForceData;
	foreach (Vertex *v, m_vertices.values()) {
		m_bfData->addElement(v);
	}

	QList<Path *> paths;
	foreach (const QList<Vertex *> &perm, m_bfData->permutations) {
		Path *path = new Path(this);
		foreach (Vertex *v, perm) {
			path->appendVertex(v);
		}
		path->appendVertex(perm.first());
		paths.append(path);
	}

	qSort(paths.begin(), paths.end(), pathLessThan);

	return paths.first();
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

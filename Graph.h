//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef GRAPH_H
#define GRAPH_H

//#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>

class QTextStream;
class QGraphicsItem;

class Vertex;
class Edge;

class Graph //: public QObject
{
	//Q_OBJECT
public:
	/** ctor */
	Graph();
	/**
	 * @return a map of ids to the vertex pointers of the graph
	 */
	QMap<uint,Vertex*> vertices() const;
	/**
	 * @return a map of vertex pairs to edges in the graph
	 * @note in an undirected graph both head,tail and tail,head should be checked
	 */
	QMap<QPair<Vertex*,Vertex*>,Edge*> edges() const;

	void vertexAdded( Vertex* v );
	void edgeAdded( Edge* e );

	void vertexRemoved( Vertex* v );
	void edgeRemoved( Edge* e );

	bool isValidNewId(uint id) const;
	/**
	 * @brief Reads a graph from a format based on DOT
	 *
	 * This function is given a QTextStream and reads it, producing a graph.
	 * The format goes basically as follows:

	UID [label="LABELTEXT",pos="X Y"];
	UID -- UID2 [weight="W"];

	 * where UID & UID2 are (unique) uints, LABELTEXT is the text attribute,
	 * and X, Y, & W are doubles. e.g.

	122453 [label="Some Random Node",pos="1.23 8.42"];
	125367 [label="NODE2",pos="5.43 3.21"];
	122453 -- 125367 [weight="2.3"];

	 * note that statements must be on seperate lines and nodes must be declared
	 * before use, unlike in DOT where they can be defined implicitly.
	 * @note it's your responsibility to delete the graph when you're finished
	 * @note do not give it a stream from stdin, it will cause an infinite loop
	 * @param s the stream to read from
	 * @param parent the parent object of the newly created nodes/edges
	 * @return a pointer to the new Graph object
	 */
	static Graph* readGraph(QTextStream *s, QGraphicsItem *parent = 0);
	/**
	 * @brief Writes a graph to a format based on DOT
	 * This function writes @param g into @param s
	 * @see readGraph
	 * @param s the stream to write to
	 * @param g the graph to write
	 */
	static void  writeGraph(QTextStream *s, Graph *g);

	/**
	 * Lays out the graph using the Kamada-Kawai spring-based algorithm
	 * @param maxiter the maximum number of iterations, -1 = until epsilon
	 * @param epsilon epsilon
	 * @param initialize if true, lay out the initial position as a regular
	 * n-sided polygon, where n is the number of vertices*/
	void layoutGraph( int maxiter, qreal epsilon, bool initialize = true );
private:
	//functions for implementing Kamada-Kawai algorithm
	inline qreal kij( Vertex *i, Vertex *j );
	inline qreal lij( Vertex *i, Vertex *j );
	inline qreal dij( Vertex *i, Vertex *j );
	qreal del_E__del_xm(Vertex *m);
	qreal del_E__del_ym(Vertex *m);
	qreal del2_E__del_x2m(Vertex *m);
	qreal del2_E__delxm_delym(Vertex *m);
	qreal del2_E__del_y2m(Vertex *m);
	qreal dx(Vertex *m);
	qreal dy(Vertex *m);
	qreal delta_m(Vertex *m);

	QMap<uint,Vertex*> m_vertices;
	QMap<QPair<Vertex*,Vertex*>,Edge*> m_edges;
};

#endif //include guard


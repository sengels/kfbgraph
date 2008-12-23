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

class QTextStream;
class QGraphicsItem;

class Vertex;
class Edge;

class Graph //: public QObject
{
	//Q_OBJECT
public:
	Graph();
	QMap<uint,Vertex*> vertices() const;
	QList<Edge*> edges() const;

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
private:
	QMap<uint,Vertex*> m_vertices;
	QList<Edge*> m_edges;
};

#endif //include guard


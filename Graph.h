//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef GRAPH_H
#define GRAPH_H

#include <QtCore/QList>
#include <QtCore/QMap>

class Vertex;
class Edge;

class Graph : public QObject
{
	Q_OBJECT
public:
	Graph();
	QMap<uint,Vertex*> vertices() const;
	QList<Edge*> edges() const;

	void vertexAdded( Vertex* v );
	void edgeAdded( Edge* e );

	void vertexRemoved( Vertex* v );
	void edgeRemoved( Edge* e );

	bool isValidNewId(uint id) const;
private:
	QMap<uint,Vertex*> m_vertices;
	QList<Edge*> m_edges;
}

#endif //include guard


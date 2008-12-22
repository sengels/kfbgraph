//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef EDGE_H
#define EDGE_H

#include <QtGui/QGraphicsLineItem>

class Vertex;
class Graph;

class Edge : public QGraphicsLineItem
{
public:
	Edge(Graph *g, Vertex *head, Vertex *tail,
	     qreal weight = 1.0, QGraphicsItem *parent = 0);

	Vertex* head() const;
	bool isHead( Vertex* v ) const;

	Vertex* tail() const;
	bool isTail( Vertex* v ) const;

	qreal weight() const;
	void setWeight(const qreal &w);
private:
	Graph *m_g;
	Vertex *m_head;
	Vertex *m_tail;
	qreal m_w;
}
#endif //include guard


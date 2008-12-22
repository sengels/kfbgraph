//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

// own
#include "Vertex.h"

// Qt
#include <QtGui/QGraphicsRectItem>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

//C std lib for rand()
#include <stdlib.h>

// other graphs
#include "Graph.h"
#include "Edge.h"

Vertex::Vertex(Graph *g, uint id, const QString &text, QPointF nodePos,
               QGraphicsItem *parent) 
      : QGraphicsRectItem(parent)
{
	m_g = g;
	m_text = text;
	if( id == 0 )
		for(;;) {
			id = (uint)(rand());
			if( m_g->isValidNewId(id) )
			break;
		}
	m_id = id;

	m_edges = QList<Edge*>;
	m_adjacent = QMap<uint,Vertex*>;

	m_nodePos = nodePos;

	m_g->vertexAdded( this );
}

uint Vertex::id() const
{
	return m_id;
}

int Vertex::degree() const
{
	return m_edges.size();
}

QString Vertex::text() const
{
	return m_text;
}

void Vertex::setText( const QString &text )
{
	m_text = text;
}

QMap<uint,Vertex*> Vertex::adjacent() const
{
	return m_adjacent;
}

QList<Edge*> Vertex::edges() const
{
	return m_edges;
}

Edge* Vertex::createEdge( Vertex *tail, qreal weight )
{
	Edge *e = new Edge( m_g, this, tail, weight, parentItem() );
	return e;
}

QPointF Vertex::nodePos() const
{
	return m_nodePos();
}

void Vertex::setNodePos( QPointF pos )
{
	m_nodePos = pos;
}

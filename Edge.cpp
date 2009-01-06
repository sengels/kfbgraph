//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "Edge.h"

#include <QtGui/QGraphicsLineItem>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtCore/QLineF>
#include <QtCore/QDebug>

#include "Vertex.h"
#include "Graph.h"

static const QPen EDGEPEN( QBrush(QColor(Qt::black)), 2.0 );

Edge::Edge( Graph *g, Vertex *head, Vertex *tail, qreal weight,
            QGraphicsItem *parent )
    : QGraphicsLineItem(parent)
{
	m_g = g;
	m_head = head; m_tail = tail;
	m_w = weight;

	setLine( QLineF( m_head->nodePos(), m_tail->nodePos() ) );
	setPen( EDGEPEN );
	//this prevents edges being drawn over top of vertices
	setZValue(0.9);
	
	m_g->edgeAdded(this);
}

Vertex* Edge::head() const
{
	return m_head;
}

bool Edge::isHead( Vertex *v ) const
{
	if( v == m_head )
		return true;
	else
		return false;
}

Vertex* Edge::tail() const
{
	return m_tail;
}

bool Edge::isTail( Vertex *v ) const
{
	if( v == m_tail )
		return true;
	else
		return false;
}

qreal Edge::weight() const
{
	return m_w;
}

void Edge::setWeight( const qreal &weight )
{
	m_w = weight;
}

void Edge::updatePos()
{
// 	qDebug() << "UpdatePos setting to " <<QLineF( m_head->nodePos(), m_tail->nodePos() );
	setLine( QLineF( m_head->nodePos(), m_tail->nodePos() ) );
}
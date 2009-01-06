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
#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>
#include <QtCore/QMap>
#include <QtCore/QString>

//C std lib for rand()
#include <stdlib.h>

// other graphs
#include "Graph.h"
#include "Edge.h"

static const QPen VERTEXPEN(QBrush(QColor(Qt::black)), 0.5);
static const QBrush VERTEXBRUSH( QColor( 0xFF, 0xFF, 0xFF, 0x80 ) );
static const QFont VERTEXFONT( "Helvetica", 12, QFont::Normal );

Vertex::Vertex(Graph *g, uint id, QString text, QPointF nodePos,
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

	m_edges = QList<Edge*>();
	m_adjacent = QMap<uint,Vertex*>();

	m_nodePos = nodePos;

	setPen  ( VERTEXPEN );
	setBrush( VERTEXBRUSH );

	//find the size & position of the vertex
	QFontMetrics m( VERTEXFONT );
	QRect r = m.boundingRect(m_text);
	// adjust for the fact that nodePos is the centre of the rect
	setRect( QRectF( m_nodePos - QPointF( r.width()/2, r.height()/2 ),
	         QSizeF( r.size() ) ) );

	setZValue(2.0);

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
	//find the size & position of the vertex
	QFontMetrics m( VERTEXFONT );
	QRect r = m.boundingRect(m_text);
	// adjust for the fact that nodePos is the centre of the rect
	setRect( QRectF( m_nodePos - QPointF( r.width()/2, r.height()/2 ),
	         QSizeF( r.size() ) ) );
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
	return m_nodePos;
}

void Vertex::setNodePos( QPointF pos )
{
	m_nodePos = pos;
	// adjust for the fact that nodePos is the centre of the rect
	setRect( QRectF( m_nodePos - QPointF(rect().width()/2, rect().height()/2),
	         rect().size() ) );
	for(QList<Edge*>::const_iterator i = m_edges.constBegin();
	    i != m_edges.constEnd(); ++i )
	{
		(*i)->updatePos();
	}
}

void Vertex::addEdge(Edge *e)
{
	m_edges.append(e);
	if(e->head() == this)
		m_adjacent.insert(e->tail()->id(), e->tail());
	else
		m_adjacent.insert(e->head()->id(), e->head());
}

void Vertex::removeEdge(Edge *e)
{
	m_edges.removeAll(e);
	if(e->head() == this)
		m_adjacent.remove(e->tail()->id());
	else
		m_adjacent.remove(e->head()->id());
}

void Vertex::paint( QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget )
{
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->setFont( VERTEXFONT );

	painter->drawRect(rect());

	painter->drawText(rect(), Qt::AlignCenter, m_text);
}
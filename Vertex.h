//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef VERTEX_H
#define VERTEX_H

#include <QtGui/QGraphicsRectItem>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

class Graph;
class Edge;

class Vertex : public QGraphicsRectItem
{
public:
	Vertex(Graph *g, uint id = 0, QString text = QString(),
	       QPointF nodePos = QPointF(), QGraphicsItem *parent = 0);

	Edge* createEdge( Vertex *tail, qreal weight = 1.0 );
	QList<Edge*> edges() const;

	QMap<uint,Vertex*> adjacent() const;

	uint id() const;
	int degree() const;

	QString text() const;
	void setText( const QString &text );

	/* This is because the graphicitem's pos is at 0,0 but the position of
	 * the node should be in the centre of the rect, not the corner */
	QPointF nodePos() const;
	void setNodePos( QPointF pos );
private:
	Graph *m_g;
	QList<Edge*> m_edges;
	QMap<uint,Vertex*> m_adjacent;
	QPointF m_nodePos;
	QString m_text;
};
#endif //include guard


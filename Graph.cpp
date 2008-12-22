//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

// own
#include "Graph.h"

// Qt
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtDebug>

#include "Vertex.h"
#include "Edge.h"

Graph::Graph()
{
	m_vertices = QMap<uint,Vertex*>;
	m_edges = QList<Edge*>;
}

QMap<uint,Vertex*> Graph::vertices() const
{
	return m_vertices;
}

QList<Edge*> Graph::edges() const
{
	return m_edges;
}

void Graph::vertexAdded( Vertex* v )
{
	m_vertices.insert( v->id(), v );
}

void Graph::edgeAdded( Edge* e )
{
	m_edges.append( e );
}

void Graph::vertexRemoved( Vertex* v )
{
	if( m_vertices.contains( v->id() ) )
		m_vertices.remove( v );

	/* Once a vertex is removed we must look through for any edges
	 * that link to it and remove them too */
	QList<Edge*>::iterator i = m_edges.begin();
	for(;;) {
		if( i->isHead(v) || i->isTail(v) ) {
			Edge *e = *i;
			//if the edge is in a QGraphicsScene we must remove it
			if( e->scene() ) {
				e->scene()->removeItem( e );
			}
			// Remove the item at i and advance to the next item
			i = m_edges.erase( i );
			delete e;
			if( i == m_edges.end() )
				goto end;
		} else {
			++i; //advance to next item
		}
	}
	end:
	return;
}

/* Isolated nodes are not autoremoved so this is fairly simple */
void Graph::edgeRemoved( Edge* e )
{
	m_edges.removeAll( e );
}

/* A valid id is one that isn't already used */
bool Graph::isValidNewId(uint id) const
{
	return !m_vertices.contains( id );
}

/*These snippets are defined here to make regexps more readable */
static const QString qE = "([0-9]+\\.?[0-9]*)"; //captures qreals
static const QString uE = "([0-9]+)"; //captures uints

static Graph* Graph::readGraph( QTextStream *s, QGraphicsItem *parent )
{
	Graph *g = new Graph();

	//This regexp matches the definition of a vertex
	QRegExp vdef("\\s*"+uE+"\\s+\\[label=\"(.*)\",pos=\""+qE+"\\s+"+qE+"\"\\]");
	//This regexp matches the definition of an edge
	QRegExp edef("\\s*"+uE+"\\s*--\\s*"+uE+"\\s*\\[weight=\""+qE+"\"\\]" );

	while( !s->atEnd() ) {
		QString curline = s->readLine();
		if( curline.contains(vdef) ) { //Does this line define a vertex?
			qDebug() << vdef.pattern() << "  MATCHES  " << curline;

			bool ok = false; //pessimists are never dissapointed
			uint id = vdef.cap(1).toUInt(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toUInt(" << vdef.cap(1);
				continue;
			} else ok = false;

			QString label = vdef.cap(2);

			qreal x = vdef.cap(3).toDouble();
			if(!ok) {
				qDebug() << "error: couldn't do toDouble(" << vdef.cap(3);
				continue;
			} else ok = false;
			qreal y = vdef.cap(4).toDouble();
			if(!ok) {
				qDebug() << "error: couldn't do toDouble(" << vdef.cap(4);
				continue;
			}

			qDebug() << "New Vertex with label \"" << label << "\", id"
			         << id << ", and X,Y " << x << "," << y;

			Vertex *v = new Vertex(g,id,label,QPointF(x,y),parent);

		} else if( curline.contains(edef) ) { //Does this line define an edge?
			qDebug() << edef.pattern() << "  MATCHES  " << curline;
			bool ok = false; //pessimists are never dissapointed

			uint head_id = edef.cap(1).toUInt(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toUInt(" << edef.cap(1);
				continue;
			} else ok = false;

			uint tail_id = edef.cap(2).toUInt(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toUInt(" << edef.cap(2);
				continue;
			} else ok = false;

			qreal w = edef.cap(3).toDouble();
			if(!ok) {
				qDebug() << "error: couldn't do toDouble(" << edef.cap(3);
				continue;
			}

			qDebug()<<"New edge (w="<<w<<") between "<<head_id<<" "<<tail_id;
			if( !g->vertices().contains(head_id) ) {
				qDebug() << "error: id " << head_id << " not found";
				continue;
			} else if( !g->vertices().contains(tail_id) ) {
				qDebug() << "error: id " << tail_id << " not found";
				continue;
			}

			Edge *e = new Edge(g, g->vertices().value(head_id),
			                      g->vertices().value(tail_id), w );
		}
	}
	return g;
}

static void Graph::writeGraph( QTextStream *s, Graph *g )
{
	//get a copy to use a bunch of times
	QMap<uint,Vertex*> vertices = g->vertices();
	for(QMap<uint,Vertex*>::const_iterator i = vertices.constBegin();
	    i != vertices.constEnd(); ++i )
	{
		//i.key is the id, which comes first
		s << '\t' << i.key() << " [";
		//Replace " with \" to avoid confusion when loading
		s << "label=\"" << i.value()->text().replace("\"", "\\\"") << "\",";
		QPointF p = i.value()->nodePos();
		s << "pos=\"" << p.x() << " " << p.y() << "\"];\n";
	}
	//add a seperator between node defs and edge defs
	s << "\n\n\n\n";
	//get a copy to use a bunch of times
	QList<Edge*> edges = g->edges();
	for(QList<Edge*>::iterator i = edges.constBegin();
	    i != edges.constEnd(); ++i )
	{
		s << '\t' << i->head()->id() << " -- " << i->tail()->id();
		s << " [weight=\"" << i->weight() << "\"];\n";
	}
}












//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

// own
#include "Graph.h"

//math
#include <cmath>

// QtGui
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>

// QtCore
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include "Vertex.h"
#include "Edge.h"

/* // not sure if these will be needed
//like sine, but faster
inline qreal fsin(qreal x)
{
	return x−pow(x,3)/6+pow(x,5)/120−pow(x,7)/5040+pow(x,9)/362880−pow(x,11)/39916800;
}

//like cosine, but faster
inline qreal fcos(qreal x)
{
	return 1-pow(x,2)/2+pow(x,4)/24-pow(x,6)/720+pow(x,8)/40328-pow(x,10)/3628800;
} */

Graph::Graph()
{
	m_vertices = QMap<uint,Vertex*>();
	m_edges = QMap<QPair<Vertex*,Vertex*>,Edge*>();
}

QMap<uint,Vertex*> Graph::vertices() const
{
	return m_vertices;
}

QMap<QPair<Vertex*,Vertex*>,Edge*> Graph::edges() const
{
	return m_edges;
}

void Graph::vertexAdded( Vertex* v )
{
	m_vertices.insert( v->id(), v );
}

void Graph::edgeAdded( Edge* e )
{
	m_edges.insert( qMakePair(e->head(), e->tail()), e );
	m_edges.insert( qMakePair(e->tail(), e->head()), e );
	e->head()->addEdge(e);
	e->tail()->addEdge(e);
}

void Graph::vertexRemoved( Vertex* v )
{
	if( m_vertices.contains( v->id() ) )
		m_vertices.remove( v->id() );

	/* Once a vertex is removed we must look through for any edges
	 * that link to it and remove them too */
	QMap<QPair<Vertex*,Vertex*>,Edge*>::iterator i = m_edges.begin();
	for(QMap<QPair<Vertex*,Vertex*>,Edge*>::iterator i;
	    i != m_edges.end(); ++i) {
		if( (*i)->isHead(v) || (*i)->isTail(v) ) {
			Edge *e = *i;
			//if the edge is in a QGraphicsScene we must remove it
			if( e->scene() ) {
				e->scene()->removeItem( e );
			}
			// Remove the item at i and advance to the next item
			i = m_edges.erase( i );
			delete e;
		} else {
			++i; //advance to next item
		}
	}
}

/* Isolated nodes are not autoremoved so this is fairly simple */
void Graph::edgeRemoved( Edge* e )
{
	m_edges.remove( qMakePair(e->head(), e->tail()) );
	m_edges.remove( qMakePair(e->tail(), e->head()) );
	e->head()->removeEdge(e);
	e->tail()->removeEdge(e);
}

/* A valid id is one that isn't already used */
bool Graph::isValidNewId(uint id) const
{
	return !m_vertices.contains( id );
}

/*These snippets are defined here to make regexps more readable */
static const QString qE = "([0-9]+\\.?[0-9]*)"; //captures qreals
static const QString uE = "([0-9]+)"; //captures uints

Graph* Graph::readGraph( QTextStream *s, QGraphicsItem *parent )
{
	Graph *g = new Graph();

	//This regexp matches the definition of a vertex
	QRegExp vdef("\\s*"+uE+"\\s+\\[label=\"(.*)\",pos=\""+qE+"\\s+"+qE+"\"\\]");
	//This regexp matches the definition of an edge
	QRegExp edef("\\s*"+uE+"\\s*--\\s*"+uE+"\\s*\\[weight=\""+qE+"\"\\]" );

	while( !s->atEnd() ) {
		QString curline = s->readLine();
		if( curline.contains(vdef) ) { //Does this line define a vertex?
			//qDebug() << vdef.pattern() << "  MATCHES  " << curline;

			bool ok = false; //pessimists are never dissapointed
			uint id = vdef.cap(1).toUInt(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toUInt(" << vdef.cap(1);
				continue;
			} else ok = false;

			QString label = vdef.cap(2);

			qreal x = vdef.cap(3).toDouble(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toDouble(" << vdef.cap(3);
				continue;
			} else ok = false;
			qreal y = vdef.cap(4).toDouble(&ok);
			if(!ok) {
				qDebug() << "error: couldn't do toDouble(" << vdef.cap(4);
				continue;
			}

			qDebug() << "New Vertex with label \"" << label << "\", id"
			         << id << ", and X,Y " << x << "," << y;

			Vertex *v = new Vertex(g,id,label,QPointF(x,y),parent);

		} else if( curline.contains(edef) ) { //Does this line define an edge?
			//qDebug() << edef.pattern() << "  MATCHES  " << curline;
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

			qreal w = edef.cap(3).toDouble(&ok);
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

void Graph::writeGraph( QTextStream *s, Graph *g )
{
	//get a copy to use a bunch of times
	QMap<uint,Vertex*> vertices = g->vertices();
	for(QMap<uint,Vertex*>::const_iterator i = vertices.constBegin();
	    i != vertices.constEnd(); ++i )
	{
		//i.key is the id, which comes first
		*s << '\t' << i.key() << " [";
		//Replace " with \" to avoid confusion when loading
		*s << "label=\"" << i.value()->text().replace("\"", "\\\"") << "\",";
		QPointF p = i.value()->nodePos();
		*s << "pos=\"" << p.x() << " " << p.y() << "\"];\n";
	}
	//add a seperator between node defs and edge defs
	*s << "\n\n\n\n";
	//get a copy to use a bunch of times
	QMap<QPair<Vertex*, Vertex*>, Edge*> edges = g->edges();
	for(QMap<QPair<Vertex*, Vertex*>, Edge*>::const_iterator i =
	    edges.constBegin(); i != edges.constEnd(); ++i )
	{
		*s << '\t' << (*i)->head()->id() << " -- " << (*i)->tail()->id();
		*s << " [weight=\"" << (*i)->weight() << "\"];\n";
	}
}

inline qreal Graph::kij( Vertex *i, Vertex *j )
{
	return m_edges.value(qMakePair(i,j))->weight() / pow( dij( i,j ), 2.0 );
}

inline qreal Graph::lij( Vertex *i, Vertex *j )
{
	return 100.0;
}

inline qreal Graph::dij( Vertex *i, Vertex *j )
{
	return sqrt( pow( i->nodePos().x() - j->nodePos().x(), 2.0 )
	           + pow( i->nodePos().y() - j->nodePos().y(), 2.0 ) );
}

//kk89 eq 7
qreal Graph::del_E__del_xm(Vertex *m)
{
	qreal result = 0;
	//qDebug() << "begin del_E__del_xm" << result;
	QPointF mp = m->nodePos();
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		if( *i == m || !m_edges.contains(qMakePair(*i,m)) )
			continue;
		QPointF ip = (*i)->nodePos();
		qreal top = lij( m, *i ) * ( mp.x() - ip.x() );
		qreal bot = pow( pow( mp.x() - ip.x(), 2.0)
		               + pow( mp.y() - ip.y(), 2.0), 0.5 );
		//qDebug() << "\tkij( m,*i )" << kij( m,*i );
		//qDebug() << "\t( mp.x() - ip.x() )" << ( mp.x() - ip.x() );
		//qDebug() << "\ttop/bot" << top << bot <<top/bot;
		result += kij( m,*i ) * ( ( mp.x() - ip.x() ) - top/bot);
		//qDebug() << "del_E__del_xm" << result;
	}
	//qDebug() << "final del_E__del_xm" << result;
	return result;
}

//kk89 eq 8
qreal Graph::del_E__del_ym(Vertex *m)
{
	qreal result = 0;
	//qDebug() << "begin del_E__del_ym" << result;
	QPointF mp = m->nodePos();
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		if( *i == m || !m_edges.contains(qMakePair(*i,m)) )
			continue;
		QPointF ip = (*i)->nodePos();
		qreal top = lij( m, *i ) * ( mp.y() - ip.y() );
		qreal bot = pow( pow( mp.x() - ip.x(), 2.0)
		               + pow( mp.y() - ip.y(), 2.0), 0.5 );
		//qDebug() << "\tkij( m,*i )" << kij( m,*i );
		//qDebug() << "\t( mp.y() - ip.y() )" << ( mp.y() - ip.y() );
		//qDebug() << "\ttop/bot" << top << bot << top/bot;
		result += kij( m,*i ) * ( ( mp.y() - ip.y() ) - top/bot);
		//qDebug() << "del_E__del_ym" << result;
	}
	//qDebug() << "final del_E__del_ym" << result;
	return result;
}

//kk89 eq 13
qreal Graph::del2_E__del_x2m(Vertex *m)
{
	qreal result = 0;
	//qDebug() << "begin del2_E__del_x2m" << result;
	QPointF mp = m->nodePos();
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		if( *i == m || !m_edges.contains(qMakePair(*i,m)) )
			continue;
		QPointF ip = (*i)->nodePos();
		qreal top = lij( m, *i ) * pow( mp.y() - ip.y(), 2.0 );
		qreal bot = pow( pow( mp.x() - ip.x(), 2.0)
		               + pow( mp.y() - ip.y(), 2.0), 1.5 );
		result += kij( m,*i ) * (1.0 - top/bot);
		//qDebug() << "del2_E__del_x2m" << result;
	}
	//qDebug() << "final del2_E__del_x2m" << result;
	return result;
}

//kk89 eq 14, eq 15
qreal Graph::del2_E__delxm_delym(Vertex *m)
{
	qreal result = 0;
	//qDebug() << "begin del2_E__delxm_delym" << result;
	QPointF mp = m->nodePos();
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		if( *i == m || !m_edges.contains(qMakePair(*i,m)) )
			continue;
		QPointF ip = (*i)->nodePos();
		qreal top = lij( m, *i ) * (mp.y() - ip.y()) * (mp.x() - ip.x());
		qreal bot = pow( pow( mp.x() - ip.x(), 2.0)
		               + pow( mp.y() - ip.y(), 2.0), 1.5 );
		result += kij( m,*i ) * (top/bot);
		//qDebug() << "del2_E__delxm_delym" << result;
	}
	//qDebug() << "final del2_E__delxm_delym" << result;
	return result;
}

//kk89 eq 16
qreal Graph::del2_E__del_y2m(Vertex *m)
{
	qreal result = 0;
	//qDebug() << "begin del2_E__del_y2m" << result;
	QPointF mp = m->nodePos();
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		if( *i == m || !m_edges.contains(qMakePair(*i,m)) )
			continue;
		QPointF ip = (*i)->nodePos();
		qreal top = lij( m, *i ) * pow( mp.x() - ip.x(), 2.0 );
		qreal bot = pow( pow( mp.x() - ip.x(), 2.0)
		               + pow( mp.y() - ip.y(), 2.0), 1.5 );
		result += kij( m,*i ) * (1.0 - top/bot);
		//qDebug() << "del2_E__del_y2m" << result;
	}
	//qDebug() << "final del2_E__del_y2m" << result;
	return result;
}

//from kk89 eq 11, eq 12
qreal Graph::dx(Vertex *m)
{
	qreal top = ((del2_E__delxm_delym(m)*del_E__del_ym(m))/del2_E__del_y2m(m))
	            - del_E__del_xm(m);
	qreal bot = del2_E__del_x2m(m) - pow(del2_E__delxm_delym(m), 2.0)/
	                                 del2_E__del_y2m(m);

	qreal result = top/bot;
	//qDebug() << "dx" << result;
	return result;
}

//from kk89 eq 11, eq 12
qreal Graph::dy(Vertex *m)
{
	qreal top = ((del2_E__delxm_delym(m)*del_E__del_xm(m))/del2_E__del_x2m(m))
	            - del_E__del_ym(m);
	qreal bot = del2_E__del_y2m(m) - pow(del2_E__delxm_delym(m), 2.0)/
	                                 del2_E__del_x2m(m);

	qreal result = top/bot;
	//qDebug() << "dy" << result;
	return result;
}

//kk89 eq 9
qreal Graph::delta_m(Vertex *m)
{
	qreal a = pow(del_E__del_xm(m), 2.0);
	qreal b = pow(del_E__del_ym(m), 2.0);
	return sqrt( a + b );
}


void Graph::layoutNGon()
{
	int n = m_vertices.size();
	qreal inc = (2.0 * M_PI)/n;
	qreal angle = 0.1;
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		//qDebug() << *i << "set to" << QPointF(100.0*cos(angle),100.0*sin(angle));
		(*i)->setNodePos( QPointF(100.0*cos(angle),100.0*sin(angle)) );
		angle += inc;
	}
}


void Graph::layoutRandom(qreal max)
{
	for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
	    i != m_vertices.constEnd(); ++i )
	{
		qreal x = (drand48() * max * 2 ) -max;
		qreal y = (drand48() * max * 2 ) -max;
		(*i)->setNodePos( QPointF(x,y) );
	}
}

void Graph::layoutKamadaKawai( int maxiter, qreal epsilon, bool initialize)
{
	qDebug() << "Laying out KamadaKawai";
	if( initialize )
		layoutRandom( 100.0 );
		//layoutNGon();
	if(maxiter < 0)
		maxiter = 65535;
	for(int iteration = 0; iteration < maxiter; ++iteration) {
		uint id = 0;
		qreal maxdelta_m = 0.0;
		for(QMap<uint,Vertex*>::const_iterator i = m_vertices.constBegin();
		    i != m_vertices.constEnd(); ++i )
		{
			qreal curdelta_m = qAbs(delta_m(*i));
			//qDebug() << "curdelta_m,maxdelta_m"<< curdelta_m << "\t" << maxdelta_m;
			if( curdelta_m >= maxdelta_m ) {
				maxdelta_m = curdelta_m;
				id = (*i)->id();
			}
		}
		Vertex *m = m_vertices.value(id);
		qreal curdx = dx(m);
		qreal curdy = dy(m);
		qDebug() << "Picked node with id=" << id <<", moving by" << QPointF(curdx,curdy) << " to "<< m->nodePos();
		m->setNodePos( m->nodePos() + QPointF(curdx,curdy) );
		if(qAbs(delta_m(m)) < epsilon ) {
			qDebug() << "Breaking early: iteration, delta_m, epsilon" << iteration << qAbs(delta_m(m)) << epsilon;
			break;
		}
	}
}


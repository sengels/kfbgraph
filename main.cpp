#include <QtGui/QApplication>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "Graph.h"
#include "Vertex.h"
#include "Edge.h"

int main(int argc, char *argv[])
{
	QApplication app(argc,argv);
	QStringList args = app.arguments();
	QFile infile(args.at(1));
	infile.open(QIODevice::ReadOnly|QIODevice::Text);
	QTextStream istream(&infile);
	Graph *g = Graph::readGraph(&istream);


	QGraphicsScene *s = new QGraphicsScene();

	//get a copy to use a bunch of times
	QMap<uint,Vertex*> vertices = g->vertices();
	for(QMap<uint,Vertex*>::const_iterator i = vertices.constBegin();
	    i != vertices.constEnd(); ++i )
	{
		s->addItem(*i);
	}

	//get a copy to use a bunch of times
	QMap<QPair<Vertex*, Vertex*>, Edge*> edges = g->edges();
	for(QMap<QPair<Vertex*, Vertex*>, Edge*>::const_iterator i =
	    edges.constBegin(); i != edges.constEnd(); ++i )
	{
		s->addItem(*i);
	}

	QGraphicsView *view = new QGraphicsView(s);
	view->resize(700, 900);
	view->setRenderHint(QPainter::Antialiasing);
	view->show();
	//view->scale(5,5);


	//g->layoutNGon();
	g->layoutKamadaKawai(-1,0.0001,true);

	view->fitInView( view->scene()->sceneRect(),Qt::KeepAspectRatio);


	QFile outfile(args.at(2));
	outfile.open(QIODevice::WriteOnly|QIODevice::Text);
	QTextStream ostream(&outfile);
	Graph::writeGraph(&ostream, g);

	return app.exec();
}

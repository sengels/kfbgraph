//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

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
			// Delete the item at i and advance to the next item
			i = m_edges.erase( i );
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













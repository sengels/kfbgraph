project(kfbgraph)
find_package(Qt4 REQUIRED)

set( kfbgraph_SRCS main.cpp Edge.cpp Vertex.cpp Graph.cpp )

include_directories( ${QT_INCLUDES} )
add_executable( kfbgraph ${kfbgraph_SRCS} )

target_link_libraries( kfbgraph ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

install(TARGETS kfbgraph RUNTIME DESTINATION bin )

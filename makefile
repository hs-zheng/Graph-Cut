bin/graph-cut: src/main.cpp src/Graph-cut.h src/Graph-cut.cpp
	g++ src/Graph-cut.h src/Graph-cut.cpp -o ${@} $< `pkg-config opencv --cflags --libs`
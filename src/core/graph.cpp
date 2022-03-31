#include "pch.h"
#include "graph.h"
#include "error.h"
#include "word.h"

Graph* inputGraph;

void buildGraph(Graph** graph, Graph** noLoopGraph, char* words[], int cnt) {
	Graph* graph1, * graph2;
	graph1 = new Graph(SET_SIZE);
	graph2 = new Graph(SET_SIZE);
	for (int i = 1; i <= cnt; i++) {
		Word* wd = new Word(words[i], strlen(words[i]));
		int s = wd->getBegin(), t = wd->getEnd(), len = wd->getLength();
		graph1->link(s, t, len, wd);
		if (s != t) {
			graph2->link(s, t, len, wd);
		}
		else {
			graph2->addPointWeight(s, len, wd);
		}
	}
	*graph = graph1;
	*noLoopGraph = graph2;
}

int topoSort(Graph* graph, int* sort) {
	int* tmp = graph->getInDegree(), *first = graph->getFirst();
	int degree[SET_SIZE];
	//printf("check %d\n", graph);
	int head = 0, tail = 0, cnt = graph->getPointCount();
	for (int i = 0; i < cnt; ++i) {
		degree[i] = tmp[i];
	}
	for (int i = 0; i < cnt; ++i) {
		if (!degree[i]) {
			sort[tail++] = i;
			//cout << i << endl;
		}
	}
	while (head < tail) {
		int pt = sort[head++];
		for (int e = first[pt]; e; e = graph->getNext(e)) {
			int to = graph->getEdgeEnd(e);
			degree[to]--;
			//cout << to << " " << degree[to] << endl;
			if (!degree[to]) {
				sort[tail++] = to;
			}
		}
	}

	int tot = graph->getPointCount();
	/*
	for (int i = 0; i < head; ++i)
		cout << sort[i] << " ";
	cout << endl;
	cout << head << " " << tail << " " << tot << endl;
	*/
	if (tail != tot) {
		return -HAVE_LOOP;
	}

	return 0;
}

// find the scc of the graph
int sccStack[MAXN_POINT], top = 0;
bool sccVisit[MAXN_POINT], sccInStack[MAXN_POINT];
int sccLow[MAXN_POINT], sccDfn[MAXN_POINT], dfsNum = 0, blockNum = 0;
int sccColor[MAXN_POINT];

void tarjan(Graph* graph, int x) {
	//cout << "DFS: " << x << endl;
	sccLow[x] = sccDfn[x] = ++dfsNum;
	sccStack[++top] = x;
	sccVisit[x] = sccInStack[x] = true;
	for (int e = graph->getFirst()[x]; e; e = graph->getNext(e)) {
		int to = graph->getEdgeEnd(e);
		if (!sccVisit[to]) {
			tarjan(graph, to);
			sccLow[x] = min(sccLow[x], sccLow[to]);
		}
		else if (sccInStack[to]) {
			sccLow[x] = min(sccLow[x], sccDfn[to]);
		}
	}
	int topPoint;
	if (sccLow[x] == sccDfn[x]) {
		do {
			topPoint = sccStack[top--];
			sccInStack[topPoint] = false;
			sccColor[topPoint] = blockNum;
			//cout << "Block : " << blockNum << " " << topPoint << " " << sccLow[topPoint] << " " << sccDfn[topPoint] << endl;
		} while (sccLow[topPoint] != sccDfn[topPoint]);
		blockNum++;
	}
}


void getNoLoopGraph(Graph* noSelfLoopGraph, Graph** noLoopGraph, Graph* subGraph, int* subGraphCnt, int* pointColor){
	//printf("No Loop Graph is building!\n");
	int originPointCnt = noSelfLoopGraph->getPointCount();
	int size = (originPointCnt + 1) << 2;
	memset(sccVisit, 0, size);
	dfsNum = blockNum = 0;
	for (int i = 0; i < originPointCnt; i++) {
		if (!sccVisit[i]) {
			top = 0;
			tarjan(noSelfLoopGraph, i);
		}
	}
	//cout << originPointCnt << endl;
	for (int i = 0; i < originPointCnt; i++) {
		pointColor[i] = sccColor[i];
	}
	for (int i = 0; i < blockNum; i++) {
		subGraph[i] = Graph(originPointCnt);
	}
	Graph* crossGraph = new Graph(blockNum);
	for (int i = 0; i < originPointCnt; i++) {
		for (int e = noSelfLoopGraph->getFirst()[i]; e; e = noSelfLoopGraph->getNext(e)) {
			int to = noSelfLoopGraph->getEdgeEnd(e);
			if (sccColor[i] != sccColor[to]) {
				//cout << "other link " << sccColor[i] << " " << sccColor[to] << endl;
				crossGraph->link(sccColor[i], sccColor[to], noSelfLoopGraph->getEdgeValue(e), noSelfLoopGraph->getEdgeWord(e));
			}
			else {
				//cout << "sub link " << sccColor[i] << " " << i << " " << to << endl;
				subGraph[sccColor[i]].link(i, to, noSelfLoopGraph->getEdgeValue(e), noSelfLoopGraph->getEdgeWord(e));
			}
		}
		//subGraph[sccColor[i]].setPointWeights(i, noSelfLoopGraph->getPointWeight(i), noSelfLoopGraph->getPointCharWeight(i));
	}
	for (int i = 0; i < SET_SIZE; i++) {
		int* selfFirst = noSelfLoopGraph->getSelfEdgeFirst();
		for (int e = selfFirst[i]; e; e = noSelfLoopGraph->getNext(e)) {
			int s = noSelfLoopGraph->getEdgeStart(e);
			int len = noSelfLoopGraph->getEdgeValue(e);
			Word* word = noSelfLoopGraph->getEdgeWord(e);
			subGraph[sccColor[i]].addPointWeight(s, len, word);
		}
	}
	(* noLoopGraph) = crossGraph;
	(*subGraphCnt) = blockNum;
}

void getNewNoSelfLoopGraph(Graph* noSelfLoopGraph, Graph** newNoSelfLoopGraph) {

	int point_count = noSelfLoopGraph->getPointCount();
	Graph* crossGraph = new Graph(point_count);

	int ind[MAXN_POINT], oud[MAXN_POINT];

	for (int i = 0; i < SET_SIZE; i++) {
		int* first = noSelfLoopGraph->getFirst();
		for (int e = first[i]; e; e = noSelfLoopGraph->getNext(e)) {
			int s = noSelfLoopGraph->getEdgeStart(e);
			int t = noSelfLoopGraph->getEdgeEnd(e);
			ind[t]++;
			oud[s]++;
		}
	}

	// for a none-self-loop-edge, we delete it when in[s] == 0 && oud[t] == 0 && s_weight == 0 && t_weight == 0
	for (int i = 0; i < SET_SIZE; i++) {
		int* first = noSelfLoopGraph->getFirst();
		for (int e = first[i]; e; e = noSelfLoopGraph->getNext(e)) {
			int s = noSelfLoopGraph->getEdgeStart(e);
			int s_weight = noSelfLoopGraph->getPointWeight(s);
			int t = noSelfLoopGraph->getEdgeEnd(e);
			int t_weight = noSelfLoopGraph->getPointWeight(t);
			if (ind[s] == 0 && oud[t] == 0 && s_weight == 0 && t_weight == 0)
				continue;
			int len = noSelfLoopGraph->getEdgeValue(e);
			Word* word = noSelfLoopGraph->getEdgeWord(e);
			crossGraph->link(s, t, len, word);
		}
	}

	// for a self-loop-edge, we delete it when ind[x] == 0 && oud[x] == 0 && point_weigh[x] == 1
	for (int i = 0; i < SET_SIZE; i++) {
		int* selfFirst = noSelfLoopGraph->getSelfEdgeFirst();
		for (int e = selfFirst[i]; e; e = noSelfLoopGraph->getNext(e)) {
			int s = noSelfLoopGraph->getEdgeStart(e);
			int s_weight = noSelfLoopGraph->getPointWeight(s);
			if (ind[s] == 0 && oud[s] == 0 && s_weight == 1)
				continue;
			int len = noSelfLoopGraph->getEdgeValue(e);
			Word* word = noSelfLoopGraph->getEdgeWord(e);
			crossGraph->addPointWeight(s, len, word);
		}
	}


	(*newNoSelfLoopGraph) = crossGraph;
}
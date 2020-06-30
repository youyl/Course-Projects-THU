#include "../tt.h"

Graph::~Graph() {
	for(Opt* opt: opts) {
		delete opt;//析构所有的结点
	}
}

void Graph::clean() {//清空标记，本质是将时间戳+1
	time_cnt += 1;
}


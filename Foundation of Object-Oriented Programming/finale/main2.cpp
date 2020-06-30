//与一阶段的main.cpp完全一致，具备完成二阶段功能的能力
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include "tt.h"

using namespace std;

int main() {
	Session sess;
	int n, m, q;
	string input;

	cin >> n;
	getline(cin, input);
	while(n --) {
		getline(cin, input);
		sess.declare(input);
	}

	
	cin >> m;
	getline(cin, input);
	while(m --) {
		getline(cin, input);
		sess.code(input);
	}

	
	cin >> q;
	getline(cin, input);
	while(q --) {
		getline(cin, input);
		sess.run(input);
	}
	
	return 0;
}


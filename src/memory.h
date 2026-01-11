#pragma once
#include <unordered_map>

using namespace std;

class Memory {
public:
	Memory();
	~Memory();

	void store(int addr, int value);
	int load(int addr);

private:
	unordered_map<int, int> mem;
};
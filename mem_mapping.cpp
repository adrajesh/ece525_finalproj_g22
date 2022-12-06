#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>

using namespace std;

extern bool NormalMode;

// Parse for trace file.
void parse_line(std::string access,unsigned int arr[]) {
	// What we want to parse
	int instr;
	unsigned int address;
	// Parse from the string we read from the file
	sscanf(access.c_str(), "%d %lx", &instr, &address);
	arr[0]=instr;
	arr[1]=address;
}

// For set bits - Shift byte offset bits times and then AND with mask 
int get_set(unsigned int address, int offset_b, int set_mask) {
	auto shifted_address = address >> offset_b;
	shifted_address = shifted_address & set_mask;
	if(NormalMode) {
		cout<<"Set:"<<shifted_address<<endl;
	}
	return shifted_address;
}

// For tag bits - Shift (index + byte offset) times
int get_tag(unsigned int address, int offset_b, int index_b) {
	auto shifted_address = address >> (offset_b + index_b);
	if(NormalMode) {
		cout<<"Tag:"<<shifted_address<<endl;
	}
	return shifted_address;
}

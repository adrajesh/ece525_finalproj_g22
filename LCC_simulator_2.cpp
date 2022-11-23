#include <bits/stdc++.h>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

int offset_b;
int index_b;
unsigned int set_mask;

// Parse for trace file.
std::tuple<int,unsigned int> parse_line(std::string access,unsigned int arr[]) {
	// What we want to parse
	int instr;
	unsigned int address;
	// Parse from the string we read from the file
	sscanf(access.c_str(), "%d %lx", &instr, &address);
	//	std::cout << instr << " 0x" << std::hex << address << std::endl;
	arr[0]=instr;
	arr[1]=address;
}

// For set bits - Shift byte offset bits times and then AND with mask 
int get_set(unsigned int address) {
	auto shifted_address = address >> offset_b;
	shifted_address = shifted_address & set_mask;
	cout<<"Set:"<<shifted_address<<endl;
	return shifted_address;
}

// For tag bits - Shift index + byte offset times
int get_tag(unsigned int address) {
	auto shifted_address = address >> (offset_b + index_b);
	cout<<"Tag:"<<shifted_address<<endl;
	return shifted_address;
}

// Main Program 
int main() {
	std::string line;
	std::ifstream infile;
	int instr;
	unsigned int address;
	unsigned int trace[2];
	int size;
	int linesize;
	int way;
	unsigned set_offset;
	unsigned tag_offset;
	infile.open ("trace.log");
	
	cout<<"Enter the size of cache in MiB: ";
    cin>>size;
    cout<<"Enter the cache line size in Bytes: ";
    cin>>linesize;
    cout<<"Enter number of associative ways: ";
    cin>>way;
	
	offset_b = log2(linesize);
	index_b = log2((size * pow(2,20))/(pow(2,log2(linesize)) * pow(2,log2(way))));
	
	cout<<"Number of Offset bits: "<<offset_b<<endl;
	cout<<"Number of Index bits: "<<index_b<<endl;
	
	set_mask = pow(2,index_b) - 1;	
	
	while (std::getline(infile, line)) {
	parse_line(line,trace);	
	instr = trace[0];
	address = trace[1];
//	std::cout << instr << " 0x" << std::hex <<address << std::endl;
	if(instr==0) {
		cout<<"Operation - Read request from L1 data cache for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==1) {
		cout<<"Operation - Write request from L1 data cache for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);		
		}
	else if(instr==2) {
		cout<<"Operation - Read request from L1 instruction cache for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
    else if(instr==3) {
		cout<<"Operation - snooped invalidate command for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
    else if(instr==4) {
		cout<<"Operation - snooped read request for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==5) {
		cout<<"Operation - snooped write request for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==6) {
		cout<<"Operation - snooped read with intent to modify request for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==7) {
		cout<<"Operation - No operation "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==8) {
		cout<<"Operation - clear the cache and reset all state for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	else if(instr==9) {
		cout<<"Operation - print contents and state of each valid cache line for address "<< std::hex << address <<endl;
		get_set(address);
		get_tag(address);
		}
	}
	infile.close();
	return 0;
}
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
#include "updateLRU.cpp"
#include "getLRU.cpp"
using namespace std;

#define temp_ways 8
#define temp_sets 16384

int offset_b;
int index_b;
unsigned int set_mask;
bool NormalMode;
bool SilentMode;
bool File;

enum mesi_st {I,E,S,M}; // I-0; E-1; S-2; M-3;
	
struct lines {
	int MESI;												// 2 bits for MESI 
	unsigned int TAG;										// tag bits
} ;
	
struct cset {
	int PLRU;												// 7 bits for PLRU.
	lines LINE[temp_ways];									// hard coded, need to check for a way not to hard code.
} ;															// hard coded, need to check for a way not to hard code.

int checkWay(cset cache) {
	int empty_way;
	for(int i=0;i<temp_ways;i++) {
		if (cache.LINE[i].MESI == I) {
			empty_way = i;
			break;
		}
	}
	if (empty_way>7) {
		empty_way = 8;
		cout<<"All ways are full!"<<endl;
	}
	cout<<"Empty way - "<<empty_way<<endl;
	return empty_way;
}

int waysFilled(cset cache) {
int ways_filled = 0;
	for (int i=0;i<temp_ways;i++) {
		if (cache.LINE[i].MESI != I) {
			ways_filled++;
		}
	}
cout<< "Ways filled - " <<ways_filled<<endl;	
return ways_filled;
}

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
int get_set(unsigned int address) {
	auto shifted_address = address >> offset_b;
	shifted_address = shifted_address & set_mask;
	if(NormalMode) {
		cout<<"Set:"<<shifted_address<<endl;
	}
	return shifted_address;
}

// For tag bits - Shift (index + byte offset) times
int get_tag(unsigned int address) {
	auto shifted_address = address >> (offset_b + index_b);
	if(NormalMode) {
		cout<<"Tag:"<<shifted_address<<endl;
	}
	return shifted_address;
}

// Main Program 
int main(int argc, char* argv[]) {
	std::string line;
	std::ifstream infile;
	int instr;
	unsigned int address;
	unsigned int trace[2];
	int size;
	int linesize;
	int ways;
	int sets;
//	infile.open ("trace.log");
	int temp_SET;
	int temp_TAG;
	cset CACHE[temp_sets];
	
	cout<<endl<<"WELCOME TO LAST LEVEL CACHE"<<endl<<endl;
	if(argc==1) {											// No Arguments provided, NormalMode (default) and Trace file is trace.log	
		NormalMode = 1;
//        if(strcmp(argv[1],"trace.log")==0) {	
		infile.open("trace.log");
		if(infile){
			cout<<"File exists"<<endl;
			File = 1;
		}
		else{
			cout<<"File - 'trace.log' does not exists"<<endl;
			File = 0;
		}
		}
	else if(argc==2) {										// 1 Argument provided, either mode or file
		if(strcmp(argv[1],"silent")==0) {					// If silent given, Trace file is trace.log
			SilentMode = 1;									// If file name given, read that file
			infile.open("trace.log");
			if(infile){
				cout<<"File exists"<<endl;
				File = 1;
			}
			else{
				cout<<"File - 'trace.log' does not exists"<<endl;
				File = 0;
			}
		}
		if(strcmp(argv[1],"silent")!=0) {
			NormalMode = 1; 
			infile.open(argv[1]);
			if(infile){
				cout<<"File exists"<<endl;
				File = 1;
			}
			else{
				cout<<"File - "<< argv[1] <<" does not exists"<<endl;
				File = 0;
			}
		}
	}
	else if(argc == 3) {									// 2 Arguments provided
		if(strcmp(argv[2],"silent")==0) {					// set silentmode and read particular trace file
			SilentMode = 1;
			infile.open(argv[1]);
			if(infile){
				cout<<"File exists"<<endl;
				File = 1;
			}
			else{
				cout<<"File - "<< argv[1] <<" does not exists"<<endl;
				File = 0;
			}
		}
	}
	
	if(File) {
	cout<<"Enter the size of cache in MiB: ";
    cin>>size;
    cout<<"Enter the cache line size in Bytes: ";
    cin>>linesize;
    cout<<"Enter number of associative ways: ";
    cin>>ways;
	
	offset_b = log2(linesize);
	sets = (size * pow(2,20))/(linesize * ways);
	index_b = log2(sets);
	
	if(NormalMode) {
	cout<<"Number of Offset bits: "<<offset_b<<endl;
	cout<<"Number of Index bits: "<<index_b<<endl;
	}
			
	set_mask = pow(2,index_b) - 1;	

// 	Initializing Cache at start of the run - setting all MESI bits to invalid
	cout<<"Initializing the Cache!"<<endl;
	for(int i=0;i<temp_sets;i=i+1){
		for(int j=0;j<temp_ways;j=j+1){
			CACHE[i].LINE[j].MESI=I;
		}
	}
/*	cout<<"Done initializing the Cache!!"<<endl;
	
	cout<<"Checking 1 -"<<CACHE[0].LINE[0].MESI<<endl;
	
	if (CACHE[0].LINE[0].MESI == I) {
		cout<<"Checking 2"<<endl;
	} */
	
	int curr_way;
	int ways_filled;
	
	while (std::getline(infile, line)) {
	parse_line(line,trace);	
	instr = trace[0];
	address = trace[1];
//	std::cout << instr << " 0x" << std::hex <<address << std::endl;
	if(instr==0) {
		cout<<endl;
		cout<<"Operation - Read request from L1 data cache for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		
		ways_filled = waysFilled(CACHE[temp_SET]);
		
		if (ways_filled == 0) {											// Nothing filled. Compulsory Miss
			curr_way = checkWay(CACHE[temp_SET]);
			if (CACHE[temp_SET].LINE[curr_way].MESI == I) {				// Just Verify whether invalid
				if(NormalMode){
					cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
				}
			CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
			CACHE[temp_SET].LINE[curr_way].MESI = E;
			CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way);
			}
		}
		
		else if (ways_filled>0 && ways_filled<8) {						// Partially filled. Check both hit or miss.
			int hit = 0;
			for(int i=0;i<temp_ways;i++) {
				if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {			// Hit
					if(NormalMode){
						cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i);
						break;
					}
				}
			}
			if(hit == 0){												// In case of no hit
				curr_way = checkWay(CACHE[temp_SET]);
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {			// Just Verify whether invalid				
					if(NormalMode){
						cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].LINE[curr_way].MESI = E;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way);
				}
			}				
		}
		else if (ways_filled == 8) {													// All filled. Either hit or miss. For miss - eviction needed
			int hit = 0;
			for(int i=0;i<temp_ways;i++) {
				if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {			// Hit
					if(NormalMode){
						cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i);
						break;
					}
				}
			}
			if(hit == 0){																// Miss, Evict and Replace
				curr_way = getPLRU(~CACHE[temp_SET].PLRU);
				if(NormalMode){
					cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Exclusive"<<endl;
				}
				CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
				CACHE[temp_SET].LINE[curr_way].MESI = E;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way);
			}
		}
	}
	else if(instr==1) {
		cout<<"Operation - Write request from L1 data cache for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);		
		}
	else if(instr==2) {
		cout<<"Operation - Read request from L1 instruction cache for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
    else if(instr==3) {
		cout<<"Operation - snooped invalidate command for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
    else if(instr==4) {
		cout<<"Operation - snooped read request for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
	else if(instr==5) {
		cout<<"Operation - snooped write request for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
	else if(instr==6) {
		cout<<"Operation - snooped read with intent to modify request for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
	else if(instr==7) {
		cout<<"Operation - No operation "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
	else if(instr==8) {
		cout<<"Operation - clear the cache and reset all state for address "<< std::hex << address <<endl;
		temp_SET = get_set(address);
		temp_TAG = get_tag(address);
		}
	else if(instr==9) {
		cout<<"Operation - print contents and state of each valid cache line"<<endl;
		for(int i=0;i<temp_sets;i=i+1){
			for(int j=0;j<temp_ways;j=j+1){
				if (CACHE[i].LINE[j].MESI!=I) {
					if(NormalMode || SilentMode) {
						cout<<"Contents of cache at Index "<< i << " way " << j << " is "<<CACHE[i].LINE[j].TAG <<endl;
					}
				}
			}
		}
	}
	}
	infile.close();
	}
	return 0;
}

#include <bits/stdc++.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include<string.h>
#include <cctype>
#include<cstring>
#include "update_PLRU.cpp"
#include "get_PLRU.cpp"
#include "GetSnoopResult.cpp"
#include "MessageToCache.cpp"
#include "BusOperation.cpp"
#include "mem_mapping.cpp"
#include "PutSnoopResult.cpp"
//#include "ways.cpp"
using namespace std;

/* Bus Operation types */
#define READ 1 /* Bus Read */
#define WRITE 2 /* Bus Write */
#define INVALIDATE 3 /* Bus Invalidate */
#define RWIM 4 /* Bus Read With Intent to Modify */

/* Snoop Result types */
#define NOHIT 0 /* No hit */
#define HIT 1 /* Hit */
#define HITM 2 /* Hit to modified line */

/* L2 to L1 message types */
#define GETLINE 1 /* Request data for modified line in L1 */
#define SENDLINE 2 /* Send requested cache line to L1 */
#define INVALIDATELINE 3 /* Invalidate a line in L1 */
#define EVICTLINE 4 /* Evict a line from L1 */
// this is when L2's replacement policy causes eviction of a line that
// may be present in L1. It could be done by a combination of GETLINE
// (if the line is potentially modified in L1) and INVALIDATELINE.

int offset_b;
int index_b;
unsigned int set_mask;
bool NormalMode;
bool SilentMode;
bool File;
int SnoopResult;
int ways;
int sets;

enum mesi_st {I,E,S,M}; // I-0; E-1; S-2; M-3;
	
struct lines {
	int MESI  ;						// 2 bits for MESI 
	unsigned int TAG;				// tag bits
} ;
	
struct cset {
	int PLRU;																	// 7 bits for PLRU.
	struct lines *LINE = (lines*)malloc(sizeof(struct lines) * ways);			// Lines for n-way
} ;															

int checkWay(cset cache) {
	int empty_way;
	for(int i=0;i<ways;i++) {
		if (cache.LINE[i].MESI == I) {
			empty_way = i;
			break;
		}
	}
	if (empty_way>7) {
		empty_way = 8;
		cout<<"All ways are full!"<<endl;
	}
	if(NormalMode) {
		cout<<"Empty way - "<<empty_way<<endl;
	}
	return empty_way;
}

int waysFilled(cset cache) {
	int ways_filled = 0;
	for (int i=0;i<ways;i++) {
		if (cache.LINE[i].MESI != I) {
			ways_filled++;
		}
	}
	if(NormalMode) {
		cout<< "Ways filled - " <<ways_filled<<endl;	
	}
	return ways_filled;
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
	int temp_SET;
	int temp_TAG;
	
	cout<<endl<<"WELCOME TO LAST LEVEL CACHE"<<endl<<endl;
	if(argc==1) {
		cout<<"NORMAL MODE"<<endl;											// No Arguments provided, NormalMode (default) and Trace file is trace.log	
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
	else if(argc==2) {	
												// 1 Argument provided, either mode or file
		if(strcmp(argv[1],"silent")==0) {	
			cout<<"SILENT MODE"<<endl;				// If silent given, Trace file is trace.log
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
			cout<<"NORMAL MODE"<<endl;
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
		if(strcmp(argv[2],"silent")==0) {
			cout<<"SILENT MODE"<<endl;					// set silentmode and read particular trace file
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
	
	cset CACHE[sets];
//	struct cset *CACHE = (cset*)malloc(sizeof(struct cset) * sets);			// Dynamic allocation of CACHE size 
	
	if(NormalMode) {
	cout<<"Number of Offset bits: "<<offset_b<<endl;
	cout<<"Number of Index bits: "<<index_b<<endl;
	}
			
	set_mask = pow(2,index_b) - 1;	

// 	Initializing Cache at start of the run - setting all MESI bits to invalid
	cout<<"Initializing the Cache!"<<endl;
	for(int i=0;i<sets;i=i+1){
		for(int j=0;j<ways;j=j+1){
			CACHE[i].LINE[j].MESI=I;
		}
	}
	cout<<"Done initializing the Cache!!"<<endl;
	
	int curr_way;
	int ways_filled;
	int sr;
	
	while (std::getline(infile, line)) {
	parse_line(line,trace);	
	instr = trace[0];
	address = trace[1];
//	std::cout << instr << " 0x" << std::hex <<address << std::endl;
	if(instr==0) {
		if (NormalMode){
			cout<<endl<<"Operation - Read request from L1 data cache for address "<< std::hex << address <<endl;
		}
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
		
		ways_filled = waysFilled(CACHE[temp_SET]);
		sr= GetSnoopResult(address);
		if (ways_filled == 0) {											
			curr_way = checkWay(CACHE[temp_SET]);
			//cout<<"Present State: "<<CACHE[temp_SET].LINE[curr_way].MESI<<endl;
			if (CACHE[temp_SET].LINE[curr_way].MESI == I) {			// Just Verify whether invalid
				if(sr==0){
					if(NormalMode){
						cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = E;
				}

				else if(sr==1){
					if(NormalMode){
						cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = S;
				}

				else if(sr==2){
					if(NormalMode){
						cout<< "Miss. Flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
						BusOperation(WRITE, address, &SnoopResult);
						cout<<"Flush"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = S;
				}
				if (NormalMode){
						BusOperation(READ, address, &SnoopResult);
						MessageToCache(SENDLINE, address);
					}
				CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
			}
		}
		
		else if (ways_filled>0 && ways_filled<8) {						// Partially filled. Check both hit or miss.
			int hit = 0;
			for(int i=0;i<ways;i++) {
				if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {			// Hit
					if(NormalMode){
						cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
					}
					hit = 1;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
					break;
				}
			}
			if(hit == 0){																		// In case of no hit										
				curr_way = checkWay(CACHE[temp_SET]);
				//cout<<"Present State: "<<CACHE[temp_SET].LINE[curr_way].MESI<<endl;
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {									// Just Verify whether invalid
					if(sr==0){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = E;
					}

					else if(sr==1){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}

					else if(sr==2){
						if(NormalMode){
							cout<< "Miss. Flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
							BusOperation(WRITE, address, &SnoopResult);
							cout<<"Flush"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}
					if (NormalMode){
						BusOperation(READ, address, &SnoopResult);
						MessageToCache(SENDLINE, address);
					}
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				}
			}				
		}
		else if (ways_filled == 8) {													// All filled. Either hit or miss. For miss - eviction needed
			int hit = 0;
			for(int i=0;i<ways;i++) {
				if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {			// Hit
					if(NormalMode){
						cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
					}
					hit = 1;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
					break;
				}
			}
			if(hit == 0){																// Miss, Evict and Replace
				curr_way = getPLRU(~CACHE[temp_SET].PLRU, NormalMode);
				//cout<<"Present State: "<<CACHE[temp_SET].LINE[curr_way].MESI<<endl;
				if(sr==0){
					if(NormalMode){
						cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Exclusive"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = E;
				}

				else if(sr==1){
					if(NormalMode){
						cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Shared"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = S;
				}

				else if(sr==2){
					if(NormalMode){
						cout<< "Evicting way - "<<curr_way<<". Flushing and Replacing data in cache and setting MESI bit to Shared"<<endl;
						BusOperation(WRITE, address, &SnoopResult);
						cout<<"Flush"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = S;
				}
				if (NormalMode){
						BusOperation(READ, address, &SnoopResult);
						MessageToCache(EVICTLINE, address);
				}
				CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
			}
		}
	}
	else if(instr==1) {
		if (NormalMode){
			cout<<endl<<"Operation - Write request from L1 data cache for address "<< std::hex << address <<endl;
		}
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
		ways_filled = waysFilled(CACHE[temp_SET]);
		if (ways_filled == 0) {													// Empty cache line. Bring to cache.
			curr_way = checkWay(CACHE[temp_SET]);								// Check
			
			if (CACHE[temp_SET].LINE[curr_way].MESI == I) {						// Just Verify whether invalid
				if(NormalMode){
					cout<< "Miss. Writing data in cache and setting MESI bit to Modified"<<endl;			// Places in M irrespective of snoop result
				}
				CACHE[temp_SET].LINE[curr_way].MESI = M;
				CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
			}
			if (NormalMode){
				BusOperation(RWIM, address, &SnoopResult);
				MessageToCache(SENDLINE, address);
			}
		}
		else if (ways_filled>0 && ways_filled<8) {										// Partially filled. Check both hit or miss.
			int hit = 0;
			for(int i=0;i<ways;i++) {
				if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {				// Hit
					hit = 1;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
					if (CACHE[temp_SET].LINE[i].MESI == S){
						CACHE[temp_SET].LINE[i].MESI == M;
						if (NormalMode){
							BusOperation(INVALIDATE, address, &SnoopResult);
							MessageToCache(SENDLINE, address);	
						}
					}
					else if (CACHE[temp_SET].LINE[i].MESI == E){
						CACHE[temp_SET].LINE[i].MESI == M;
						if (NormalMode){
							MessageToCache(SENDLINE, address);	
						}	
					}
					break;
				}
			}
			if(hit == 0){																			// In case of no hit
				curr_way = checkWay(CACHE[temp_SET]);
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {			// Just Verify whether invalid
					if(NormalMode){
						cout<< "Miss. Writing data in cache and setting MESI bit to Modified"<<endl;			// Places in M irrespective of snoop result
					}
					CACHE[temp_SET].LINE[curr_way].MESI = M;
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				}
				if (NormalMode){
					BusOperation(RWIM, address, &SnoopResult);
					MessageToCache(SENDLINE, address);
				}
				
			}				
		}
		else if (ways_filled == 8) {																		// All filled. Either hit or miss. For miss - eviction needed
			int hit = 0;
			for(int i=0;i<ways;i++) {
			if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {				// Hit
				hit = 1;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
				if (CACHE[temp_SET].LINE[i].MESI == S){
					CACHE[temp_SET].LINE[i].MESI == M;
					if (NormalMode){
						BusOperation(INVALIDATE, address, &SnoopResult);
						MessageToCache(SENDLINE, address);
					}
				}
				else if (CACHE[temp_SET].LINE[i].MESI == E){
					CACHE[temp_SET].LINE[i].MESI == M;	
					if (NormalMode){
						MessageToCache(SENDLINE, address);
					}
				}
				break;
				}
			}
			if(hit == 0){																				// In case of no hit and cache full 
				curr_way = getPLRU(~CACHE[temp_SET].PLRU, NormalMode);										// Just Verify whether invalid
				if(NormalMode){
					cout<< "Evicting way - "<<curr_way<<". Writing data in cache and setting MESI bit to Modified"<<endl;	// Places in M irrespective of snoop result
				}
				CACHE[temp_SET].LINE[curr_way].MESI = M;
				CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
				CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				if (NormalMode){
					BusOperation(RWIM, address, &SnoopResult);
					MessageToCache(EVICTLINE, address);
				}
			}
		}
	}
	else if(instr==2) {
		if (NormalMode){
			cout<<endl<<"Operation - Read request from L1 instruction cache for address "<< std::hex << address <<endl;
		}
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
	}
    else if(instr==3) {
		if (NormalMode){
			cout<<endl<<"Operation - snooped invalidate command for address "<< std::hex << address <<endl;
		}
		curr_way=8;
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, set_mask, NormalMode);

		for(int i=0;i<ways;i++) {
			if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[curr_way].MESI != I){
				curr_way = i;
				break;
			}
		}
		
		if (curr_way != 8) {												// Hit
			if (CACHE[temp_SET].LINE[curr_way].MESI == S){
				CACHE[temp_SET].LINE[curr_way].MESI=I;
				if (NormalMode){
					cout<<"INVALID STATE"<<endl;
					PutSnoopResult(address,NOHIT);
					MessageToCache(INVALIDATELINE, address);
				}
			}
		}
		else if (curr_way == 8) {
			if (NormalMode){
				cout<<"INVALID STATE"<<endl;
				PutSnoopResult(address,NOHIT);
			}	
		}
		

	}
    else if(instr==4) {
		if (NormalMode){
			cout<<endl<<"Operation - snooped read request for address "<< std::hex << address <<endl;
		}
		curr_way=8;
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
		
		for(int i=0;i<ways;i++) {
			if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[curr_way].MESI != I){
				curr_way = i;
				break;
			}
		}
		
		if (curr_way != 8) {												// Hit
			if (CACHE[temp_SET].LINE[curr_way].MESI == M){
				CACHE[temp_SET].LINE[curr_way].MESI=S;
				if (NormalMode){
					MessageToCache(GETLINE, address);
					cout<<"Flush"<<endl;
					BusOperation(WRITE, address, &SnoopResult);										// Need to show Flush Bus Op
					cout<<"SHARED STATE"<<endl;									
					PutSnoopResult(address,HIT);
				}
			}

			else if (CACHE[temp_SET].LINE[curr_way].MESI == E){
				CACHE[temp_SET].LINE[curr_way].MESI=S;
				if (NormalMode){
					cout<<"SHARED STATE"<<endl;
					PutSnoopResult(address,HIT);	
				}
			}

			else if (CACHE[temp_SET].LINE[curr_way].MESI == S){
				CACHE[temp_SET].LINE[curr_way].MESI=S;
				if (NormalMode){
					cout<<"SHARED STATE"<<endl;
					PutSnoopResult(address,HIT);
				}
			}
		}
		else if (curr_way == 8) {	
			if (NormalMode){
				cout<<"INVALID STATE"<<endl;
				PutSnoopResult(address,NOHIT);		
			}	
		}
	}
	else if(instr==5) {	
		if (NormalMode){
			cout<<"Operation - snooped write request for address "<< std::hex << address <<endl;
		}
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
		if (NormalMode){
			PutSnoopResult(address,NOHIT);	
		}
	}
	else if(instr==6) {
		if (NormalMode){
			cout<<endl<<"Operation - snooped read with intent to modify request for address "<< std::hex << address <<endl;
		}
		curr_way=8;
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
		
		for(int i=0;i<ways;i++) {
			if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[curr_way].MESI != I){	
				curr_way = i;
				break;
			}
		}
		if (curr_way != 8) {												// Hit
			if(CACHE[temp_SET].LINE[curr_way].MESI == M) {
				CACHE[temp_SET].LINE[curr_way].MESI=I;
				if (NormalMode){
					MessageToCache(GETLINE, address);
					BusOperation(WRITE, address, &SnoopResult);
					cout<<"Flush"<<endl;										
					cout<<"INVALID STATE"<<endl;								
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
				}
				
			}
			else if(CACHE[temp_SET].LINE[curr_way].MESI == S) {
				CACHE[temp_SET].LINE[curr_way].MESI=I;
				if (NormalMode){										// Need to show Flush Bus Op ?
					cout<<"INVALID STATE"<<endl;								// we can remove this later
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
				}
			}
			else if(CACHE[temp_SET].LINE[curr_way].MESI == E) {
				CACHE[temp_SET].LINE[curr_way].MESI=I;
				if (NormalMode){										// Need to show Flush Bus Op ?
					cout<<"INVALID STATE"<<endl;								// we can remove this later
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
				}
			}
		}
		else if (curr_way == 8) {
			if (NormalMode){											// No Hit
				cout<<"INVALID STATE"<<endl;
				PutSnoopResult(address,NOHIT);
			}
		}
	}
	else if(instr==7) {
		if (NormalMode){
			cout<<endl<<"Operation - No operation "<< std::hex << address <<endl;
		}
		temp_SET = get_set(address, offset_b, set_mask, NormalMode);
		temp_TAG = get_tag(address, offset_b, index_b, NormalMode);
	}
	else if(instr==8) {
		if (NormalMode){
			cout<<endl<<"Operation - clear the cache and reset all state"<<endl;
		}
		cout<<"Clearing the Cache!"<<endl;
		for(int i=0;i<sets;i=i+1){
			for(int j=0;j<ways;j=j+1){
				CACHE[i].LINE[j].MESI=I;
			}
		}
	}
	else if(instr==9) {
		cout<<endl<<"Operation - print contents and state of each valid cache line"<<endl;
		for(int i=0;i<sets;i=i+1){
			for(int j=0;j<ways;j=j+1){
				if (CACHE[i].LINE[j].MESI!=I) {
					//if(NormalMode || SilentMode) {
						cout<<"Contents of cache(Tag) at Index "<<std::hex<< i << " way " << j << " is "<<std::hex<<CACHE[i].LINE[j].TAG <<" , "<<CACHE[i].LINE[j].MESI<<endl;
					//}
				}
			}
		}
	}
	}
	infile.close();
	}
	return 0;
}
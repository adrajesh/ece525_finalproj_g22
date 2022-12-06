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
//#include "getMesiName.cpp"
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
int cachemiss=0;
int cachehit=0;
int cacheread=0;
int cachewrite=0;
float hitratio;
char MESI_name;

enum mesi_st {I,E,S,M}; // I-0; E-1; S-2; M-3;
	
struct lines {
	int MESI  ;						// 2 bits for MESI 
	unsigned int TAG;				// tag bits
} ;
	
struct cset {
	int PLRU;																	// 7 bits for PLRU.
	struct lines *LINE = (lines*)malloc(sizeof(struct lines) * ways);			// Lines for n-way
} ;															

char getMesiName(int mesi) 
{
   switch (mesi) 
   {
      case 0: MESI_name= 'I'; break;
      case 1: MESI_name= 'E'; break;
      case 2: MESI_name= 'S'; break;
      case 3: MESI_name= 'M'; break;	  
   }
   return MESI_name;
}

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
		cout<<endl<<"Enter the size of cache in MiB: ";
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
			cout<<endl<<"Number of Offset bits: "<<offset_b<<endl;
			cout<<"Number of Index bits: "<<index_b<<endl;
		}
				
		set_mask = pow(2,index_b) - 1;	

	// 	Initializing Cache at start of the run - setting all MESI bits to invalid
		if(NormalMode){
			cout<<endl<<"Initializing the Cache!"<<endl;
		}
		for(int i=0;i<sets;i=i+1){
			for(int j=0;j<ways;j=j+1){
				CACHE[i].LINE[j].MESI=I;
			}
		}
		if(NormalMode){
			cout<<"Done initializing the Cache!!"<<endl;
		}
		
		int curr_way;
		int ways_filled;
		
		while (std::getline(infile, line)) {
		parse_line(line,trace);	
		instr = trace[0];
		address = trace[1];
	//	std::cout << instr << " 0x" << std::hex <<address << std::endl;
		if(instr==0) {
			if (NormalMode){
				cout<<endl<<"Operation - Read request from L1 data cache for address "<< std::hex << address <<endl;
			}
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			cacheread++;
			ways_filled = waysFilled(CACHE[temp_SET]);
			if (ways_filled == 0) {
				BusOperation(READ, address, &SnoopResult);					
				curr_way = checkWay(CACHE[temp_SET]);
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {			
					if(SnoopResult==0){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = E;
					}

					else if(SnoopResult==1){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}

					else if(SnoopResult==2){
						if(NormalMode){
							cout<< "Miss. Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}
					MessageToCache(SENDLINE, address);
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				}
				cachemiss++;
			}
			
			else if (ways_filled>0 && ways_filled<8) {					
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {
						cachehit++;			
						if(NormalMode){
							cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						}
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						break;
					}
				}
				if(hit == 0){																					
					curr_way = checkWay(CACHE[temp_SET]);
					BusOperation(READ, address, &SnoopResult);
					if (CACHE[temp_SET].LINE[curr_way].MESI == I) {	
						if(SnoopResult==0){
							if(NormalMode){
								cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = E;
						}

						else if(SnoopResult==1){
							if(NormalMode){
								cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = S;
						}

						else if(SnoopResult==2){
							if(NormalMode){
							cout<< "Miss. Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = S;
						}
						MessageToCache(SENDLINE, address);
						CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
						cachemiss++;
					}
				}				
			}
			else if (ways_filled == 8) {						
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {	
						cachehit++;		
						if(NormalMode){
							cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						}
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						break;
					}
				}
				if(hit == 0){																
					curr_way = getPLRU(~CACHE[temp_SET].PLRU, NormalMode);
					BusOperation(READ, address, &SnoopResult);
					if(SnoopResult==0){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Exclusive"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = E;
					}

					else if(SnoopResult==1){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}

					else if(SnoopResult==2){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}
					MessageToCache(EVICTLINE, address);
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
					cachemiss++;
				}
			}
			cout<<"Read: "<<cacheread<<", Miss: "<<cachemiss<<", Hit: "<<cachehit<<endl;
		}
		else if(instr==1) {
			if (NormalMode){
				cout<<endl<<"Operation - Write request from L1 data cache for address "<< std::hex << address <<endl;
			}
			cachewrite++;
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			ways_filled = waysFilled(CACHE[temp_SET]);
			if (ways_filled == 0) {													
				curr_way = checkWay(CACHE[temp_SET]);								
				BusOperation(RWIM, address, &SnoopResult);
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {						
					if(NormalMode){
						cout<< "Miss. Writing data in cache and setting MESI bit to Modified"<<endl;			
					}
					CACHE[temp_SET].LINE[curr_way].MESI = M;
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				}
				MessageToCache(SENDLINE, address);
				cachemiss++;
			}
			else if (ways_filled>0 && ways_filled<8) {														
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {	
						cachehit++;		
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						if (CACHE[temp_SET].LINE[i].MESI == S){
							CACHE[temp_SET].LINE[i].MESI = M;
							if(NormalMode){
								cout<<"WRITE HIT"<<endl;
							}
							BusOperation(INVALIDATE, address, &SnoopResult);		
							MessageToCache(SENDLINE, address);
						}
						else if (CACHE[temp_SET].LINE[i].MESI == E){
							CACHE[temp_SET].LINE[i].MESI = M;
							if(NormalMode){
								cout<<"WRITE HIT"<<endl;
							}
							MessageToCache(SENDLINE, address);	
						}
						break;
					}
				}
				if(hit == 0){													
					curr_way = checkWay(CACHE[temp_SET]);
					BusOperation(RWIM, address, &SnoopResult);
					if (CACHE[temp_SET].LINE[curr_way].MESI == I) {													
						if(NormalMode){
							cout<< "Miss. Writing data in cache and setting MESI bit to Modified"<<endl;		
						}
						CACHE[temp_SET].LINE[curr_way].MESI = M;
						CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
					}
					MessageToCache(SENDLINE, address);
					cachemiss++;
				}				
			}
			else if (ways_filled == 8) {													
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {	
						cachehit++;		
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						if (CACHE[temp_SET].LINE[i].MESI == S){
							CACHE[temp_SET].LINE[i].MESI = M;
							if(NormalMode){
								cout<<"WRITE HIT"<<endl;
							}
							BusOperation(INVALIDATE, address, &SnoopResult);
							MessageToCache(SENDLINE, address);
						}
						else if (CACHE[temp_SET].LINE[i].MESI == E){
							CACHE[temp_SET].LINE[i].MESI = M;
							if(NormalMode){
								cout<<"WRITE HIT"<<endl;
							}
							MessageToCache(SENDLINE, address);
						}
						break;
					}
				}
				if(hit == 0){																	 
					curr_way = getPLRU(~CACHE[temp_SET].PLRU, NormalMode);
					BusOperation(RWIM, address, &SnoopResult);							
					if(NormalMode){
						cout<< "Evicting way - "<<curr_way<<". Writing data in cache and setting MESI bit to Modified"<<endl;
					}
					CACHE[temp_SET].LINE[curr_way].MESI = M;
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
					MessageToCache(EVICTLINE, address);
					cachemiss++;
				}
			}
			cout<<"Write: "<<cacheread<<", Miss: "<<cachemiss<<", Hit: "<<cachehit<<endl;
		}
		else if(instr==2) {
			if (NormalMode){
				cout<<endl<<"Operation - Read request from L1 instruction cache for address "<< std::hex << address <<endl;
			}
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			cacheread++;
			ways_filled = waysFilled(CACHE[temp_SET]);
			if (ways_filled == 0) {
				BusOperation(READ, address, &SnoopResult);					
				curr_way = checkWay(CACHE[temp_SET]);
				if (CACHE[temp_SET].LINE[curr_way].MESI == I) {			
					if(SnoopResult==0){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = E;
					}

					else if(SnoopResult==1){
						if(NormalMode){
							cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}

					else if(SnoopResult==2){
						if(NormalMode){
							cout<< "Miss. Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}
					MessageToCache(SENDLINE, address);
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
				}
				cachemiss++;
			}
			
			else if (ways_filled>0 && ways_filled<8) {					
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {
						cachehit++;			
						if(NormalMode){
							cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						}
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						break;
					}
				}
				if(hit == 0){																					
					curr_way = checkWay(CACHE[temp_SET]);
					BusOperation(READ, address, &SnoopResult);
					if (CACHE[temp_SET].LINE[curr_way].MESI == I) {	
						if(SnoopResult==0){
							if(NormalMode){
								cout<< "Miss. Placing data in cache and setting MESI bit to Exclusive"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = E;
						}

						else if(SnoopResult==1){
							if(NormalMode){
								cout<< "Miss. Placing data in cache and setting MESI bit to Shared"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = S;
						}

						else if(SnoopResult==2){
							if(NormalMode){
							cout<< "Miss. Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
							}
							CACHE[temp_SET].LINE[curr_way].MESI = S;
						}
						MessageToCache(SENDLINE, address);
						CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
						cachemiss++;
					}
				}				
			}
			else if (ways_filled == 8) {						
				int hit = 0;
				for(int i=0;i<ways;i++) {
					if (CACHE[temp_SET].LINE[i].MESI != I && CACHE[temp_SET].LINE[i].TAG == temp_TAG) {	
						cachehit++;		
						if(NormalMode){
							cout<< "Hit!!! Tag is - "<< CACHE[temp_SET].LINE[i].TAG <<endl;
						}
						hit = 1;
						CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,i, NormalMode);
						break;
					}
				}
				if(hit == 0){																
					curr_way = getPLRU(~CACHE[temp_SET].PLRU, NormalMode);
					BusOperation(READ, address, &SnoopResult);
					if(SnoopResult==0){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Exclusive"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = E;
					}

					else if(SnoopResult==1){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Replacing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}

					else if(SnoopResult==2){
						if(NormalMode){
							cout<< "Evicting way - "<<curr_way<<". Other cache flushing and Placing data in cache and setting MESI bit to Shared"<<endl;
						}
						CACHE[temp_SET].LINE[curr_way].MESI = S;
					}
					MessageToCache(EVICTLINE, address);
					CACHE[temp_SET].LINE[curr_way].TAG = temp_TAG;
					CACHE[temp_SET].PLRU = updatePLRU(CACHE[temp_SET].PLRU,curr_way, NormalMode);
					cachemiss++;
				}
			}
			cout<<"Read: "<<cacheread<<", Miss: "<<cachemiss<<", Hit: "<<cachehit<<endl;
		}
		else if(instr==3) {
			if (NormalMode){
				cout<<endl<<"Operation - snooped invalidate command for address "<< std::hex << address <<endl;
			}
			curr_way=8;
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, set_mask);

			for(int i=0;i<ways;i++) {
				if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[curr_way].MESI != I){
					curr_way = i;
					break;
				}
			}
			
			if (curr_way != 8) {											
				if (CACHE[temp_SET].LINE[curr_way].MESI == S){
					CACHE[temp_SET].LINE[curr_way].MESI=I;
					if (NormalMode){
						cout<<"INVALID STATE"<<endl;
					}
					PutSnoopResult(address,NOHIT);
					MessageToCache(INVALIDATELINE, address);
				}
			}
			else if (curr_way == 8) {
				if (NormalMode){
					cout<<"INVALID STATE"<<endl;
				}
				PutSnoopResult(address,NOHIT);	
			}
			

		}
		else if(instr==4) {
			if (NormalMode){
				cout<<endl<<"Operation - snooped read request for address "<< std::hex << address <<endl;
			}
			curr_way=8;
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			
			for(int i=0;i<ways;i++) {
				if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[curr_way].MESI != I){
					curr_way = i;
					break;
				}
			}
			
			if (curr_way != 8) {										
				if (CACHE[temp_SET].LINE[curr_way].MESI == M){
					CACHE[temp_SET].LINE[curr_way].MESI=S;
					MessageToCache(GETLINE, address);
					BusOperation(WRITE, address, &SnoopResult);
					if (NormalMode){
						cout<<"Flush"<<endl;									
						cout<<"SHARED STATE"<<endl;	
					}
					PutSnoopResult(address,HIT);
				}

				else if (CACHE[temp_SET].LINE[curr_way].MESI == E){
					CACHE[temp_SET].LINE[curr_way].MESI=S;
					if (NormalMode){
						cout<<"SHARED STATE"<<endl;	
					}
					PutSnoopResult(address,HIT);
				}

				else if (CACHE[temp_SET].LINE[curr_way].MESI == S){
					CACHE[temp_SET].LINE[curr_way].MESI=S;
					if (NormalMode){
						cout<<"SHARED STATE"<<endl;
					}
					PutSnoopResult(address,HIT);
				}
			}
			else if (curr_way == 8) {	
				if (NormalMode){
					cout<<"INVALID STATE"<<endl;	
				}	
				PutSnoopResult(address,NOHIT);	
			}
		}
		else if(instr==5) {	
			if (NormalMode){
				cout<<endl<<"Operation - snooped write request for address "<< std::hex << address <<endl;
			}
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			//PutSnoopResult(address,NOHIT);	
		}
		else if(instr==6) {
			if (NormalMode){
				cout<<endl<<"Operation - snooped read with intent to modify request for address "<< std::hex << address <<endl;
			}
			curr_way=8;
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
			
			for(int i=0;i<ways;i++) {
				if(CACHE[temp_SET].LINE[i].TAG == temp_TAG && CACHE[temp_SET].LINE[i].MESI != I){	
					curr_way = i;
					break;
				}
			}
			if (curr_way != 8) {						
				if(CACHE[temp_SET].LINE[curr_way].MESI == M) {
					CACHE[temp_SET].LINE[curr_way].MESI=I;
					MessageToCache(GETLINE, address);
					BusOperation(WRITE, address, &SnoopResult);
					if (NormalMode){
						cout<<"Flush"<<endl;										
						cout<<"INVALID STATE"<<endl;	
					}
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
					
				}
				else if(CACHE[temp_SET].LINE[curr_way].MESI == S) {
					CACHE[temp_SET].LINE[curr_way].MESI=I;
					if (NormalMode){										
						cout<<"INVALID STATE"<<endl;					
					}
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
				}
				else if(CACHE[temp_SET].LINE[curr_way].MESI == E) {
					CACHE[temp_SET].LINE[curr_way].MESI=I;
					if (NormalMode){										
						cout<<"INVALID STATE"<<endl;					
					}
					MessageToCache(INVALIDATELINE, address);
					PutSnoopResult(address,NOHIT);
				}
			}
			else if (curr_way == 8) {
				if (NormalMode){									
					cout<<"INVALID STATE!"<<endl;
				}
				PutSnoopResult(address,NOHIT);
			}
		}
		else if(instr==7) {
			if (NormalMode){
				cout<<endl<<"Operation - No operation "<< std::hex << address <<endl;
			}
			temp_SET = get_set(address, offset_b, set_mask);
			temp_TAG = get_tag(address, offset_b, index_b);
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
			double s;
			if(NormalMode){
				cout<<endl<<"Operation - print contents and state of each valid cache line"<<endl;
			}
			for(int i=0;i<sets;i=i+1){
				for(int j=0;j<ways;j=j+1){
					if(CACHE[i].LINE[j].MESI==I){
						s++;
					}
				}
			}

			if (s==ways*sets){
				cout<<"-----------EMPTY CACHE----------"<<endl;
			}

			else{
				cout<<"---------------------------------------------------------------------------------"<<endl;
				for(int i=0;i<sets;i=i+1){
					for(int j=0;j<ways;j=j+1){
						if (CACHE[i].LINE[j].MESI!=I) {
							cout<<"Set: "<<std::hex<<i<<"   -   ";
							for (int k=0;k<ways;k=k+1) {
								if (CACHE[i].LINE[k].MESI!=I) {
									cout<<std::hex<<CACHE[i].LINE[k].TAG<<"("<<getMesiName(CACHE[i].LINE[k].MESI)<<")";
								}
								else if (CACHE[i].LINE[k].MESI==I) {
									cout<<"X"<<"("<<getMesiName(CACHE[i].LINE[k].MESI)<<")";
								}
							cout<<"\t";
							}
							cout<<endl;
							cout<<"---------------------------------------------------------------------------------"<<endl;
							cout<<endl;
							break;
						}
					}
				}
			}
		}
	}
	infile.close();
	}
	hitratio= (cachehit*100)/(cacheread + cachewrite);
	cout<<"---------------------------"<<endl;
	cout<<"| Cache Hit: \t|  "<<cachehit<<"\t  |"<<endl;
	cout<<"| Cache Miss: \t|  "<<cachemiss<<"\t  |"<<endl;
	cout<<"| Cache Read: \t|  "<<cacheread<<"\t  |"<<endl;
	cout<<"| Cache Write: \t|  "<<cachewrite<<"\t  |"<<endl;
	cout<<"| Hit Ratio: \t|  "<<hitratio<<" %"<<"\t  |"<<endl;
	cout<<"---------------------------"<<endl;
	return 0;
}

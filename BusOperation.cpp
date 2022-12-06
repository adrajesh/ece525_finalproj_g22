#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

extern bool NormalMode;

void BusOperation(int BusOp, unsigned int address, int *SnoopResult){
    *SnoopResult = GetSnoopResult(address);
	string temp_snoop;
	if (*SnoopResult == 0){
	temp_snoop = "NOHIT";
	}
	else if (*SnoopResult == 1){
	temp_snoop = "HIT";
	}
	else if (*SnoopResult == 2){
	temp_snoop = "HITM";
	}
	
	if(NormalMode){
		if (BusOp == 1) {
			cout<<"Bus Operation: READ"<<", Address: "<<address<<", SnoopResult: "<<temp_snoop<<endl;
		}
		else if (BusOp == 2) {
			cout<<"Bus Operation: WRITE"<<", Address: "<<address<<", SnoopResult: "<<temp_snoop<<endl;
		}
		else if (BusOp == 3) {
			cout<<"Bus Operation: INVALIDATE"<<", Address: "<<address<<", SnoopResult: "<<temp_snoop<<endl;
		}
		else if (BusOp == 4) {
			cout<<"Bus Operation: RWIM"<<", Address: "<<address<<", SnoopResult: "<<temp_snoop<<endl;
		}
	}
	
}

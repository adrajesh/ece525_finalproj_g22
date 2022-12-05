#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

#define NOHIT 0
#define HIT 1
#define HITM 2

/* Report the result of our snooping bus operations performed by other caches */
void PutSnoopResult(unsigned int Address, int SnoopResult) 
{
	if(SnoopResult == 0) {
	cout<<"SnoopResult: NOHIT"<< ", Address: " << Address<<endl;
	}
	if(SnoopResult == 1) {
	cout<<"SnoopResult: HIT"<< ", Address: " << Address<<endl;
	}
	if(SnoopResult == 2) {
	cout<<"SnoopResult: HITM"<< ", Address: " << Address<<endl;
	}
}
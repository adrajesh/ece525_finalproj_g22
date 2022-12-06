#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

extern bool NormalMode;

void MessageToCache(int Message, unsigned int address){
	if(NormalMode){
		if (Message == 1) {
			cout<<"L2 to L1 Message: GETLINE"<<", "<<address<<endl;
		}
		else if (Message == 2) {
			cout<<"L2 to L1 Message: SENDLINE"<<", "<<address<<endl;
		}
		else if (Message == 3) {
			cout<<"L2 to L1 Message: INVALIDATELINE"<<", "<<address<<endl;
		}
		else if (Message == 4) {
			cout<<"L2 to L1 Message: EVICTLINE"<<", "<<address<<endl;
		}
	}
	
}

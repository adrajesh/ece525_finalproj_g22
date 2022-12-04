#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

// #define SENDLINE 1
// #define GETLINE 2
// #define INVALIDATELINE 3
// #define EVICTLINE 4

void MessageToCache(int Message, unsigned int address){
	if (Message == 1) {
		cout<<"L2 to L1 Message: SENDLINE"<<", "<<address<<endl;
	}
	else if (Message == 2) {
		cout<<"L2 to L1 Message: GETLINE"<<", "<<address<<endl;
	}
	else if (Message == 3) {
		cout<<"L2 to L1 Message: INVALIDATELINE"<<", "<<address<<endl;
	}
	else if (Message == 4) {
		cout<<"L2 to L1 Message: EVICTLINE"<<", "<<address<<endl;
	}
}

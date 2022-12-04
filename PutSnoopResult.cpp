#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

/* Report the result of our snooping bus operations performed by other caches */
void PutSnoopResult(unsigned int Address, int SnoopResult) 
{
	cout<<"SnoopResult: "<< SnoopResult << ", Address: " << Address<<endl;
}
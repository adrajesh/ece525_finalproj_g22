#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;


void BusOperation(int BusOp, unsigned int address, int *SnoopResult){
    *SnoopResult = GetSnoopResult(address);       
    cout<<"Bus Operation: "<<BusOp<<", Address: "<<address<<", SnoopResult: "<<*SnoopResult<<endl;
}
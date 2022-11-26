#include <bits/stdc++.h>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include<string.h>
#include <cctype>
#include<cstring>
using namespace std;

int GetSnoopResult(int address){

    enum snoop {NOHIT, HIT, HITM};     
    if((address & 3)==0){              //address with LSB 00
        return HIT;
    }

    else if((address & 3)==2){         //address with LSB 10
        return HITM;
    }

    else{                              //address ending with 01 or 11
        return NOHIT;
    }
}

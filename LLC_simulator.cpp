#include<iostream>
#include<fstream>
#include<sstream>
#include<cstring>
#include<vector>
#include <algorithm>
using namespace std;

int main()
{
    ifstream file;
    file.open("trace.log");
    if(!file.is_open())
    {
        cout<<"Unable to open the file."<<endl;
    }

    string line[500];
    string lines;
    string oneline;
    //string array[1000];
    int i=0;
    int t=1;
    int s=1;

    while(!file.eof())
    {
        getline(file, lines, ' ');
        //cout<<"Line "<<s<<" is "<<lines<<endl;
        line[i]= lines;
        s++;

        getline(file, lines, '\n');
        //cout<<"Line "<<s<<" is "<<lines<<endl;
        line[i+1]= lines;
        s++;
        
        cout<<"Line "<<s/2<<" is "<<line[i]<<" "<<line[i+1]<<endl;

        if(line[i]=="0")
        {
            cout<<"Operation - Read request from L1 data cache for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="1")
        {
            cout<<"Operation - Write request from L1 data cache for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="2")
        {
            cout<<"Operation - Read request from L1 instruction cache for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="3")
        {
            cout<<"Operation - snooped invalidate command for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="4")
        {
            cout<<"Operation - snooped read request for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="5")
        {
            cout<<"Operation - snooped write request for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="6")
        {
            cout<<"Operation - snooped read with intent to modify request for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="7")
        {
            cout<<"Operation - No operation "<<line[i+1]<<endl;
        }

        else if(line[i]=="8")
        {
            cout<<"Operation - clear the cache and reset all state for address "<<line[i+1]<<endl;
        }

        else if(line[i]=="9")
        {
            cout<<"Operation - print contents and state of each valid cache line for address "<<line[i+1]<<endl;
        }

        cout<<endl;

    }

    file.close();

}
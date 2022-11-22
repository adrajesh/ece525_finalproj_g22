#include<iostream>
#include<string.h>
#include <cctype>
#include <iostream>
#include<cstring>
#include<vector>
#include<algorithm>
#include"hex2bin.cpp"
#include"mem_mapping.cpp"
using namespace std;

int main(int argc, char* argv[])
{
    cout<<endl<<"-------------------------------------START-----------------------------------------"<<endl;

    if(argc>2)
    {
        cout<<endl<<"WELCOME TO LAST LEVEL CACHE"<<endl<<endl;
        ifstream file;
        if(strcmp(argv[1],"trace.log")==0)                      //argument with file name
        {
            if(strcmp(argv[2],"silent")==0)                     //argument for silent mode
            {
                cout<<"This is SILENT MODE"<<endl<<endl;
                //mem_map();
                
            }

            else if(strcmp(argv[2],"normal")==0)                     //argument for normal mode
            {
                cout<<"This is NORMAL MODE"<<endl<<endl;
                //mem_map();
                file.open(argv[1]);                             //open file with name passed in argument

                if(!file.is_open())                             //if there is no such file
                {
                    cout<<"Unable to open the file."<<endl;
                }

                string line[500];
                string lines;
                string oneline;
                int i=0;
                int t=1;
                int s=1;

                cout<<endl;

                while(!file.eof())                              //open the file and read till end line of the file
                {
                    getline(file, lines, ' ');                  //dividing the each line where SPACE occurs 
                    //cout<<"Line "<<s<<" is "<<lines<<endl;
                    line[i]= lines;
                    s++;

                    getline(file, lines, '\n');                 //dividing the line where NEXT LINE occurs
                    //cout<<"Line "<<s<<" is "<<lines<<endl;
                    line[i+1]= lines;
                    s++;
                    
                    cout<<"Line "<<s/2<<" is n: "<<line[i]<<", address: "<<line[i+1]<<endl;

                    if(line[i]=="0")
                    {
                        cout<<"Operation - Read request from L1 data cache for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="1")
                    {
                        cout<<"Operation - Write request from L1 data cache for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="2")
                    {
                        cout<<"Operation - Read request from L1 instruction cache for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="3")
                    {
                        cout<<"Operation - snooped invalidate command for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="4")
                    {
                        cout<<"Operation - snooped read request for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="5")
                    {
                        cout<<"Operation - snooped write request for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="6")
                    {
                        cout<<"Operation - snooped read with intent to modify request for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="7")
                    {
                        cout<<"Operation - No operation "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="8")
                    {
                        cout<<"Operation - clear the cache and reset all state for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    else if(line[i]=="9")
                    {
                        cout<<"Operation - print contents and state of each valid cache line for address "<<line[i+1]<<endl;
                        hex2bin(line[i+1]);
                    }

                    cout<<endl<<endl;

                }
            }

            else
            {
                cout<<"Enter the MODE correctly"<<endl;
            }        

            file.close();    
        }

        else
        {
            cout<<argv[1]<<" file does not exist"<<endl;
        }
            
    }

    else if(argc==2 && strcmp(argv[1],"trace.log")==0)                 //if we do not enter mode in the argument
    {
        cout<<endl<<"Enter the MODE"<<endl;

    }

    else if(argc==2 && strcmp(argv[1],"trace.log")!=0)                //if file name is wrong in argument
    {
        cout<<endl<<"File does not exist"<<endl;

    }

    else                                                              //if we do not ener any file name
    {
        cout<<endl<<"Enter the file name in argument"<<endl<<endl;
    }

    cout<<endl<<"--------------------------------------END------------------------------------------"<<endl<<endl;

}
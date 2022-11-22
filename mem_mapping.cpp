#include <iostream>
using namespace std;

void mem_map() 
{
    int size;
    int linesize;
    int way;
    int set;
    int indexbits=0;
    int byteoffset=0;

    cout<<"Enter the size of cache in MiB: ";
    cin>>size;
    cout<<"Enter the cache line size in Bytes: ";
    cin>>linesize;
    cout<<"Enter number of associative ways: ";
    cin>>way;

    set= (size*1024)/(way*linesize);
    //cout<<endl<<"Number of sets(Ki): "<<set<<endl;

    while (set>=2)
    {
        set=set/2;
        indexbits++;
    }

    cout<<endl<<"Index bits: "<<indexbits+10<<endl;

    while (linesize>=2)
    {
        linesize=linesize/2;
        byteoffset++;
    }

    cout<<"Byte Offset bits: "<<byteoffset<<endl;
    cout<<"Tag bits: "<<32-(byteoffset+indexbits+10)<<endl;
    
}
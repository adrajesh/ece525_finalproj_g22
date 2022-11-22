#include <iostream>
using namespace std;

int main() 
{
    int size;
    int linesize;
    int way;
    int set;
    int count=0;
    cout<<"Enter the size of cache in MiB: ";
    cin>>size;
    cout<<endl<<"Enter the cache line size in Bytes: ";
    cin>>linesize;
    cout<<endl<<"Enter number of associative ways: ";
    cin>>way;

    set= (size*1024)/(way*linesize);
    cout<<endl<<"Number of sets(Ki): "<<set<<endl;

    while (set>=1)
    {
        set=set/2;
        count++;
    }
    
    
    int hex[1000];
    int i = 1, j = 0, rem, dec = 0, binaryNumber;
        cin>> binaryNumber;
    while (binaryNumber > 0) 
    {
        rem = binaryNumber % 2;
        dec = dec + rem * i;
        i = i * 2;
        binaryNumber = binaryNumber / 10;
    }
        i = 0;
    while (dec != 0) 
    {
        hex[i] = dec % 16;
        dec = dec / 16;
        i++;
    }

    cout<<" The hexadecimal value: ";
    for (j = i - 1; j >= 0; j--)
    {
        if (hex[j] > 9) 
        cout<<(char)(hex[j] + 55)<<"\n";
        else
        cout<<hex[j];
    }

    return 0;
}
#include<stdio.h>
#include<windows.h>
#include<iostream>
using namespace std;


int main()
{
    if (IsDebuggerPresent())
    {
        cout << "Stop debugging program!" << std::endl;
        exit(-1);
    }
    return 0;
} 
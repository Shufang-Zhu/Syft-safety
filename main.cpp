#include <iostream>
#include <string>
#include "syn.h"

using namespace std;

int main(int argc, char ** argv){

    string filename;
    string partfile;
    string flag;
    if(argc != 4){
        cout<<"Usage: ./SSyft DFAfile Partfile Starting_player(0: system, 1: environment)"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
        flag = argv[3]; 
    }
    
    syn test(filename, partfile);
    bool res = 0;
    if(flag == "1")
        res = test.realizablity_variant();
    else
        res = test.realizablity();

    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn

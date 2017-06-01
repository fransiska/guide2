#include <iostream>
#include "sortTeachFile.h"
using namespace std;

int main(int argc, char**argv) {
  if(argc<2) {
    cerr << "usage: " << argv[0] << " " << "unsortedfile.tea" << endl;
    exit(1);
  }
  stringstream ss,so;
  ss << argv[1];
  string folder = ss.str().substr(0,ss.str().find_last_of("/\\"));
  string fileout = ss.str().substr(ss.str().find_last_of("/\\")+1,ss.str().size());
  so << folder << "/" << fileout.substr(0,fileout.size()-4) << ".conv.tea"; 
  convertFile(argv[1],so.str());
  return 0;
} 

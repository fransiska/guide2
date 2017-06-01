#include <fstream>
#include "teachViewerWindow.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

int main(int argc, char**argv) {
  if(argc<2) {
    cerr << "usage: sortTea <posture.tea file>" << endl;
    exit(1);
  }

  ifstream infile;
  infile.open (argv[1]);
  if(!infile.good()) {
    cerr <<"error opening "<< argv[1] <<endl;
    exit(2);
  }

  
  std::string tea,name;
  helium::convert(tea,argv[1]);

  name = tea.substr(0,tea.rfind("tea"));
  name.append("conv.tea");

  convertFile(tea,name);
  
  std::stringstream ss;
  
  tea = name.substr(0,name.find("conv"));
  tea.append("conv.tea");
  
  ss << "./xmlTeach.xps0 " << tea;
  system(ss.str().c_str());
  
  infile.close();
  
  return 0;
}

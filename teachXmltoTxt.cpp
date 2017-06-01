#include <tinyxml/tinyxml.h>
#include <iostream>
#include <fstream>
#include <helium/vector/array.h>
#include <helium/robot/frame.h>

using namespace std;


class TeachList {
public:
  helium::Array<int> sensor;
  helium::Posture input;
  helium::Posture output;
  TeachList(const helium::Array<int> &psensor, const helium::Posture &panglei, const helium::Posture &pangleo);
  TeachList(const helium::Array<int> &psensor, const helium::Posture &panglei);
  TeachList(const std::string& psensor, const std::string& anglei, const std::string& angleo);
};

TeachList::TeachList(const helium::Array<int> &psensor, const helium::Posture &panglei, const helium::Posture &pangleo):
  sensor(psensor),input(panglei),output(pangleo) {
}
TeachList::TeachList(const helium::Array<int> &psensor, const helium::Posture &panglei):
  sensor(psensor),input(panglei),output(panglei.size()) {
}
TeachList::TeachList(const std::string& psensor, const std::string& anglei, const std::string& angleo) {
  helium::fromString(input,anglei);
  helium::fromString(output,angleo);
  helium::fromString(sensor,psensor);
}

int main(int argc, char** argv) {

  cout << argv[1] << endl;

  vector<TeachList> tl;

  TiXmlDocument doc(argv[1]);
  std::stringstream fname;
  fname << argv[1];

  if(!doc.LoadFile())
    throw helium::exc::XMLError("while loading "+fname.str()+":"+doc.ErrorDesc());    

  TiXmlElement* root = doc.FirstChildElement("instructions");
  if(!root)  {
    doc.Clear();
    throw helium::exc::XMLError(fname.str()+" has wrong TeachList xml format");    
  }
  for(TiXmlElement* ins=root->FirstChildElement("instruction");ins;ins=ins->NextSiblingElement()) {

    if(ins->FirstChildElement("sensor") &&
       ins->FirstChildElement("poseinput") &&
       ins->FirstChildElement("poseoutput")
       ) {
      
      
      tl.push_back(TeachList(ins->FirstChildElement("sensor")->GetText(),
			     ins->FirstChildElement("poseinput")->GetText(),
			     ins->FirstChildElement("poseoutput")->GetText()
			     ));
    }
    else
      throw helium::exc::XMLError(fname.str()+" has wrong TeachList xml format");    
  }//for 

  
  
  ofstream myfile;
  stringstream ss;

  //sensor
  ss << argv[1] << ".sensor.out";
  cout << ss.str() << " " << tl.size() << endl;
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<tl.size();i++) {
    myfile <<  tl[i].sensor << "\n";
  }
  myfile.close();

  //pin
  ss.str("");
  ss << argv[1] << ".pin.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<tl.size();i++) {
    myfile <<  tl[i].input << "\n";
  }
  myfile.close();

  //pout
  ss.str("");
  ss << argv[1] << ".pout.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<tl.size();i++) {
    myfile <<  tl[i].output << "\n";
  }
  myfile.close();


  return 0;
}

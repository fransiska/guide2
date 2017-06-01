#ifndef SORTTEACHFILE
#define SORTTEACHFILE

#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "teachIO.h"
#include <helium/xml/xmlLoader.h>

const std::string defaultFolder="/home/fransiska/research/touchExperiment/";
const int maxSensorId = 113;
const int sensorSize = 90;
const int motorSize = 22;


inline  void updateLastOpenedFile(std::string lof, std::string &folder, std::string &title,std::string &posture,std::string &username) {
    if(lof.size()) {
      folder = lof.substr(0,lof.find_last_of("/\\"));
      title = lof.substr(folder.find_last_of("/\\")+1,lof.size());
      username = lof.substr(folder.find_last_of("/\\")+1,title.find_last_of("/\\"));
      posture = title.substr(0,title.find('.'));
      posture = posture.substr(posture.find_last_of("/\\")+1,posture.size());
    }
  }

inline void convertFile(std::string lastOpenedFile, std::string conver) {


  std::ifstream infile;
  infile.open (lastOpenedFile.c_str());
  if(!infile.good()) {
    std::cerr << "fail opening " << lastOpenedFile << std::endl;    
    exit(1);
  }

      std::vector<TeachIO> tl;
      helium::XmlLoader xml;
      helium::Memory inmem;
      xml.loadDataAndModel(inmem,lastOpenedFile);
      helium::dimport(tl,inmem);

      std::string title,folder,username,posture;
      updateLastOpenedFile(lastOpenedFile,folder,title,posture,username);

      std::string randomFile = folder;
      randomFile.append("/random_");
      randomFile.append(posture);
      randomFile.append("_");
      randomFile.append(username);
      randomFile.append(".txt");
      std::cout << "fetching id from " <<randomFile << std::endl;
      
      std::fstream in(randomFile.c_str());
      if(!in.is_open()) {
	throw randomFile;
      }

      std::cout << tl.size() << " teaches" <<std::endl;

      //do stuff
      //create sorted vector
      std::vector<TeachIO> sortedtl;
      for(int i=0;i<maxSensorId;i++) {
	helium::Array<int> sensorMax(sensorSize);
	helium::Array<int> sensorMin(sensorSize);
	helium::Array<int> sensor(sensorSize);
	helium::Posture anglei(motorSize);
	helium::Posture angleo(motorSize);
	helium::Array<int> gyro(5);
	helium::zero(sensorMax);
	helium::zero(sensorMin);
	helium::zero(sensor);
	helium::zero(anglei);
	helium::zero(angleo);
	helium::zero(gyro);
	sortedtl.push_back(TeachIO(sensor,anglei,gyro,sensorMin,sensorMax,0));
	sortedtl[i].tout = 0;
	sortedtl[i].output = angleo;
      }

      std::string line,partname;
      char unders;
      std::vector<int> randomId;
      int j = 0;
      while(in) {
	getline(in,line);
	if(line.size()==0) break;
	int d;
	std::stringstream ss;
	ss << line;
	ss >> d;
	ss >> unders;
	ss >> partname;

	partname = partname.substr(0,partname.find('.'));
	std::stringstream ps;
	ps << j << " " << partname;

	randomId.push_back(d);
	
	
	//put in the sorted tl
	sortedtl[d] = tl[j];
	sortedtl[d].name = ps.str();
	helium::subn(sortedtl[d].input,tl[j].output,tl[j].input,motorSize);
	j++;
      }
       


      std::cout << "saving converted file to " << conver << std::endl;

      helium::Memory outmem;    
      helium::XmlLoader out;
      dexport(outmem,sortedtl);    
      out.save(conver.c_str(),outmem);
  }  
#endif

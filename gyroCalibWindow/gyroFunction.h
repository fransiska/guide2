/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GYROFUNCTION
#define GYROFUNCTION
#include <vector>
#include <helium/math/random.h>

void getRandomCouples(std::vector<std::pair<int,int> > &idx,int n){
  int *p=new int[n];
  idx.clear();
  helium::randomPermutation(p,n);
  for (int i=0;i+1<n;i=i+2){
    idx.push_back(std::make_pair(p[i],p[i+1]));
  }
  delete[] p;
}


void getAllCouples(std::vector<std::pair<int,int> > &idx,int n){   
  for (int i=0;i<n;i++){
    for (int j=i+1;j<n;j++){
      idx.push_back(std::make_pair(i,j));
    }
  }
}

void getStatistics(double* vars,double& rm,double &rvar,const std::vector<helium::Array<double,3> > &data,int n,const double* c){
//void getStatistics(double* vars,double& rm,double &rvar,const double* data,int n,const double* c){
  double m[3];
  memset(m,0,sizeof(double)*3);
  memset(vars,0,sizeof(double)*3);
  
  rm=0;
  rvar=0;  
  
  for (int i=0;i<n;i++){
    for (int j=0;j<3;j++){
      m[j]+=data[i][j]/n; //m[j]+=data[i*3+j]/n;
    }
    double v=sqrt(pow(c[3]*(data[i][0]-c[0]),2)+ //pow(c[3]*(data[i*3+0]-c[0]),2)+
		  pow(c[4]*(data[i][1]-c[1]),2)+ //pow(c[4]*(data[i*3+1]-c[1]),2)+
		  pow(data[i][2]-c[2],2))/n; //pow(data[i*3+2]-c[2],2))/n;
    rm+=v;
    //cout<<v<<" "<<rm<<endl;

  }

  for (int i=0;i<n;i++){
    for (int j=0;j<3;j++){
      vars[j]+=pow(data[i][j]-m[j],2)/n; // vars[j]+=pow(data[i*3+j]-m[j],2)/n;
    }
    double v=sqrt(pow(c[3]*(data[i][0]-c[0]),2)+ //pow(c[3]*(data[i*3+0]-c[0]),2)+
		  pow(c[4]*(data[i][1]-c[1]),2)+ //pow(c[4]*(data[i*3+1]-c[1]),2)+
		  pow(data[i][2]-c[2],2))/n; //pow(data[i*3+2]-c[2],2))/n;

    rvar+=pow(v-rm,2)/n;
  }

}

void getMean(std::vector<std::vector<double> > mean, double* m) {
  for(int j=0;j<5;j++) {
    unsigned int i=0;
    double temp=0;
    while(i<mean.size()) {
      temp+=mean[i][j];
      i++;
    }
    m[j]=temp/(double)mean.size();
  }
}

#endif //GYROFUNCTION

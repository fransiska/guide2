///for data analysis
#ifndef EXPERIMENTUSERINFO
#define EXPERIMENTUSERINFO

namespace experiment{
  const std::string user[]  = {"01_ishida",            //0
			       "02_yoshikawa",         //1
			       "03_ueda_teach",        //2
			       "04_yoshimura_teach",   //3
			       "05_nakanishi",         //4
			       "06_denny_teach",   //5 
			       "07_arimoto_teach", //6
			       "08_nakamichi",     //7
			       "09_katou_teach", 
			       "10_kurima",        //8
			       "11_watanabe_teach",//9
			       "12_hamada",        //10
			       "13_hoshikawa",     //11
			       "14_okadome_teach", //12
			       "15_jo",         //13
			       "16_urai_teach", //14
			       "17_nishio",      //15
			       "18_li_teach",
			       "19_chang",
			       "20_mao_teach",
			       "21_yamanaka_teach"
                               };    

const std::string posture[] = {"standing","sitting","crawling"}; 
const std::string posangle[] = {"0 0 -0.777433 0.777433 1.15234 -1.15234 0.0913377 -0.0913377 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
				" 0.00106207 -0.0031862 0.0573516 -0.0594757 1.02489 -1.03233 -0.108331 0.114703 0.0212413 -0.031862 -0.00106207 0.0106207 -1.31696 1.32971 -0.00849653 -0.00849653 -0.031862 0.0212413 0.320744 -0.307999 0.00531033 0.00424827",
				" -0.372785 -0.0945239 1.33077 -1.33077 0.995156 -0.995156 0.288882 -0.288882 0.079655 -0.0785929 0.0350482 -0.0361103 0.216662 -0.216662 -1.33396 1.34033 0.573516 -0.573516 -0.276137 0.267641 0.0339861 -0.0350482"};

const int USR = 4;
const int MOT = 22;
const int SEN = 113;
const int RSEN = 90;
const int POS = 3;
const int BEHAV = 34;
};

#endif

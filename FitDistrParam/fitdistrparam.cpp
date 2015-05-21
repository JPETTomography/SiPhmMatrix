#include <stdio.h>
#include <fstream>
#include <memory>
#include <fit.h>
#include <initialconditions.h>
#include <filter.h>
#include <math_h/sympson.h>
#include <math_h/interpolate.h>
#include <math_h/singleparam.h>
#include <math_h/functions.h>
using namespace std;
using namespace Genetic;
double distr_actual(double x, double rize, double sigma, double decay){
	auto A=[sigma](double t){if(t<0)return 0.0;return Gaussian(t,2.5*sigma,sigma);};
	auto B=[rize,decay](double t){if(t<0)return 0.0;return exp(-t/decay)-exp(-t/rize);};
	return Sympson([x,rize,sigma,decay,A,B](double ksi){return A(ksi)*B(x-ksi);},-20.0,20.0,0.01);
}
double distr_old(double x, double sigma,double decay){
	auto A=[sigma](double t){if(t<0)return 0.0;return Gaussian(t,2.5*sigma,sigma);};
	auto B=[decay](double t){if(t<0)return 0.0;return exp(-t/decay);};
	return Sympson([x,sigma,decay,A,B](double ksi){return A(ksi)*B(x-ksi);},-20.0,20.0,0.01);
}
int main(int , char **){
	double sigma_old=0.20;
	double decay_old=1.50;
	double new_rise_t=0.01;
	SingleParam<double,0,double,double,double> A(distr_old,INFINITY,sigma_old,decay_old);
	auto points=make_shared<FitPoints>();
	for(double x=0; x<=10; x+=0.2)
		points<<make_pair(x,A(x));
	Fit<DifferentialMutations<>,ChiSquare> fit(points,[&new_rise_t](ParamSet&&X,ParamSet&&P){
		return distr_actual(X[0],new_rise_t,P[0],P[1]);
	});
	fit.SetFilter(make_shared<Above>()<<0<<0);
	auto initial=make_shared<GenerateByGauss>()
			<<make_pair(sigma_old,sigma_old*0.1)
		   <<make_pair(decay_old,decay_old*0.1);
	fit.Init(10,initial);
	while(!fit.ConcentratedInOnePoint())
		fit.Iterate();
	printf("DEFINES+=lighting_params=%f,%f,%f\n",new_rise_t,fit[0],fit[1]);
	{
		ofstream file;
		file.open("emission.txt");
		if(file.is_open()){
			for(double x=-1;x<=20;x+=0.01)
				file<<x<<"\t"<<A(x)<<"\t"<<fit(ParamSet(x))<<"\n";
		}
	}
	{string script=".plotscript.gp";
				ofstream str;
				str.open(script.c_str());
				if(str.is_open()){
						str<<"set terminal pngcairo size 800,600 enhanced monochrome font 'Verdana,18'\n";
						str<<"set output 'emission_time_distribution.png'\n";
						str<<"set xlabel 'Time [ns]'\n";
						str << "plot ";
						str <<"\"emission.txt\" using 1:2 w l ls 1 title \"Old distribution\"";
						str << ",\\\n";
						str <<"\"emission.txt\" using 1:3 w l ls 1 title \"New distribution\"";
						str.close();
				}
				system((string("gnuplot ")+script).c_str());
		}
	return 0;
}

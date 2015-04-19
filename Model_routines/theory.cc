#ifndef ___THEORY___
#define ___THEORY___
#include <LongScintillator/scintillator_debug.cc>
#include <LongScintillator/math_h/sympson.h>
template<class distr>
double IntegralForResolution(distr y, double beg, double end, double dx){
	auto y_der=[&y,dx](double x){return (y(x-dx)-y(x))/dx;};
	auto sub_int_func=[&y,y_der](double x){
		double Y=y(x);
		if(0.0==Y)return 0.0;
		return pow(y_der(x),2)/Y;
	};
	return Sympson(sub_int_func,beg,end,dx);
}
class TheorResolutionCalc:public ScintillatorDebug{
private:
	double integral;
public:
	TheorResolutionCalc(LongScintillator* scin):ScintillatorDebug(scin){
		double dx=0.001;double beg=0.0;double end=40.0;
		FuncTable<double> y(int((end-beg)/dx)+2);
		fillFuncTable(y,beg-dx,end,[this](double x){return LightingTimeDistr(x);});
		integral=IntegralForResolution(y,beg,end,dx);
	}
	double MinimalLightingSigma(uint N){return 1.0/(integral*N);}
	virtual ~TheorResolutionCalc(){}
};
#endif

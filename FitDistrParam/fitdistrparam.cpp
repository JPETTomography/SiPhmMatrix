#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <stdio.h>
#include <memory>
#include <genetic.h>
#include <initialconditions.h>
#include <filter.h>
#include <fitpoints.h>
#include <math_h/sympson.h>
#include <math_h/interpolate.h>
#include <math_h/singleparam.h>
#include <math_h/functions.h>
using namespace std;
using namespace Fit;
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
double sigma_old=0.20;
double decay_old=1.50;
double new_rise_t=0.01;
int main(int , char **){
	SingleParam<double,0,double,double,double> A(distr_old,INFINITY,sigma_old,decay_old);
	auto points=make_shared<SquareDiff>();
	for(double x=0.1; x<=10; x+=0.2)
		points->Add(ParamSet(x),A(x));
	DifferentialRandomMutations<> fit(make_shared<ParameterFunction<>>([&new_rise_t](ParamSet &X,ParamSet &P){
			return distr_actual(X[0],new_rise_t,P[0],P[1]);
	}),points,2);
	fit.SetFilter(make_shared<Above>()<<0<<0);
	auto initial=make_shared<GenerateByGauss>()
			<<make_pair(sigma_old,sigma_old*0.1)
		   <<make_pair(decay_old,decay_old*0.1);
	fit.Init(30,initial);
	while(!fit.ConcentratedInOnePoint())
		fit.Iterate();
	printf("%f,%f,%f\n",new_rise_t,fit[0],fit[1]);
	SingleParam<double,0,double,double,double,double> B(distr_actual,INFINITY,0.005,0.20,1.50);
	{QFile file("output.txt");QFile file2("output2.txt");QFile file3("output3.txt");
		file.open(QFile::WriteOnly);file2.open(QFile::WriteOnly);
		if((file.isOpen())&&(file2.isOpen())){
			QTextStream str(&file);QTextStream str2(&file2);
			for(double x=-1; x<=10; x+=0.01){
				str <<x<<" " << A(x) <<"\n";
				str2<<x<<" " << fit(ParamSet(x)) <<"\n";
			}
			file.close();file2.close();
		}
	}
	{QString script=".plotscript.gp";
		QFile file(script);
		file.open(QFile::WriteOnly);
		if(file.isOpen()){
			QTextStream str(&file);
			str << "plot ";
			str <<"\"output.txt\" w l title \"old params\"";
			str << ",\\\n";
			str <<"\"output2.txt\" w l title \"new params\"";
			str << "\n";
			str<<"\npause -1";
			file.close();
		}
		QProcess *gnuplot=new QProcess();
		gnuplot->startDetached("gnuplot",QStringList()<<script);
	}
	return 0;
}

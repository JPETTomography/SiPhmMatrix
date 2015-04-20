#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <TH1F.h>
#include <LongScintillator/scintillator3d.h>
#include <LongScintillator/photoncounters.h>
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
const uint n=24;
uint N[]={1,2, 3, 4,5,6,7,8,9, 10, 20, 50, 100, 200, 500, 1000, 1800, 2000, 3000, 3410, 4000,5000,6000, 10000 };
const uint n_exp=3;
double L[]={300,500,1000};
int main(int , char **){
	Scintillator3D_rect scintillator(1200,scin_hwx_si,scin_hwy_si,scin_refr,lighting_params);
	{QFile file("theory.output.txt");
		file.open(QFile::WriteOnly);
		if(file.isOpen()){
			TheorResolutionCalc debug(&scintillator);
			{
				auto F=[&debug](double x){return debug.LightingTimeDistr(x);};
				double norm=Sympson(F,0.0,39.0,0.005);
				double avr=Sympson([F,&norm](double x){return F(x)*x/norm;},0.0,39.0,0.005);
				double sigma=Sympson([F,&norm,&avr](double x){return pow(x-avr,2)*F(x)/norm;},0.0,39.0,0.005);
				Printf("Distribution parameters: average=%f; sigma=%f\n",avr,sqrt(sigma));
			}
			auto emission=[&debug](uint N_photons){
				return 4.0*debug.MinimalLightingSigma(N_photons);
			};
			QTextStream str(&file);
			for(uint i=0; i<n;i++){
				str <<N[i]<<" " << emission(N[i]) <<"\n";
			}
			file.close();
		}
	}
	for(uint l=0; l<n_exp;l++){
		double z=L[l];
		QFile file(QString("theory.")+QString::number(z)+".output.txt");
		file.open(QFile::WriteOnly);
		if(file.isOpen()){
			QTextStream str(&file);
			PhotonDistribution distr(&scintillator,-0.01,203,0.0,40.0);
			double x[]={z,0,0};
			scintillator.RegisterLighting(x,100000);
			double norm=Sympson([&distr](double x){return distr.LeftDistr(x);},0.0,39.0,0.01);
			auto Y=[&distr,norm](double x){return distr.LeftDistr(x)/norm;};
			double integral=IntegralForResolution(Y,0.0,39.0,0.005);
			auto registering=[&integral](uint N){return 4.0/(integral*N);};
			for(uint i=0; i<n;i++){
				str <<N[i]<<" " << registering(N[i]) <<"\n";
			}
			file.close();
		}
	}
	{QString script=".plotscript.gp";
		QFile file(script);
		file.open(QFile::WriteOnly);
		if(file.isOpen()){
			QTextStream str(&file);
			str<<"set terminal pngcairo size 800,600 enhanced monochrome font 'Verdana,16'\n";
			str<<"set output 'theory_estimation.png'\n";
			str<<"set xlabel 'Number of emitted photons'\n";
			str<<"set ylabel 'Lower limit [ns]'\n";
			str<<"set logscale x\n";
			str<<"set logscale y\n";
			QStringList colors;
			colors<<"rgb 'black'"<<"rgb 'black'"<<"rgb 'gray'"<<"rgb 'gray'";
			QStringList types;
			types<<"1"<<"0"<<"1"<<"0";
			QStringList widths;
			widths<<"1"<<"3"<<"1"<<"3";
			for(uint l=0;l<=n_exp;l++)
				str<<"set style line "<<l+1<<" lt "<<types[l]<<" linecolor "<<colors[l]<<" lw "<<widths[l]<<"\n";
			str << "plot ";
			str <<"\"theory.output.txt\" w l ls 1 title \"emission\"";
			for(uint l=0; l<n_exp;l++){
				double z=L[l];
				str << ",\\\n";
				str <<"\"theory."<<z<<".output.txt\" w l ls "<<l+2<<" title \""<<z/10<<" cm\"";
			}
			str << ",\\\n";
			str << "\"<echo '200 0.08'\" with points title \"Experiment\"";
			str<<"\npause -1";
			file.close();
		}
		QProcess *gnuplot=new QProcess();
		gnuplot->startDetached("gnuplot",QStringList()<<script);
	}
	return 0;
}

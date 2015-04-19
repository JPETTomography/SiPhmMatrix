#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <LongScintillator/scintillator3d.h>
#include <LongScintillator/photoncounters.h>
#include <Model_routines/theory.cc>
const uint n=23;
uint N[]={2, 3, 4,5,6,7,8,9, 10, 20, 50, 100, 200, 500, 1000, 1800, 2000, 3000, 3410, 4000,5000,6000, 10000 };
const uint n_exp=3;
double L[]={300,500,1000};
int main(int , char **){
	Scintillator3D_rect scintillator(1200,scin_hwx_si,scin_hwy_si,scin_refr,lighting_params);
	{QFile file("theory.output.txt");
		file.open(QFile::WriteOnly);
		if(file.isOpen()){
			TheorResolutionCalc debug(&scintillator);
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
			str<<"set xlabel 'Number of emitted photons'\n";
			str<<"set ylabel 'Lower limit [ns]'\n";
			str<<"set logscale x\n";
			str<<"set logscale y\n";
			str << "plot ";
			str <<"\"theory.output.txt\" w l title \"emission\"";
			for(uint l=0; l<n_exp;l++){
				double z=L[l];
				str << ",\\\n";
				str <<"\"theory."<<z<<".output.txt\" w l title \""<<z/10<<" cm\"";
			}
			str << ",\\\n";
			str << "\"<echo '200 0.8'\" with points title \"Experiment\"";
			str<<"\npause -1";
			file.close();
		}
		QProcess *gnuplot=new QProcess();
		gnuplot->startDetached("gnuplot",QStringList()<<script);
	}
	return 0;
}
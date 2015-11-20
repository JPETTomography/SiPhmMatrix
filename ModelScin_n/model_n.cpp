// this file is distributed under
// GPL v 3.0 license
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <Model_routines/displaygraph.h>
#include <Model_routines/display_root_object.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/math_h/randomfunc.h>
class lt_test:public LongScintillator{
public:
	lt_test(ScinLightingParamsHeader):LongScintillator(1,ScinLightingParams){}
protected:
	virtual void TracePhoton(double* data, bool isright, double,double, double,std::mt19937&G)override{
		PhotonReachesPhotomultiplier(isright,data,G);
	}
};
int nn=22;
double NN[]={25,
			 30, 40, 50,
			 80, 100,200,
			 500, 700,1000,
			 1500, 2000, 2500,
			 3000, 4000, 5000,
			6000,7000,8000,
			10000, 15000, 20000};
int main(int , char **arg){
	std::mt19937 G;
	std::uniform_real_distribution<double> posx(-scin_hwx,scin_hwx),posy(-scin_hwy,scin_hwy);
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double n_ph[nn];
	double* sig_time_diff[K];
	for(uint k=0; k<K; k++)
		sig_time_diff[k]=new double[nn];
	double X_lighting[3];X_lighting[0]=0.5;
	for(int index=0; index<nn;index++ ){
		LongScintillator *scintillator=new lt_test(scin_refr,lighting_params);
		uint N_photons=NN[index];
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for N = %i (%i virtual experiments)...", N_photons, events_number);
		FirstPhotonTimeRes first_photons(scintillator,this_K);
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=posx(G);X_lighting[2]=posy(G);
			scintillator->RegisterLighting(X_lighting,N_photons,G);
		}
		n_ph[index]=N_photons;
		for(uint k=0; k<K; k++){
			sig_time_diff[k][index]=first_photons.SigmaTimeDifference(k);
			Printf("Sigma: %f",sig_time_diff[k][index]);
		}
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		delete scintillator;
	}
	DisplayObject_plusplus("nnn.png",[](TCanvas* c1, TMultiGraph*){
		c1->SetLogx();
	},MakeGraph(K,nn,n_ph,sig_time_diff,Sigma2CRT,"diffgr",""),"acp",[](TCanvas*, TMultiGraph*){
	});
	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<n; i++){
			str<<n_ph[i];
			for(uint k=0;k<K;k++)
				str<<" "<<sig_time_diff[k][i];
			str<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	for(uint k=0; k<K; k++)
		delete[] sig_time_diff[k];
	return 0;
}

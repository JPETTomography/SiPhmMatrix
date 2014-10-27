#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/math_h/randomfunc.h>
#include <LongScintillator/math_h/interpolate.h>
int main(int , char **arg){
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double n_ph[n];
	double n_ph_l[n];
	double n_ph_r[n];
	double n_ph_both[n];
	double *sig_time_left[K+1],*sig_time_right[K+1],*sig_time_diff[K+1];
	for(uint k=0; k<=K; k++){
		sig_time_left[k]=new double[n];
		sig_time_right[k]=new double[n];
		sig_time_diff[k]=new double[n];
	}
	double X_lighting[3];X_lighting[0]=scin_z;
	LongScintillator *scintillator=CreateScintillatorBC420();
	LongScintillator *scintillator2=CreateScintillatorBC420();
	AbstractPhotoMultiplier *photomult=CreateTubePhotoMultiplier(scintillator);
	AbstractPhotoMultiplier *photomult2=CreateTubePhotoMultiplier(scintillator2);
	for(int index=0; index<n;index++ ){
		uint N_photons=(index*step)+scin_begin;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		PhotonCounter counter(photomult,true);
		FirstPhotonTimeRes first_photons(photomult,this_K);
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx,scin_hwx);
			X_lighting[2]=RandomUniformly(-scin_hwy,scin_hwy);
			scintillator->RegisterLighting(X_lighting,N_photons);
		}
		n_ph[index]=N_photons;
		n_ph_l[index]=double(counter.LeftCount())/events_number;
		n_ph_r[index]=double(counter.RightCount())/events_number;
		n_ph_both[index]=n_ph_l[index]+n_ph_r[index];
		for(uint k=0; k<K; k++){
			sig_time_left[k][index]=first_photons.SigmaLeftPhotonTime(k);
			sig_time_right[k][index]=first_photons.SigmaRightPhotonTime(k);
			sig_time_diff[k][index]=first_photons.SigmaTimeDifference(k);
			Printf("Sigma: %f; %f",sig_time_left[k][index],sig_time_diff[k][index]);
		}
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Second Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		auto w_d=[&first_photons](int index){
			double ss=first_photons.SigmaSqrTimeDifference(index);
			if(0==std::isfinite(ss))return 0.0;
			if(ss>first_photons.SigmaSqrTimeDifference(0))return 0.0;
			return 1.0/ss;
		};
		auto s_d=[](FirstPhotonsReg* reg, int i){return reg->TimeDifference(i);};
		SignalObtainer<decltype(w_d),decltype(s_d),FirstPhotonsReg,uint>
				signal_diff(photomult2,w_d,s_d,this_K,this_K);
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx,scin_hwx);
			X_lighting[2]=RandomUniformly(-scin_hwy,scin_hwy);
			scintillator2->RegisterLighting(X_lighting,N_photons);
		}
		sig_time_diff[K][index]=signal_diff.ResolutionSignal();
		Printf("Sigma: %f; %f",sig_time_left[K][index],sig_time_diff[K][index]);
	}
	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	displaygraph(MakeGraph(K,n,n_ph,sig_time_left,"leftgr","Left Photomultiplier"),n,n_ph,n_ph_l,"Number of photoelectrons");
	displaygraph(MakeGraph(K,n,n_ph,sig_time_left,"rightgr","Right Photomultiplier"),n,n_ph,n_ph_l,"Number of photoelectrons");
	displaygraph(MakeGraph(K+1,n,n_ph,sig_time_diff,"diffgr",""),n,n_ph,n_ph_both,"Number of photoelectrons");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<n; i++){
			str<<n_ph[i]<<" "<<n_ph_l[i]<<" "<<n_ph_r[i]<<" "<<n_ph_both[i];
			for(uint k=0;k<=K;k++)
				str<<" "<<sig_time_diff[K][i];
			str<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	for(uint k=0; k<=K; k++){
		delete[] sig_time_left[k];
		delete[] sig_time_right[k];
		delete[] sig_time_diff[k];
	}
	delete photomult;delete scintillator;
	delete photomult2;delete scintillator2;
	return 0;
}

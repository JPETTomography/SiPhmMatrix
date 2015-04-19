#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <Model_routines/display_root_object.h>
#include <LongScintillator/math_h/randomfunc.h>
uint uklad[]={ukl_arr};
double uklad_dead[]={ukl_dead};
int main(int , char **arg){
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double X_lighting[3];X_lighting[0]=scin_z;
	double n_ph[n];
	double* sig_time_diff[ukl_k];
		for(uint k=0; k<ukl_k; k++)sig_time_diff[k]=new double[n];
	LongScintillator *scintillator=CreateScintillatorBC420_4Si_matrix();
	AbstractPhotoMultiplier *photomult=CreateSiliconPhotoMultiplier(scintillator);
	LongScintillator *scintillator2=CreateScintillatorBC420_4Si_matrix();
	AbstractPhotoMultiplier *photomult2=CreateSiliconPhotoMultiplier(scintillator2);
	for(int index=0; index<n;index++ ){
		uint N_photons=(index*step)+ukl_N;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		SortFirst* first_phm[ukl_k];
		for(uint k=0;k<ukl_k;k++)first_phm[k]=
				new	SortFirst(photomult,FirstConstrPar(uklad[k*2],uklad[k*2+1],uklad_dead[k]));
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx_si,scin_hwx_si);
			X_lighting[2]=RandomUniformly(-scin_hwy_si,scin_hwy_si);
			scintillator->RegisterLighting(X_lighting,N_photons);
		}
		n_ph[index]=N_photons;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Second Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		uint k_=0;
		auto w_d=[&first_phm,&k_](int index){
			double ss=first_phm[k_]->getSigmaSqr(index);
			if((index>2)&&(ss>first_phm[k_]->getSigmaSqr(0)))return 0.0;
			if(0==std::isfinite(ss))return 0.0;
			return 1.0/ss;
		};
		auto s_d=[](SortFirst* master, int i){return master->operator[](i);};
		SignalObtainer<decltype(w_d),decltype(s_d),TemplateParams> *signal_diff[ukl_k];
		for(k_=0;k_<ukl_k;k_++)signal_diff[k_]=
				new SignalObtainer<decltype(w_d),decltype(s_d),TemplateParams>(
						photomult2,w_d,s_d,uklad[k_*2]*uklad[k_*2+1],
						FirstConstrPar(uklad[k_*2],uklad[k_*2+1],uklad_dead[k_])
					);
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx_si,scin_hwx_si);
			X_lighting[2]=RandomUniformly(-scin_hwy_si,scin_hwy_si);
			scintillator2->RegisterLighting(X_lighting,N_photons);
		}
		for(uint k=0;k<ukl_k;k++){
			sig_time_diff[k][index]=signal_diff[k]->ResolutionSignal();
			Printf("Sigma_gen[%i][%i] = %f",k,index,sig_time_diff[k][index]);
			delete signal_diff[k];
		}
		for(uint k=0;k<ukl_k;k++)delete first_phm[k];
	}

	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	displaygraph(MakeGraph(ukl_k,n,n_ph,sig_time_diff,"diffgr",""),n,n_ph,NULL,"_additional");
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<n; i++){
			str<<n_ph[i];
			for(uint k=0;k<ukl_k;k++)
				str<<" "<<sig_time_diff[k][i];
			str<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	for(uint k=0; k<ukl_k; k++)
			delete[] sig_time_diff[k];
	delete photomult;delete scintillator;
	delete photomult2;delete scintillator2;
	return 0;
}
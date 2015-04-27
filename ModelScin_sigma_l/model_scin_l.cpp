#include <QDateTime>
#include <Model_routines/displaygraph.h>
#include <Model_routines/display_root_object.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/math_h/randomfunc.h>
int nn=10;
double lengths[]={30, 50, 75, 100, 150, 200, 300, 400, 500, 1000};
int main(int , char **arg){
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double n_ph[nn];
	double* sig_time_diff[3];
	for(uint k=0; k<3; k++)
		sig_time_diff[k]=new double[nn];
	double X_lighting[3];
	for(int index_ph=27;index_ph<28; index_ph+=1){
	for(int index=0; index<nn;index++ ){
		uint N_photons=100*index_ph;
		double length=lengths[index];X_lighting[0]=length/2;
		LongScintillator *scin_ideal=CreateScintillatorBC420_4Si_matrix(length);
		LongScintillator *scin_ideal2=CreateScintillatorBC420_4Si_matrix(length);
		LongScintillator *scin_real=CreateScintillatorBC420_4Si_matrix(length);
		LongScintillator *scin_real2=CreateScintillatorBC420_4Si_matrix(length);
		AbstractPhotoMultiplier* phm_r=CreateTubePhotoMultiplier(scin_real);
		AbstractPhotoMultiplier* phm_r2=CreateTubePhotoMultiplier(scin_real2);
		LongScintillator *scin_si=CreateScintillatorBC420_4Si_matrix(length);
		LongScintillator *scin_si2=CreateScintillatorBC420_4Si_matrix(length);
		AbstractPhotoMultiplier* phm_si=CreateSiliconPhotoMultiplier(scin_si);
		AbstractPhotoMultiplier* phm_si2=CreateSiliconPhotoMultiplier(scin_si2);
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for L = %f  N = %i (%i virtual experiments)...",length,N_photons,events_number);
		FirstPhotonTimeRes first_ideal(scin_ideal,this_K);
		FirstPhotonTimeRes first_real(phm_r,this_K);
		SortFirst first_si(phm_si,FirstConstrPar(phm_x,phm_y,phm_dead));
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx_si,scin_hwx_si);
			X_lighting[2]=RandomUniformly(-scin_hwy_si,scin_hwy_si);
			scin_ideal->RegisterLighting(X_lighting,N_photons);
			scin_real->RegisterLighting(X_lighting,N_photons);
			scin_si->RegisterLighting(X_lighting,N_photons);
		}
		n_ph[index]=length;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Second Monte Carlo for L= %f; N = %i (%i virtual experiments)...", length,N_photons, events_number);
		auto w_i=[&first_ideal](uint index){
			double ss=first_ideal.SigmaSqrTimeDifference(index);
			if(0==std::isfinite(ss))return 0.0;
			if(ss>first_ideal.SigmaSqrTimeDifference(0))return 0.0;
			return 1.0/ss;
		};
		auto s_ff=[](FirstPhotonsReg* reg, uint i){return reg->TimeDifference(i);};
		SignalObtainer<decltype(w_i),decltype(s_ff),FirstPhotonsReg,uint>
				signal_ideal(scin_ideal2,w_i,s_ff,this_K,this_K);
		auto w_s=[&first_si](uint index){
			double ss=first_si.getSigmaSqr(index);
			if((index>2)&&(ss>first_si.getSigmaSqr(0)))return 0.0;
			if(0==std::isfinite(ss))return 0.0;
			return 1.0/ss;
		};
		auto s_si=[](SortFirst* reg, uint i){return reg->operator[](i);};
		SignalObtainer<decltype(w_s),decltype(s_si),TemplateParams>
				signal_si(phm_si2,w_s,s_si,phm_x*phm_y,FirstConstrPar(phm_x,phm_y,phm_dead));
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformly(-scin_hwx_si,scin_hwx_si);
			X_lighting[2]=RandomUniformly(-scin_hwy_si,scin_hwy_si);
			scin_ideal2->RegisterLighting(X_lighting,N_photons);
			scin_real2->RegisterLighting(X_lighting,N_photons);
			scin_si2->RegisterLighting(X_lighting,N_photons);
		}
		sig_time_diff[0][index]=signal_ideal.ResolutionSignal();
		sig_time_diff[1][index]=(first_real.SigmaTimeDifference(0)+first_real.SigmaTimeDifference(2))/2.0;
		sig_time_diff[2][index]=signal_si.ResolutionSignal();
		delete phm_si;delete phm_si2;
		delete scin_ideal;delete scin_ideal2;
		delete phm_r;delete phm_r2;
		delete scin_real;delete scin_real2;
		delete scin_si;delete scin_si2;
	}
	DisplayObject_plusplus(Form("length_dep_%i.png",index_ph),[](TCanvas*, TMultiGraph*){
	},MakeGraph(3,nn,n_ph,sig_time_diff,"diffgr",""),"acp",[](TCanvas*, TMultiGraph* gr){
		gr->GetYaxis()->SetTitle("#sigma [ns]");
		gr->GetXaxis()->SetLabelSize(0.05);
		gr->GetXaxis()->SetTitleSize(0.05);
		gr->GetYaxis()->SetLabelSize(0.05);
		gr->GetYaxis()->SetTitleSize(0.05);
		gr->GetXaxis()->SetTitle("Scintillator length [mm]");
	});
	}
	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	Printf("DONE.");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	for(uint k=0; k<3; k++)
		delete[] sig_time_diff[k];
	return 0;
}

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
#include <LongScintillator/math_h/interpolate.h>
const uint this_K_=1000;
int main(int , char **arg){
	std::mt19937 G;
	std::uniform_real_distribution<double> posx(-scin_hwx,scin_hwx),posy(-scin_hwy,scin_hwy);
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	LongScintillator *scintillator=CreateIdealScintillator(0);
	LongScintillator *scintillator2=CreateIdealScintillator(0);
	double X_lighting[3];X_lighting[0]=0;
	double n_ph[n];
	double* sig_time_diff[K+1];
	for(uint k=0; k<=K+1; k++)
		sig_time_diff[k]=new double[n];
	for(int index=0; index<n;index++ ){
		uint N_photons=(index*step)+ideal_begin;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		FirstPhotonTimeRes first_photons(scintillator,this_K_);
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
		if(N_photons==n_widm){
			TGraph* gr=new TGraph(this_K_);
			Printf("SAVING FILE");
			QString name(arg[0]);
			QFile file(name+".n.output.txt");
			file.open(QIODevice::WriteOnly);
			if(file.isOpen()){
				QTextStream str(&file);
				for(uint k=0; k<this_K_;k++){
					gr->SetPoint(k,k+1,first_photons.SigmaTimeDifference(k) *2.35/sqrt(2));
					str<<k<<" "<<first_photons.SigmaTimeDifference(k) *2.35/sqrt(2)<<"\n";
				}
				file.close();
			}
			Printf("DONE.");
			gr->SetTitle("");
			gr->GetYaxis()->SetTitle("CRT [ns]");
			gr->GetXaxis()->SetTitle("Order statistics");
			gr->GetXaxis()->SetLabelSize(0.05);
			gr->GetXaxis()->SetTitleSize(0.05);
			gr->GetYaxis()->SetLabelSize(0.05);
			gr->GetYaxis()->SetTitleSize(0.05);
			gr->SetLineColor(kBlack);
			gr->SetLineWidth(2);
			gr->SetMarkerStyle(8);
			gr->SetMarkerColor(kBlack);
			gr->SetMarkerSize(1);
			DisplayObject_plusplus("Fig_6.png",[](TCanvas* c, TGraph*){c->SetLogx();},gr,"",[](TCanvas*,TGraph*){});
		}
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Second Monte Carlo for N = %i (%i virtual experiments)...", N_photons, events_number);
		auto w_d=[&first_photons](int index){
			double ss=first_photons.SigmaSqrTimeDifference(index);
			if(0==std::isfinite(ss))return 0.0;
			if(ss>first_photons.SigmaSqrTimeDifference(0))return 0.0;
			return 1.0/ss;
		};
		auto s_d=[](FirstPhotonsReg* reg, int i){return reg->TimeDifference(i);};
		SignalObtainer<decltype(w_d),decltype(s_d),FirstPhotonsReg,uint>
				signal_diff(scintillator2,w_d,s_d,this_K,this_K);
		TH1F *hist[K+1];
		if(0==N_photons%1000){
			for(uint k=0; k<=K; k++)
				hist[k]=new TH1F(Form("rozpodzial%i_%i",k+1,N_photons),Form("Photon pair %i",k+1),100,-0.2,0.2);
			hist[K]=new TH1F(Form("rozpodzial%i_%i",K+1,N_photons),Form("Wieghted"),100,-0.2,0.2);
		}
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=posx(G);X_lighting[2]=posy(G);
			scintillator2->RegisterLighting(X_lighting,N_photons,G);
			if(0==N_photons%1000){
				for(uint k=0; k<K; k++)
					hist[k]->Fill(dynamic_cast<FirstPhotonsReg*>(&signal_diff)->TimeDifference(k));
				hist[K]->Fill(signal_diff.LastSignal());
			}
		}
		if(0==N_photons%1000){
			for(uint k=0; k<=K; k++)
				DisplayObject(Form("rozpodzial_%i_%i.png",N_photons,k),hist[k]);
		}
		sig_time_diff[K][index]=signal_diff.ResolutionSignal();
		Printf("Sigma: %f",sig_time_diff[K][index]);
	}
	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	displaygraph(MakeGraph(K+1,n,n_ph,sig_time_diff,"diffgr",""),n,n_ph);
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<n; i++){
			str<<n_ph[i];
			for(uint k=0;k<K+1;k++)
				str<<" "<<sig_time_diff[k][i];
			str<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	for(uint k=0; k<K+1; k++)
		delete[] sig_time_diff[k];
	delete scintillator;
	delete scintillator2;
	return 0;
}

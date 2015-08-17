// this file is distributed under
// GPL v 3.0 license
#include <random>
#include <QDateTime>
#include <TGraph.h>
#include <TH1F.h>
#include <TGaxis.h>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/display_root_object.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/photoncounters.h>
int main(int , char **arg){
	std::mt19937 G;
	std::uniform_real_distribution<double> posx(-scin_hwx,scin_hwx),posy(-scin_hwy,scin_hwy);
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double X_lighting[3];X_lighting[0]=scin_z;
	LongScintillator *scintillator=CreateScintillatorBC420();
	AbstractPhotoMultiplier *photomult=CreateTubePhotoMultiplier(scintillator);
	DisplayPlots();
	double n[this_K],sigma[3][this_K];
	for(uint k=0; k<this_K;k++)n[k]=k+1;
	PhotonHistogram hist_builder(photomult);
	hist_builder.StartHist("Time of registering",300,0,6);
	PhotonHistogram wavelength_cnt(photomult,3);
	wavelength_cnt.StartHist("Registered photons' wavelength",int(xmax-xmin),xmin,xmax);
	FirstPhotonTimeRes first_photons(photomult,this_K);
	for(uint cnt=0;cnt<events_number; cnt++){
		if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
		X_lighting[1]=posx(G);X_lighting[2]=posy(G);
		scintillator->RegisterLighting(X_lighting,n_widm,G);
	}
	for(uint k=0; k<this_K;k++){
		sigma[0][k]=first_photons.SigmaLeftPhotonTime(k) *2.35/sqrt(2);
		sigma[1][k]=first_photons.SigmaRightPhotonTime(k) *2.35/sqrt(2);
		sigma[2][k]=first_photons.SigmaTimeDifference(k) *2.35/sqrt(2);
	}
	QString name="timeres";
	QString title[3];
	title[0]="Left photomultiplier";
	title[1]="Right photomultiplier";
	title[2]="";
	for(uint i=0;i<3;i++){
		uint KK=0;while((KK<this_K)&&(std::isfinite(sigma[i][KK])))KK++;
		TGraph* gr=new TGraph(KK,&(n[0]),&(sigma[i][0]));
		gr->SetNameTitle((name+QString::number(i)).toStdString().c_str(),title[i].toStdString().c_str());
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
		DisplayObject_plusplus((QString("Sigma_n_")+QString::number(i)+".png").toStdString(),
				[](TCanvas* c, TGraph*){c->SetLogx();},gr,"",[](TCanvas*,TGraph*){});
	}
	{
		TGraph *gr=new TGraph(100);
		for(uint i=0;i<100;i++){
			double x=double(i)*0.01;
			gr->SetPoint(i,x,ReflectionProbability(1.58,x));
		}
		gr->SetNameTitle("refl","");
		gr->SetLineWidth(2);
		gr->SetMarkerStyle(0);
		gr->SetMarkerSize(0);
		gr->GetXaxis()->SetLabelSize(0.05);
		gr->GetXaxis()->SetTitleSize(0.05);
		gr->GetYaxis()->SetLabelSize(0.05);
		gr->GetYaxis()->SetTitleSize(0.05);
		gr->GetXaxis()->SetTitle("sin#alpha");
		gr->GetYaxis()->SetTitle("reflection probability");
		//gr->GetYaxis()->SetTitleOffset(1.5);
		DisplayObject("reflection.png",gr);
	}
	{
		TMultiGraph *hists=new TMultiGraph();
		TH1F* hist=hist_builder.getHist();
		double max=hist->GetMaximum();
		TGraph* experiment=new TGraph(hist);
		hists->Add(experiment);
		experiment->SetLineWidth(2);
		experiment->SetMarkerStyle(0);
		experiment->SetMarkerSize(0);
		DisplayTimeHists("Photon_Time.png",hists,max);
	}
	DisplayWavelengthHist(wavelength_cnt.getHist());
	delete photomult;delete scintillator;
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	return 0;
}

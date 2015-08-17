// this file is distributed under
// GPL v 3.0 license
#include <Model_routines/display_root_object.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/photoncounters.h>
int main(int , char **arg){
	std::mt19937 G;
	Printf(arg[0]);
	LongScintillator *scintillator=CreateIdealScintillator(0);
	double x[3];for(uint i=0; i<3;i++)x[i]=0;
	FirstPhotonsReg reg(scintillator,100);
	scintillator->RegisterLighting(x,50,G);
	double y[40],left[40],right[40];
	uint nl=0;uint nr=0;
	for(uint i=0;i<20;i++){
		y[i]=0;
		double t1=reg.LeftPhotonTime(i);
		if(std::isfinite(t1)){left[nl]=t1;nl++;}
		double t2=reg.RightPhotonTime(i);
		if(std::isfinite(t2)){right[nr]=t2+6;nr++;}
	}
	TMultiGraph *mgr=new TMultiGraph();
	TGraph *gr1=new TGraph(nl,left,y);
	mgr->Add(gr1);
	gr1->SetNameTitle("left","left");
	gr1->SetLineStyle(-1);
	gr1->SetMarkerStyle(kOpenTriangleUp);
	gr1->SetMarkerColor(kBlack);
	gr1->SetMarkerSize(2);
	TGraph *gr2=new TGraph(nl,right,y);
	mgr->Add(gr2);
	gr2->SetNameTitle("right","right");
	gr2->SetLineStyle(-1);
	gr2->SetMarkerStyle(kOpenCircle);
	gr2->SetMarkerColor(kBlack);
	gr2->SetMarkerSize(2);
	DisplayObject_plusplus("Fig_5b.png",[](TCanvas *,TMultiGraph* mgr){
		mgr->SetMinimum(-0.0);
		mgr->SetMaximum(0.0);
	},mgr,"AP",[](TCanvas *, TMultiGraph* mgr){
		mgr->GetYaxis()->SetTickLength(0);
		mgr->GetXaxis()->SetTitle("Photon registering time [ns]");
		mgr->GetXaxis()->SetTickLength(0.01);
		mgr->Draw("AP");
	},1000,800);
	Printf("DONE.");
	delete scintillator;
	return 0;
}

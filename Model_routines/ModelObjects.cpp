#include <QString>
#include <TCanvas.h>
#include <TGaxis.h>
#include <TPad.h>
#include <LongScintillator/scintillator2d.h>
#include <LongScintillator/scintillator3d.h>
#include <LongScintillator/scintillator_templates.h>
#include "ModelObjects.h"
#include "funcfromfile.h"
#include "displaygraph.h"
#include "display_root_object.h"
const QString emissioncurve="ModelData/BC-420_emission.txt";
const QString efficiencycurve="ModelData/tube_QE.txt";
const QString efficiencycurve2="ModelData/Si_PhM_QE.txt";
const QString absorbtioncurve="ModelData/absorption_coef_cm-1.txt";
FuncFromFile wl(emissioncurve);
FuncFromFile absor(absorbtioncurve,0,2);bool done_abs=false;
FuncFromFile eff(efficiencycurve);bool done_eff=true;//is in needed units already
FuncFromFile eff2(efficiencycurve2);bool done_eff2=false;
LongScintillator* CreateScintillator(double length){
	if(!done_abs){absor.MultiplyBy(0.18);done_abs=true;}//SCALING!!!
#define params double,double,double,ScinLightingParamsTypes
	return new ScintillatorWithAbsorption<Scintillator3D_rect,FuncFromFile,3,FuncFromFile,params>
			(absor,wl,wl.Min(),wl.Max(),0.01,length,scin_hwx,scin_hwy,scin_params);
#undef params
}
LongScintillator* CreateScintillator4Si(double length){
	if(!done_abs){absor.MultiplyBy(0.18);done_abs=true;}//SCALING!!!
#define params double,double,double,ScinLightingParamsTypes
	return new ScintillatorWithAbsorption<Scintillator3D_rect,FuncFromFile,3,FuncFromFile,params>
			(absor,wl,wl.Min(),wl.Max(),0.01,length,scin_hwx_si,scin_hwy_si,scin_params);
#undef params
}
LongScintillator* CreateIdealScintillator(double length){
	return new Scintillator3D_rect(length,scin_hwx,scin_hwy,scin_params);
}
class PhotoMultiplier:public virtual PhotoMultiplier_Efficiency1par<3,FuncFromFile>
		,public virtual PhotoMultiplierConstTimeRes{
public:
	PhotoMultiplier(IPhoton *source, FuncFromFile efficiency, double timeres)
		:AbstractPhotoMultiplier(source),
		  PhotoMultiplier_Efficiency1par<3,FuncFromFile>(source,efficiency),
		  PhotoMultiplierConstTimeRes(source,timeres){}
	virtual ~PhotoMultiplier(){}
protected:
	virtual double Efficiency(double* data)override{
		return PhotoMultiplier_Efficiency1par<3,FuncFromFile>::Efficiency(data);
	}
	virtual void SmearParameters(double* data)override{
		PhotoMultiplierConstTimeRes::SmearParameters(data);
	}
};

double ValueFirst(MultRowColF* master, int i){
	int phmx=master->count();
	int x=i%phmx;int y=i/phmx;
	return (*master)[x][y].TimeDifference(photon_index);
}
double ValueCnt(MultRowColC* master, int i2){
	int i=i2/2;
	int phmx=master->count();
	int x=i%phmx;int y=i/phmx;
	if(0==i2%2)return (*master)[x][y].LeftCount();
	else return (*master)[x][y].RightCount();
}

AbstractPhotoMultiplier* CreatePhotoMultiplier(IPhoton *source){
	if(!done_eff){eff.MultiplyBy(0.01);done_eff=true;}
	return new PhotoMultiplier(source,eff,phm_res_R);
}
AbstractPhotoMultiplier* CreateSiliconPhotoMultiplier(IPhoton *source){
	if(!done_eff2){eff2.MultiplyBy(0.01);done_eff2=true;}
	return new PhotoMultiplier(source,eff2,phm_res_si);
}
void DisplayPlots(){
	if(!done_eff){eff.MultiplyBy(0.01);done_eff=true;}
	if(!done_eff2){eff2.MultiplyBy(0.01);done_eff2=true;}
	TGraph* gr2=wl.Display("Emission");
	gr2->SetLineColor(kGray);
	gr2->SetMarkerColor(kGray);
	gr2->SetLineWidth(2);
	gr2->SetMarkerStyle(0);
	gr2->SetMarkerSize(0);
	TGaxis *sec_axis=new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
	sec_axis->SetLabelColor(kGray);
	sec_axis->SetTitle("Emission intencity [a.u.]");
	DisplayObject_plusplus("absorbtion.png",[](TCanvas* c1,TGraph* gr){
		TPad *pad1=new TPad("pad1","",0.05,0,1,1);
		pad1->Draw();pad1->cd();
		TH1F *th1f=c1->DrawFrame(xmin,0,xmax,0.01);
		th1f->GetXaxis()->SetTitle("Wavelength [nm]");
		th1f->GetYaxis()->SetTitle("Absorption coefficient [mm^{-1}]");
		th1f->GetXaxis()->SetLabelSize(0.05);
		th1f->GetXaxis()->SetTitleSize(0.05);
		th1f->GetYaxis()->SetLabelSize(0.05);
		th1f->GetYaxis()->SetTitleSize(0.05);
		th1f->GetYaxis()->SetTitleOffset(1.5);
		gr->SetLineWidth(2);
		gr->SetLineStyle(kDashed);
		gr->SetMarkerStyle(0);
		gr->SetMarkerSize(0);
	},absor.Display(""),"",[gr2,sec_axis](TCanvas*,TGraph*){
		TPad *pad2=new TPad("pad2","",0.1,0,1,1);
		pad2->SetFillStyle(4000);
		double dy = (ymax-ymin)/0.8;double dx = (xmax-xmin)/0.8;
		pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
		pad2->Draw();pad2->cd();
		gr2->Draw("LP");sec_axis->Draw();
	});
	TMultiGraph *mgr=new TMultiGraph();
	TGraph* gr0=eff.Display("R4998");
	mgr->Add(gr0);
	gr0->SetLineWidth(2);
	gr0->SetMarkerStyle(0);
	gr0->SetMarkerSize(0);
	TGraph* gr1=eff2.Display("Silicon");
	mgr->Add(gr1);
	gr1->SetLineWidth(2);
	gr1->SetLineStyle(kDashed);
	gr1->SetMarkerStyle(0);
	gr1->SetMarkerSize(0);
	DisplayObject_plusplus("displ_fn.png",[](TCanvas* c1,TMultiGraph* ){
		TPad *pad1=new TPad("pad1","",0,0,1,1);
		pad1->Draw();pad1->cd();
		TH1F *th1f=c1->DrawFrame(xmin,0,xmax,1);
		th1f->GetXaxis()->SetTitle("Wavelength [nm]");
		th1f->GetYaxis()->SetTitle("Quantum efficiency");
		th1f->GetXaxis()->SetLabelSize(0.05);
		th1f->GetXaxis()->SetTitleSize(0.05);
		th1f->GetYaxis()->SetLabelSize(0.05);
		th1f->GetYaxis()->SetTitleSize(0.05);
	},mgr,"",[gr2,sec_axis](TCanvas*,TMultiGraph*){
		TPad *pad2=new TPad("pad2","",0,0,1,1);
		pad2->SetFillStyle(4000);
		double dy = (ymax-ymin)/0.8;double dx = (xmax-xmin)/0.8;
		pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
		pad2->Draw();pad2->cd();
		gr2->Draw("LP");sec_axis->Draw();
	});
	delete gr2;delete sec_axis;
}
void DisplayWavelengthHist(TH1F *hist){
	double hist_max=hist->GetMaximum()*1.1;
	DisplayObject_plusplus("wavelength_reg.png",[hist_max](TCanvas* c1,TGraph* ){
		TPad *pad1=new TPad("pad1","",0,0,1,1);
		pad1->Draw();pad1->cd();
		TH1F *th1f=c1->DrawFrame(xmin,0,xmax,hist_max);
		th1f->GetXaxis()->SetTitle("Wavelength [nm]");
		th1f->GetYaxis()->SetTitle("Counts");
		th1f->GetYaxis()->SetTitleOffset(1.5);
		th1f->SetLineWidth(2);
		th1f->SetMarkerStyle(0);
		th1f->SetMarkerSize(0);
	},new TGraph(hist),"LP",[](TCanvas*,TGraph*){
		TGraph* gr2=wl.Display("Emission");
		gr2->SetLineColor(kGray);
		gr2->SetMarkerColor(kGray);
		gr2->SetLineWidth(2);
		gr2->SetMarkerStyle(0);
		gr2->SetMarkerSize(0);
		TGaxis *sec_axis=new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
		sec_axis->SetLabelColor(kGray);
		sec_axis->SetTitle("Emission intensity [a.u.]");
		TPad *pad2=new TPad("pad2","",0,0,1,1);
		pad2->SetFillStyle(4000);
		double dy = (ymax-ymin)/0.8;double dx = (xmax-xmin)/0.8;
		pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
		pad2->Draw();pad2->cd();
		gr2->Draw("LP");sec_axis->Draw();
	});
	delete hist;
}
void DisplayTimeHists(std::string name, TMultiGraph *hists, double maxval){
	TCanvas c1("c1","",800,600);
	TPad pad1("pad1","",0.05,0,1,1);
	pad1.Draw();
	pad1.cd();
	   // draw a frame to define the range
	TH1F *th1f=c1.DrawFrame(0,0,6,maxval*1.2);
	th1f->GetYaxis()->SetTitle("Counts");
	th1f->GetXaxis()->SetTitle("Time [ns]");
	th1f->GetXaxis()->SetLabelSize(0.05);
	th1f->GetXaxis()->SetTitleSize(0.05);
	th1f->GetYaxis()->SetLabelSize(0.05);
	th1f->GetYaxis()->SetTitleSize(0.05);
	th1f->GetYaxis()->SetTitleOffset(1.5);
	   // create first graph
	hists->Draw("LP");
	//make pad2 frame transparent
	c1.SaveAs(name.c_str());
	delete hists;
}

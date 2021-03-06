// this file is distributed under
// GPL v 3.0 license
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
const QString emissioncurve2="ModelData/E-J230_emission.txt";
const QString efficiencycurve="ModelData/tube_QE.txt";
const QString efficiencycurve2="ModelData/Si_PhM_QE.txt";
const QString absorbtioncurve="ModelData/absorption_coef_cm-1.txt";
const QString absorbtioncurve2="ModelData/EJ230_absorption_length.txt";
FuncFromFile wl(emissioncurve,1);
FuncFromFile wl2(emissioncurve2,1);
FuncFromFile absor(absorbtioncurve,0.18,0,2);
FuncFromFile attenuation(absorbtioncurve2,10);
double absor2(double wavelength){return 1.0/attenuation(wavelength);}
FuncFromFile eff(efficiencycurve,1);
FuncFromFile eff2(efficiencycurve2,0.01);
#define params double,double,double,ScinLightingParamsTypes
LongScintillator* CreateScintillatorEJ230(double length){
	return new AbsorptionCoef<Scintillator3D_rect,decltype(absor2)*,3,params>
		(&absor2,[](double w){return wl2(w);},wl2.Min(),wl2.Max(),201,length,scin_hwx,scin_hwy,scin_refr,lighting_params);
}
LongScintillator* CreateIdealScintillator(double length){
	return new Scintillator3D_rect(length,scin_hwx,scin_hwy,scin_refr,lighting_params);
}
LongScintillator* CreateScintillatorBC420(double length){
	return new AbsorptionCoef<Scintillator3D_rect,decltype(absor),3,params>
		(absor,[](double w){return wl(w);},wl.Min(),wl.Max(),201,length,scin_hwx,scin_hwy,scin_refr,lighting_params);
}
LongScintillator* CreateScintillatorBC420_4Si_matrix(double length){
	return new AbsorptionCoef<Scintillator3D_rect,FuncFromFile,3,params>
			(absor,[](double w){return wl(w);},wl.Min(),wl.Max(),201,length,scin_hwx_si,scin_hwy_si,scin_refr,lighting_params);
}
#undef params
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
	virtual void SmearParameters(double* data,std::mt19937&G)override{
		PhotoMultiplierConstTimeRes::SmearParameters(data,G);
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

AbstractPhotoMultiplier* CreateTubePhotoMultiplier(IPhoton *source){
	return new PhotoMultiplier(source,eff,phm_res_R);
}
AbstractPhotoMultiplier* CreateSiliconPhotoMultiplier(IPhoton *source){
	return new PhotoMultiplier(source,eff2,phm_res_si);
}
void DisplayPlots(){
	DisplayObject("test1.png",wl.Display("Emission"));
	{
	  TGraph* gr2=wl.Display("Emission");
	  gr2->SetLineColor(kBlack);
	  gr2->SetMarkerColor(kBlack);
	  gr2->SetLineWidth(1);
	  gr2->SetLineStyle(1);
	  gr2->SetMarkerStyle(0);
	  gr2->SetMarkerSize(0);
	  TGaxis *sec_axis=new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
	  sec_axis->SetLabelColor(kBlack);
	  sec_axis->SetLabelSize(0.03);
	  sec_axis->SetTitle("Emission intensity [a.u.]");
	  TMultiGraph *mgr=new TMultiGraph();
	  TGraph* gr0=absor.Display("");
	  mgr->Add(gr0);
	  gr0->SetLineWidth(4);
	  gr0->SetMarkerStyle(0);
	  gr0->SetMarkerSize(0);
	  DisplayObject_plusplus("absorbtion.png",[](TCanvas* c1,TMultiGraph* ){
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
	  },mgr,"",[gr2,sec_axis](TCanvas*,TMultiGraph*){
		  TPad *pad2=new TPad("pad2","",0,0,1,1);
		  pad2->SetFillStyle(4000);
		  double dy = (ymax-ymin)/0.8;double dx = (xmax-xmin)/0.8;
		  pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
		  pad2->Draw();pad2->cd();
		  gr2->Draw("LP");sec_axis->Draw();
	  });
	  delete gr2;delete sec_axis;
	 }{
	  TGraph* gr2=wl.Display("Emission");
	  gr2->SetLineColor(kBlack);
	  gr2->SetMarkerColor(kBlack);
	  gr2->SetLineWidth(1);
	  gr2->SetLineStyle(1);
	  gr2->SetMarkerStyle(0);
	  gr2->SetMarkerSize(0);
	  TGaxis *sec_axis=new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
	  sec_axis->SetLabelColor(kBlack);
	  sec_axis->SetLabelSize(0.03);
	  sec_axis->SetTitle("Emission intensity [a.u.]");
	  TMultiGraph *mgr=new TMultiGraph();
	  TGraph* gr0=eff.Display("R4998");
	  mgr->Add(gr0);
	  gr0->SetLineWidth(4);
	  gr0->SetMarkerStyle(0);
	  gr0->SetMarkerSize(0);
	  TGraph* gr1=eff2.Display("Silicon");
	  mgr->Add(gr1);
	  gr1->SetLineWidth(4);
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
	DisplayObject("test2.png",wl.Display("Emission"));
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
		th1f->SetLineWidth(4);
		th1f->SetMarkerStyle(0);
		th1f->SetMarkerSize(0);
	},new TGraph(hist),"LP",[](TCanvas*,TGraph*){
		TGraph* gr2=wl.Display("Emission");
		gr2->SetLineColor(kBlack);
		gr2->SetMarkerColor(kBlack);
		gr2->SetLineWidth(1);
		gr2->SetLineStyle(1);
		gr2->SetMarkerStyle(0);
		gr2->SetMarkerSize(0);
		TGaxis *sec_axis=new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
		sec_axis->SetLabelColor(kBlack);
		sec_axis->SetLabelSize(0.03);
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
	TH1F *th1f=c1.DrawFrame(0,0,20,maxval*1.2);
	th1f->GetYaxis()->SetTitle("Counts");
	th1f->GetXaxis()->SetTitle("Time [ns]");
	th1f->GetXaxis()->SetLabelSize(0.05);
	th1f->GetXaxis()->SetTitleSize(0.05);
	th1f->GetYaxis()->SetLabelSize(0.05);
	th1f->GetYaxis()->SetTitleSize(0.05);
	th1f->GetYaxis()->SetTitleOffset(1.5);
	hists->Draw("LP");
	c1.SaveAs(name.c_str());
	delete hists;
}

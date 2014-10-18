#include <TFrame.h>
#include <QApplication>
#include <QDir>
#include <TGaxis.h>
#include "displaygraph.h"
#include <Model_routines/display_root_object.h>
int file_no=0;
void displaygraph(TMultiGraph *gr, uint n, double *x, double *x_alt, std::string alt_axis_name, double *x_alt_2, std::string alt_axis_name_2){
	file_no++;
	QString fn="Graph_";
	fn+=QString::number(file_no);
	if(alt_axis_name!="")
		fn+="_"+QString::fromStdString(alt_axis_name);
	if(alt_axis_name_2!="")
		fn+="_"+QString::fromStdString(alt_axis_name_2);
	fn+=".png";
	fn=fn.replace(" ","_");
	DisplayObject_plus(fn.toStdString(),gr,"acp",[n,x,x_alt,alt_axis_name,x_alt_2,alt_axis_name_2](TMultiGraph* gr){
		gr->GetYaxis()->SetTitle("#sigma [ns]");
		gr->GetXaxis()->SetTitle("N");
		gr->GetXaxis()->SetLabelSize(0.05);
		gr->GetXaxis()->SetTitleSize(0.05);
		gr->GetYaxis()->SetLabelSize(0.05);
		gr->GetYaxis()->SetTitleSize(0.05);
		if((n>0)&&(x_alt!=NULL)){
			double y=gr->GetYaxis()->GetXmax();
			TGaxis *alt_axis = new TGaxis(x[0],y,x[n-1],y,x_alt[0],x_alt[n-1]);
			alt_axis->SetTitle(alt_axis_name.c_str());
			alt_axis->Draw();
			if(x_alt_2!=NULL){
				double y2=gr->GetYaxis()->GetXmin();
				y2=y+(y2-y)*0.15;
				TGaxis *alt_axis_2 = new TGaxis(x[0],y2,x[n-1],y2,x_alt_2[0],x_alt_2[n-1]);
				alt_axis_2->SetTitle(alt_axis_name_2.c_str());
				alt_axis_2->Draw();
			}
		}
	});
}
EMarkerStyle styles[]={kFullCircle,kFullTriangleUp,kFullSquare,kOpenCircle,kOpenTriangleUp, kOpenSquare};
TMultiGraph* MakeGraph(uint k, uint n, double* x, double **y, std::string name, std::string title){
	TMultiGraph* mgr=new TMultiGraph(name.c_str(),title.c_str());
	for(uint grn=0; grn<k;grn++){
		TGraph *gr=new TGraph(n, x, y[grn]);
		QString nm=QString::number(grn);
		mgr->Add(gr);
		gr->SetNameTitle(nm.toStdString().c_str(),nm.toStdString().c_str());
		gr->SetLineStyle(0);
		gr->SetLineWidth(0);
		gr->SetMarkerStyle(styles[grn]);
		gr->SetMarkerColor(kBlack);
		gr->SetMarkerSize(2);
	}
	return mgr;
}

PhotonHistogram::PhotonHistogram(IPhoton *source, uint parn)
	:PhotonReg(source){hist=NULL;counter=0;par_n=parn;}
PhotonHistogram::~PhotonHistogram(){
	if(hist!=NULL)delete hist;
}
void PhotonHistogram::StartHist(std::string title, int bins, double a, double b){
	if(hist!=NULL)delete hist;
	counter++;
	fn=QString::fromStdString(title)+QString("registered_photons_")+
			QString::number(counter)+"_param_"+QString::number(par_n);
	hist=new TH1F(fn.toStdString().c_str(),title.c_str(),bins,a,b);
	fn+=".png";
	fn=fn.replace(" ","_");
}
void PhotonHistogram::ReleaseHist(){
	DisplayObject(fn.toStdString(),hist);hist=NULL;
}
TH1F* PhotonHistogram::getHist(bool release){
	TH1F* res=hist;
	if(release)hist=NULL;
	return res;
}
void PhotonHistogram::Process(bool isRight, double *data){
	if((hist!=NULL)&&(!isRight))hist->Fill(data[par_n]);
}

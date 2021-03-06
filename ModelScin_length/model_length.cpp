// this file is distributed under
// GPL v 3.0 license
#include <TGraph.h>
#include <TH1F.h>
#include <TGaxis.h>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/display_root_object.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/scintillator3d.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/math_h/randomfunc.h>
double lengths[]={test_L};
Width_t widths[]={4,4,2,1};
EColor colors[]={kBlack,kBlack,kBlack,kBlack};
ELineStyle styles[]={kSolid,kDotted,kSolid,kDotted};
int main(int , char **arg){
	std::mt19937 G;
	std::uniform_real_distribution<double> posx(-scin_hwx,scin_hwx),posy(-scin_hwy,scin_hwy);
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	Scintillator3D_rect scintillator(lengths[test_L_k-1]+50,scin_hwx,scin_hwy,scin_refr,lighting_params);
	PhotonHistogram timehist(&scintillator);
	double max=0;
	TMultiGraph* distr=new TMultiGraph();
	for(uint index=0; index<test_L_k;index++){
		double coords[3];coords[0]=lengths[index];
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for L=%f (%i photons; %i events)",lengths[index],n_widm,events_number);
		timehist.StartHist("",400,0,20);
		for(uint cnt=0; cnt<events_number;cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			coords[1]=posx(G);	coords[2]=posy(G);
			scintillator.RegisterLighting(coords,n_widm,G);
		}
		Printf("SAVING FILE");
		QString name(arg[0]);
		QFile file(name+"."+QString::number(index)+".output.txt");
		file.open(QIODevice::WriteOnly);
		if(file.isOpen()){
			QTextStream str(&file);
			auto hist=timehist.getHist(false);
			for(int i=1,n=hist->GetNbinsX(); i<=n; i++)
				str<<hist->GetBinCenter(i)<<" "<<hist->GetBinContent(i)<<"\n";
			file.close();
		}
		Printf("SAVED.");
		double m=timehist.getHist(false)->GetMaximum();if(m>max)max=m;
		TGraph* graph=new TGraph(timehist.getHist());
		distr->Add(graph);
		graph->SetMarkerSize(0);
		graph->SetMarkerColor(colors[index]);
		graph->SetLineColor(colors[index]);
		graph->SetLineWidth(widths[index]);
		graph->SetLineStyle(styles[index]);
	}
	DisplayTimeHists("Fig_1_time_distr.png",distr,max);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	return 0;
}

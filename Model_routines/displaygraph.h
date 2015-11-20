// this file is distributed under
// GPL v 3.0 license
#ifndef DISPLAYGRAPH_H
#define DISPLAYGRAPH_H
#include <QObject>
#include <QString>
#include <TF1.h>
#include <TH1.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <LongScintillator/scintillator.h>

// ATTENTION! dangerous place
// Provide coefficient needed after publication recensing
// it's converting sigma->CRT
// 63 ps is the time difference smearing caused by detector geometry
// it corresponds to time of gamma-quantum flying throwgh detectors width
// Sory for providing this change such way :-(
enum CrutchType{No_crutch,Sigma2CRT,Sigma2CRT_plusCorrection};
const double Crutch_sigma2CRT=2.35/sqrt(2);
//63ps corresponds to time of gamma-quantum flight through detector width
const double Crutch_Correction=0.063;

void displaygraph(TMultiGraph* gr, uint n, double* x, CrutchType crutch
				, QString *legend=NULL
				, double* x_alt=NULL, std::string alt_axis_name=""
				, double* x_alt_2=NULL, std::string alt_axis_name_2=""
		);
TMultiGraph* MakeGraph(uint k, uint n, double* x, double **y, CrutchType crutch, std::string name, std::string title);
class PhotonHistogram:public PhotonReg{
	Q_OBJECT
public:
	PhotonHistogram(IPhoton *source, uint parn=0);
	virtual ~PhotonHistogram();
	void StartHist(std::string title, int bins, double a, double b);
	void ReleaseHist();
	TH1F* getHist(bool release=true);
protected:
	virtual void Process(bool isRight, double *data,std::mt19937&G)override;
private:
	TH1F* hist;
	uint counter;
	uint par_n;
	QString fn;
};
#endif // DISPLAYGRAPH_H

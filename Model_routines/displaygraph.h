#ifndef DISPLAYGRAPH_H
#define DISPLAYGRAPH_H
#include <QObject>
#include <QString>
#include <TF1.h>
#include <TH1.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <LongScintillator/scintillator.h>
void displaygraph(TMultiGraph* gr, uint n, double* x
				, double* x_alt=NULL, std::string alt_axis_name=""
				, double* x_alt_2=NULL, std::string alt_axis_name_2=""
		);
TMultiGraph* MakeGraph(uint k, uint n, double* x, double **y, std::string name, std::string title);
class PhotonHistogram:public PhotonReg{
	Q_OBJECT
public:
	PhotonHistogram(IPhoton *source, uint parn=0);
	virtual ~PhotonHistogram();
	void StartHist(std::string title, int bins, double a, double b);
	void ReleaseHist();
	TH1F* getHist(bool release=true);
protected:
	virtual void Process(bool isRight, double *data)override;
private:
	TH1F* hist;
	uint counter;
	uint par_n;
	QString fn;
};
#endif // DISPLAYGRAPH_H

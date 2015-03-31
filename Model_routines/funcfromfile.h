#ifndef FUNCFROMFILE_H
#define FUNCFROMFILE_H
#include <QString>
#include <LongScintillator/math_h/interpolate.h>
#include <TGraph.h>
class FuncFromFile{
public:
	FuncFromFile();
	FuncFromFile(FuncFromFile &f);
	FuncFromFile(QString name, uint xc=0, uint yc=1);
	virtual ~FuncFromFile();
	double operator()(double x);
	void MultiplyBy(double k);
	double Min();
	double Max();
	TGraph* Display(QString title);
private:
	LinearInterpolation<double> m_func;
	double K;
	QString Name;
};

#endif // FUNCFROMFILE_H

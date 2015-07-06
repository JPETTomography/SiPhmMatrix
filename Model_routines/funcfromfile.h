// this file is distributed under
// GPL v 3.0 license
#ifndef FUNCFROMFILE_H
#define FUNCFROMFILE_H
#include <QString>
#include <LongScintillator/math_h/interpolate.h>
#include <TGraph.h>
class FuncFromFile{
public:
	FuncFromFile();
	FuncFromFile(FuncFromFile &f);
	FuncFromFile(QString name, double k, uint xc=0, uint yc=1);
	virtual ~FuncFromFile();
	double operator()(double x);
	double Min();
	double Max();
	TGraph* Display(QString title);
private:
	LinearInterpolation<double> m_func;
	double K;
	QString Name;
};

#endif // FUNCFROMFILE_H

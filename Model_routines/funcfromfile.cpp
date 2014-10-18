#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "funcfromfile.h"
#include <LongScintillator/math_h/interpolate.h>
FuncFromFile::FuncFromFile(){K=1;Name="";}
FuncFromFile::FuncFromFile(FuncFromFile &f){
	for(int i=0; i<f.X.count();i++){
		X.append(f.X[i]);Y.append(f.Y[i]);
	}
	K=f.K;
	Name=f.Name;
}
FuncFromFile::FuncFromFile(QString name, uint xc, uint yc){
	Name=name;
	QFile file(Name);
	file.open(QIODevice::ReadOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		while(!str.atEnd()){
			QString line=str.readLine();
			QStringList cols=line.replace(',',' ').replace(';',' ').replace('\t',' ').split(' ');
			bool ok=false;
			double x=cols[xc].toDouble(&ok);
			double y=cols[yc].toDouble(&ok);
			int index=WhereToInsert(0,X.count()-1,X,x);
			X.insert(index,x);
			Y.insert(index,y);
		}
		file.close();
	}
	K=1;
}
FuncFromFile::~FuncFromFile(){}
void FuncFromFile::MultiplyBy(double k){K=k;}
double FuncFromFile::operator ()(double x){
	return Interpolate_Linear(0,X.count()-1,X,Y,x)*K;
}
double FuncFromFile::Min(){return X[0];}
double FuncFromFile::Max(){return X[X.count()-1];}
TGraph *FuncFromFile::Display(QString title){
	TGraph *gr=new TGraph(X.count());
	for(int i=0; i<X.count();i++)
		gr->SetPoint(i,X[i],(*this)(X[i]));
	gr->SetNameTitle(Name.toStdString().c_str(),title.toStdString().c_str());
	gr->SetLineWidth(2);
	gr->SetMarkerStyle(0);
	gr->SetMarkerSize(0);
	return gr;
}


// this file is distributed under
// GPL v 3.0 license
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <utility>
#include <LongScintillator/math_h/interpolate.h>
#include "funcfromfile.h"
FuncFromFile::FuncFromFile(){K=1;Name="";}
FuncFromFile::FuncFromFile(FuncFromFile &f){
	m_func=f.m_func;
	K=f.K;
	Name=f.Name;
}
FuncFromFile::FuncFromFile(QString name, double k, uint xc, uint yc){
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
			if(ok){
				double y=cols[yc].toDouble(&ok);
				if(ok)
					m_func<<std::make_pair(x,y);
			}
		}
		file.close();
	}else{
		throw;
	}
	K=k;
}
FuncFromFile::~FuncFromFile(){}
double FuncFromFile::operator ()(double x){
	return m_func(x)*K;
}
double FuncFromFile::Min(){return m_func[0].first;}
double FuncFromFile::Max(){return m_func[m_func.size()-1].first;}
TGraph *FuncFromFile::Display(QString title){
	TGraph *gr=new TGraph(m_func.size());
	int i=0;
	for(auto p:m_func){
		gr->SetPoint(i,p.first,p.second*K);
		i++;
	}
	gr->SetNameTitle(Name.toStdString().c_str(),title.toStdString().c_str());
	gr->SetLineWidth(2);
	gr->SetMarkerStyle(0);
	gr->SetMarkerSize(0);
	return gr;
}


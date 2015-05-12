#include <QFile>
#include <QTextStream>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/scintillator_templates.h>
#include <LongScintillator/math_h/randomfunc.h>
int main(int , char **arg){
	int n=int(scin_length/10);
	double n_reg[n+1];
	double dn_reg[n+1];
	double Length[n+1];
	double X_lighting[3];
	LongScintillator *scintillator=CreateScintillatorBC420();
	AbstractPhotoMultiplier *photomult=CreateTubePhotoMultiplier(scintillator);
	for(int i=0; i<=n;i++){
		X_lighting[0]=10.0*i;
		Printf("Position %f",X_lighting[0]);
		PhotonCounter counter(photomult);
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=RandomUniformlyR(-scin_hwx,scin_hwx);
			X_lighting[2]=RandomUniformlyR(-scin_hwy,scin_hwy);
			scintillator->RegisterLighting(X_lighting,3492);
		}
		Length[i]=X_lighting[0];
		n_reg[i]=counter.LeftAverage();
		dn_reg[i]=counter.LeftSigma();
	}
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<=n; i++){
			str<<Length[i]<<" "<<n_reg[i]<<" "<<dn_reg[i]<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	delete photomult;delete scintillator;
	return 0;
}

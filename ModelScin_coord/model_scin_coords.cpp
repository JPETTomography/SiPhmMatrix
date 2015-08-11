// this file is distributed under
// GPL v 3.0 license
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <Model_routines/displaygraph.h>
#include <Model_routines/funcfromfile.h>
#include <Model_routines/ModelObjects.h>
#include <Model_routines/display_root_object.h>
#include <LongScintillator/math_h/sigma.h>
const uint n_phm=phm_x*phm_y;
int main(int , char **arg){
	std::default_random_engine G;
	std::uniform_real_distribution<double> posx(-scin_hwx_si,scin_hwx_si),posy(-scin_hwy_si,scin_hwy_si);
	Printf(arg[0]);
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	double X_lighting[3];X_lighting[0]=scin_z;
	double n_ph[n];
	double n_ph_small[n];
	double n_ph_big[n];
	double* sig_time_diff[K+1];
		for(uint k=0; k<=K; k++)sig_time_diff[k]=new double[n];
	LongScintillator *scintillator=CreateScintillatorBC420_4Si_matrix();
	AbstractPhotoMultiplier *photomult=CreateSiliconPhotoMultiplier(scintillator);
	{
		MultRowColC cntphotons(photomult,ConstrParams(phm_x,phm_y,phm_dead));
		const uint photon_number=3410;
		for(uint ii=0;ii<events_number;ii++){
			X_lighting[1]=posx(G);X_lighting[2]=posy(G);
			scintillator->RegisterLighting(X_lighting,photon_number);
		}
		QFile file("average.per.phm.output.txt");
		file.open(QIODevice::WriteOnly);
		double ymax=0;
		if(file.isOpen()){
			QTextStream str(&file);
			for(int px=0;px<phm_x;px++)for(int py=0;py<phm_y;py++){
				str<<px*phm_y+py+1<<"\t"<<cntphotons[px][py].LeftAverage()<<"\t"<<cntphotons[px][py].LeftSigma()<<"\n";
				double max=cntphotons[px][py].LeftAverage()+2.0*cntphotons[px][py].LeftSigma();
				if(max>ymax)ymax=max;
			}
			file.close();
		}
		{QString script="Photon_number_distribution.gnuplot";
			QFile file(script);
			file.open(QFile::WriteOnly);
			if(file.isOpen()){
				QTextStream str(&file);
				str<<"set terminal pngcairo size 800,600 enhanced monochrome font 'Verdana,20'\n";
				str<<"set output 'Photon_number_distribution.png'\n";
				str<<"set xlabel 'Photomultiplier index'\n";
				str<<"set ylabel 'Average number of photoelectrons'\n";
				str<<"set xrange [0:"<<phm_x*phm_y+1<<"]\n";
				str<<"set yrange [0:"<<ymax<<"]\n";
				str<<"set pointsize 3 \n";
				str <<"plot ";
				str <<"\"average.per.phm.output.txt\" with points title ''";
				str<<"\n";
				file.close();
			}
			QProcess *gnuplot=new QProcess();
			gnuplot->startDetached("gnuplot",QStringList()<<script);
		}
	}
	LongScintillator *scintillator2=CreateScintillatorBC420_4Si_matrix();
	AbstractPhotoMultiplier *photomult2=CreateSiliconPhotoMultiplier(scintillator2);
	for(int index=0; index<n;index++ ){
		uint N_photons=(index*step)+scin_begin_si;
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		SortFirst first_phm(photomult,FirstConstrPar(phm_x,phm_y,phm_dead));
		MultRowColC cnt_photons(photomult,ConstrParams(phm_x,phm_y,phm_dead));
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=posx(G);X_lighting[2]=posy(G);
			scintillator->RegisterLighting(X_lighting,N_photons);
		}
		n_ph[index]=N_photons;
		WeightedAverageCalculator<double> Small;
		double Big=0;
		for(int px=0;px<phm_x;px++)for(int py=0;py<phm_y;py++){
			auto Phm=&(cnt_photons[px][py]);
			Small.AddValue(Phm->LeftAverage(),Phm->LeftSigma())
					.AddValue(Phm->LeftAverage(),Phm->LeftSigma());
			Big+=Phm->LeftAverage()+Phm->RightAverage();
		}
		n_ph_small[index]=Small.Average();
		n_ph_big[index]=Big;
		for(uint k=0; k<K;k++){
			sig_time_diff[k][index]=first_phm.getSigma(k);
			Printf("Sigma[%i][%i] = %f",k,index,sig_time_diff[k][index]);
		}
		Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
		Printf("Second Monte Carlo for N = %i (%i virtual experiments)...",N_photons, events_number);
		auto w_d=[&first_phm](int index){
			double ss=first_phm.getSigmaSqr(index);
			if((index>2)&&(ss>first_phm.getSigmaSqr(0)))return 0.0;
			if(0==std::isfinite(ss))return 0.0;
			return 1.0/ss;
		};
		auto s_d=[](SortFirst* master, int i){return master->operator[](i);};
		SignalObtainer<decltype(w_d),decltype(s_d),TemplateParams>
				signal_diff(photomult2,w_d,s_d,n_phm,FirstConstrPar(phm_x,phm_y,phm_dead));
		for(uint cnt=0;cnt<events_number; cnt++){
			if(0==((cnt+1)%1000))Printf("\texperiment number %i...",cnt+1);
			X_lighting[1]=posx(G);X_lighting[2]=posy(G);
			scintillator2->RegisterLighting(X_lighting,N_photons);
		}
		sig_time_diff[K][index]=signal_diff.ResolutionSignal();
		Printf("Sigma_gen[*][%i] = %f",index,sig_time_diff[K][index]);
	}

	Printf("CALCULATION FINISHED. DISPLAYING RESULTS");
	displaygraph(MakeGraph(K+1,n,n_ph,sig_time_diff,"diffgr",""),
				n,n_ph,NULL,n_ph_small,"average photons registered (one photomultiplier)",
				n_ph_big,"all photons registered");
	Printf("SAVING FILE");
	QString name(arg[0]);
	QFile file(name+".output.txt");
	file.open(QIODevice::WriteOnly);
	if(file.isOpen()){
		QTextStream str(&file);
		for(int i=0; i<n; i++){
			str<<n_ph[i]<<" "<<n_ph_small[i]<<" "<<n_ph_big[i];
			for(uint k=0;k<=K;k++)
				str<<" "<<sig_time_diff[k][i];
			str<<"\n";
		}
		file.close();
	}
	Printf("DONE.");
	Printf(QDateTime::currentDateTime().toString().toStdString().c_str());
	for(uint k=0; k<=K; k++)
			delete[] sig_time_diff[k];
	delete photomult;delete scintillator;
	delete photomult2;delete scintillator2;
	return 0;
}

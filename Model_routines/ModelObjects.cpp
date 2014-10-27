#include <QString>
#include <TCanvas.h>
#include <TGaxis.h>
#include <TPad.h>
#include <LongScintillator/scintillator2d.h>
#include <LongScintillator/scintillator3d.h>
#include <LongScintillator/scintillator_templates.h>
#include "ModelObjects.h"
#include "funcfromfile.h"
#include "displaygraph.h"
#include "display_root_object.h"
const QString emissioncurve="ModelData/BC-420_emission.txt";
const QString emissioncurve2="ModelData/E-J230_emission.txt";
const QString efficiencycurve="ModelData/tube_QE.txt";
const QString efficiencycurve2="ModelData/Si_PhM_QE.txt";
const QString absorbtioncurve="ModelData/absorption_coef_cm-1.txt";
const QString absorbtioncurve2="ModelData/EJ230_absorption_length.txt";
FuncFromFile wl(emissioncurve);FuncFromFile wl2(emissioncurve2);
FuncFromFile absor(absorbtioncurve,0,2);bool done_abs=false;
FuncFromFile attenuation(absorbtioncurve2);bool done_abs2=false;
double absor2(double wavelength){return 1.0/attenuation(wavelength);}
FuncFromFile eff(efficiencycurve);bool done_eff=true;//is in needed units already
FuncFromFile eff2(efficiencycurve2);bool done_eff2=false;
LongScintillator* CreateScintillatorEJ230(double length){
#define params double,double,double,ScinLightingParamsTypes
	if(!done_abs2){attenuation.MultiplyBy(10);done_abs2=true;}//SCALING!!!
	return new ScintillatorWithAbsorptionCoef<Scintillator3D_rect,decltype(absor2)*,3,FuncFromFile,params>
			(&absor2,wl2,wl2.Min(),wl2.Max(),0.01,length,scin_hwx,scin_hwy,scin_params);
#undef params
}
LongScintillator* CreateScintillatorBC420(double length){
	if(!done_abs){absor.MultiplyBy(0.18);done_abs=true;}//SCALING!!!
#define params double,double,double,ScinLightingParamsTypes
	return new ScintillatorWithAbsorptionCoef<Scintillator3D_rect,decltype(absor),3,FuncFromFile,params>
			(absor,wl,wl.Min(),wl.Max(),0.01,length,scin_hwx,scin_hwy,scin_params);
#undef params
}
LongScintillator* CreateScintillatorBC420_4Si_matrix(double length){
	if(!done_abs){absor.MultiplyBy(0.18);done_abs=true;}//SCALING!!!
#define params double,double,double,ScinLightingParamsTypes
	return new ScintillatorWithAbsorptionCoef<Scintillator3D_rect,FuncFromFile,3,FuncFromFile,params>
			(absor,wl,wl.Min(),wl.Max(),0.01,length,scin_hwx_si,scin_hwy_si,scin_params);
#undef params
}
class PhotoMultiplier:public virtual PhotoMultiplier_Efficiency1par<3,FuncFromFile>
		,public virtual PhotoMultiplierConstTimeRes{
public:
	PhotoMultiplier(IPhoton *source, FuncFromFile efficiency, double timeres)
		:AbstractPhotoMultiplier(source),
		  PhotoMultiplier_Efficiency1par<3,FuncFromFile>(source,efficiency),
		  PhotoMultiplierConstTimeRes(source,timeres){}
	virtual ~PhotoMultiplier(){}
protected:
	virtual double Efficiency(double* data)override{
		return PhotoMultiplier_Efficiency1par<3,FuncFromFile>::Efficiency(data);
	}
	virtual void SmearParameters(double* data)override{
		PhotoMultiplierConstTimeRes::SmearParameters(data);
	}
};

double ValueFirst(MultRowColF* master, int i){
	int phmx=master->count();
	int x=i%phmx;int y=i/phmx;
	return (*master)[x][y].TimeDifference(photon_index);
}
double ValueCnt(MultRowColC* master, int i2){
	int i=i2/2;
	int phmx=master->count();
	int x=i%phmx;int y=i/phmx;
	if(0==i2%2)return (*master)[x][y].LeftCount();
	else return (*master)[x][y].RightCount();
}

AbstractPhotoMultiplier* CreateTubePhotoMultiplier(IPhoton *source){
	if(!done_eff){eff.MultiplyBy(0.01);done_eff=true;}
	return new PhotoMultiplier(source,eff,phm_res_R);
}
AbstractPhotoMultiplier* CreateSiliconPhotoMultiplier(IPhoton *source){
	if(!done_eff2){eff2.MultiplyBy(0.01);done_eff2=true;}
	return new PhotoMultiplier(source,eff2,phm_res_si);
}

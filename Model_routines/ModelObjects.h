// this file is distributed under
// GPL v 3.0 license
#ifndef MODELOBJECTS_H
#define MODELOBJECTS_H
#include <TH1F.h>
#include <TMultiGraph.h>
#include "funcfromfile.h"
#include <LongScintillator/scintillator.h>
#include <LongScintillator/photomultiplier.h>
#include <LongScintillator/photoncounters.h>
#include <LongScintillator/scintillator_templates.h>
//SIMULATION CONFIGURATION
const int n = N_POINTS;
const int step=STEP;
const uint n_widm=WIDMO;
const uint K=3;
const uint this_K=75;
LongScintillator* CreateIdealScintillator(double length=scin_length);
LongScintillator* CreateScintillatorEJ230(double length=scin_length);
LongScintillator* CreateScintillatorBC420(double length=scin_length);
LongScintillator* CreateScintillatorBC420_4Si_matrix(double length=scin_length);
AbstractPhotoMultiplier* CreateTubePhotoMultiplier(IPhoton* source);
AbstractPhotoMultiplier* CreateSiliconPhotoMultiplier(IPhoton* source);
// SCINTILLATOR SIZE
#define scin_size_2d scin_length,scin_hwx
#define scin_size scin_length,scin_hwx,scin_hwy
// FOR PLOTS
const double ymin = 0;
const double ymax = 105;
const double xmin = 350;
const double xmax = 500;
extern FuncFromFile wl;
void DisplayPlots();
void DisplayWavelengthHist(TH1F* hist);
void DisplayTimeHists(std::string name,TMultiGraph* hists, double maxval);
//SILICON
#define templateparams uint, double, double, uint, double
typedef PhotonParamGroup<FirstPhotonsReg,uint> MultRowF;
typedef PhotonParamGroup<PhotonCounter> MultRowC;
typedef PhotonParamGroup<MultRowF,templateparams,uint> MultRowColF;
typedef PhotonParamGroup<MultRowC,templateparams> MultRowColC;
double ValueCnt(MultRowColC* master, int i2);
double ValueFirst(MultRowColF* master, int i);
typedef ValueSorter_and_SigmaObtainer<double (*)(MultRowColF*,int),MultRowColF,templateparams,templateparams,uint> SortFirst;
typedef ValueSorter_and_SigmaObtainer<double (*)(MultRowColC*,int),MultRowColC,templateparams,templateparams> SortCnt;
#define TemplateParams SortFirst,double (*)(MultRowColF*,int),int,templateparams,templateparams,uint
#define ConstrParams(X,Y,D) 1,-scin_hwx_si-D,scin_hwx_si+D, X,D ,2,-scin_hwy_si-D,scin_hwy_si+D,Y,D
#define CntConstrPar(X,Y,D) &ValueCnt,X*Y*2,ConstrParams(X,Y,D)
#define FirstConstrPar(X,Y,D) &ValueFirst,X*Y,ConstrParams(X,Y,D),photon_index+1
#endif // MODELOBJECTS_H

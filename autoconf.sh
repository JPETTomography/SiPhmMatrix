#/bin/bash
(git submodule init;git submodule update)
(cd LongScintillator;./autoconf.sh)
(cd FitDistrParam/FitGen; git submodule init; git submodule update)
echo ROOTDEPENDPATH = $ROOTSYS/lib$'\n'ROOTINCLUDEPATH = $ROOTSYS/include$'\n'DEFINES+=scin_refr=1.58$'\n'DEFINES+=N_POINTS=26$'\n'DEFINES+=STEP=200$'\n'DEFINES+=WIDMO=3000$'\n'DEFINES+=events_number=1000$'\n'DEFINES+=ideal_begin=1800$'\n'DEFINES+=phm_res_R=0.068$'\n'DEFINES+=phm_res_si=0.128$'\n'DEFINES+=scin_begin=1800$'\n'DEFINES+=scin_length=300.0$'\n'DEFINES+=scin_z=150.0$'\n'DEFINES+=scin_hwx=9.5$'\n'DEFINES+=scin_hwy=2.5$'\n'DEFINES+=scin_begin_si=1800$'\n'DEFINES+=scin_hwx_si=9.5$'\n'DEFINES+=scin_hwy_si=3.5$'\n'DEFINES+=phm_x=5$'\n'DEFINES+=phm_y=2$'\n'DEFINES+=phm_dead=0.5$'\n'DEFINES+=photon_index=0$'\n'DEFINES+=test_L_k=4$'\n'DEFINES+=test_L=0,300,500,1000$'\n'DEFINES+=ukl_N=1800$'\n'DEFINES+=ukl_k=6$'\n'DEFINES+=ukl_arr=2,1,3,1,5,2,5,2,7,2,7,3$'\n'DEFINES+=ukl_dead=0.0,0.0,0.0,0.5,0.0,0.0 > qt_build.pri
echo "COMPILING PREPARING APPLICATION..."
(cd FitDistrParam; cmake .;make)
echo "FITTING EMISSION TIME DISTRIBUTION PARAMETERS..."
(export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:FitGen;FitDistrParam/fitdistrparam_app > lighting.pri)
echo "parameters saved."

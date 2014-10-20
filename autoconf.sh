#/bin/bash
(git submodule init;git submodule update)
(cd LongScintillator;./autoconf.sh)
echo ROOTDEPENDPATH = /usr/lib64/root/$'\n'ROOTINCLUDEPATH = /usr/include/root/$'\n'DEFINES+=scin_params=1.58,0.005,0.2,1.5$'\n'DEFINES+=N_POINTS=26$'\n'DEFINES+=STEP=200$'\n'DEFINES+=WIDMO=3000$'\n'DEFINES+=events_number=1000$'\n'DEFINES+=phm_res_R=0.068$'\n'DEFINES+=phm_res_si=0.128$'\n'DEFINES+=scin_begin=1800$'\n'DEFINES+=scin_length=300.0$'\n'DEFINES+=scin_z=150.0$'\n'DEFINES+=scin_hwx=9.5$'\n'DEFINES+=scin_hwy=2.5$'\n'DEFINES+=scin_begin_si=1800$'\n'DEFINES+=scin_hwx_si=9.5$'\n'DEFINES+=scin_hwy_si=3.5$'\n'DEFINES+=phm_x=5$'\n'DEFINES+=phm_y=2$'\n'DEFINES+=phm_dead=0.5$'\n'DEFINES+=photon_index=0 > qt_build.pri

#/bin/bash
echo Running all programs in background every application in separate thread...
./ModelScin_ideal_app > run.ideal.log &
./ModelScin_length_app > run.length.log &
./ModelScin_sigma_length_app > run.sigma_l.log &
./ModelScin_sigma_app > run.sigma_n.log &
./ModelScin_app > run.r4998.log &
./ModelScin_coord_app > run.si.log &
./ModelScin_coord_2_app > run.si_2.log &
echo DONE!!!

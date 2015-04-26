#/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:LongScintillator:Model_routines
echo Running all programs
./ModelScin_ideal/ModelScin_ideal_app
./ModelScin_length/ModelScin_length_app
./ModelScin_sigma_l/ModelScin_sigma_length_app
./ModelScin_sigma_n/ModelScin_sigma_app
./ModelScin/ModelScin_app
./ModelScin_coord/ModelScin_coord_app
./ModelScin_coord_2/ModelScin_coord_2_app
./theory/theory_app
./Comparement/Comparement_app
./ModelScin_n/ModelScin_n_app
echo DONE!!!

#/bin/bash
echo "COMPILING PREPARING APPLICATION..."
(cd FitGen; cmake .;make clean all)
(cd FitDistrParam; qmake FitDistrParam.pro;make clean all)
echo "FITTING EMITION TIME DISTRIBUTION PARAMETERS..."
FitDistrParam/fitdistrparam_app > lighting.pri
echo "COMPILING..."
(cd LongScintillator;./buildme.sh)
if (( $? )); then exit 1; fi
(cd Model_routines;qmake model_routines.pro;make clean; make)
if (( $? )); then exit 1; fi
(cd ModelScin;qmake ModelScin.pro;make clean; make)
(cd ModelScin_coord;qmake ModelScin_coord.pro;make clean; make)
if [ $# -ne 1 ]; then
    echo "Binaries are not copied"
    exit
fi
echo "Copying the binaries to " $1
find -name *.so -exec cp '{}' $1 \;
find -name *.so.* -exec cp '{}' $1 \;
find -name *_app -exec cp '{}' $1 \;
find -name *_app.* -exec cp '{}' $1 \;
mkdir $1/ModelData
cp ModelData/* $1/ModelData
echo "done."
echo "Before running programs you may need to execute:"
echo "export LD_LIBRARY_PATH=."

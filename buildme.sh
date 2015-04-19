#/bin/bash
echo "COMPILING..."
(cd LongScintillator;./buildme.sh)
if (( $? )); then exit 1; fi
(cd Model_routines;qmake model_routines.pro;make clean; make)
if (( $? )); then exit 1; fi
(cd ModelScin;qmake ModelScin.pro;make clean; make)
(cd ModelScin_coord;qmake ModelScin_coord.pro;make clean; make)
(cd ModelScin_coord_2;qmake ModelScin_coord_2.pro;make clean; make)
(cd ModelScin_sigma_n;qmake ModelScin_sigma_n.pro;make clean; make)
(cd ModelScin_length;qmake ModelScin_length.pro;make clean; make)
(cd ModelScin_ideal;qmake ModelScin_ideal.pro;make clean; make)
(cd ModelScin_sigma_l;qmake ModelScin_sigma_l.pro;make clean; make)
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
cp run_sync.sh $1
cp run_async.sh $1
echo "done."
echo "Before running programs you may need to execute:"
echo "export LD_LIBRARY_PATH=."

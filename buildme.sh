#/bin/bash
echo "COMPILING..."
cd LongScintillator
./buildme.sh
if (( $? )); then exit 1; fi
cd ..
cd Model_routines
qmake model_routines.pro
if (( $? )); then exit 1; fi
make clean
make
if (( $? )); then exit 1; fi
cd ..
cd ModelScin
qmake ModelScin.pro
make clean
make
cd ..
cd ModelScin_coord
qmake ModelScin_coord.pro
make clean
make
cd ..
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

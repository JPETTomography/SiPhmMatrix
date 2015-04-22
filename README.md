J-PET Monte Carlo simulations
===========

required packages:
libqt4, libqt4-dev, cmake, make, ROOT

Run script "autoconf.sh" for creating all needed files.
Edit file qt_build.pri if needed (View it to see if it's needed!!!!)
I prefered to set events_number to 100000.
Then you may run "buildme.sh" for building. If you give it some path as 
a parameter, it will copy all binaries there.
If you give some path for" build.sh" it will copy all needed files
so that you can easily run the applications one by one manually
or use running scripts that also will be copied.

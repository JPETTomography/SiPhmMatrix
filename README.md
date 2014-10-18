J-PET Monte Carlo simulations
===========

required packages:
libqt4, libqt4-dev, make, ROOT (both libraries and develop package)

LongScintillator - library that implements simulation of emitted 
	photons movement in long scintillator with two 
	photomultipliers at the ends. (github)

ModelScin - application for simulation of scintillator with two
	photomultipliers.

ModelScin_coord - application that performs sumulation of scintillator
	with many small (3x3mm) photomultipliers.

Build requires libqt4 and libqt4-dev packages installed on your system.
Also it requires root installed.
The configuration and build can be done automatically.
Run script "autoconf.sh" for creating all needed files.
Edit file qt_build.pri if needed (View it to see if it's needed!!!!)
I prefered to set events_number to 100000.
Then you may run "buildme.sh" for building. If you give it some path as 
a parameter, it will copy all binaries there.

# Ssyft
Ssyft is a tool for the realizability of Safety LTL specifications.
You can either install Ssyft from scratch or use a Docker container (see below).


## Compilation Instructions for Syft using CMake

### Install CUDD

1. Make sure CUDD is installed. CUDD can be found at: https://github.com/KavrakiLab/cudd.git

2. Install CUDD:

	$ ./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=[install location]
	$ sudo make install

    If you get an error about aclocal, this might be due to either
    a. Not having automake:

	$ sudo apt-get install automake

    b. Needing to reconfigure, do this before configuring:

	$ autoreconf -i

### Install FLEX, BISON

3. Install flex and bison:
  
	$ sudo apt-get install flex bison

### Install Boost
4. Install Boost with

	$ sudo apt-get intall libboost-all-dev

### Install MONA

5. You probably want MONA if you are using Syft:
    
	$ sudo apt-get install mona

### Install Syfco
6. Install syfco:
	
	$ curl -sSL https://get.haskellstack.org/ | sh
	$ git clone https://github.com/reactive-systems/syfco.git
	$ stack install

### Install& Run SSyf

1. Compile the parser:

	$ cd parser/safe2fol
	$ make run

1. Compile SSyft using the makefile:

   	$ make

2. Run SSyft:

 	$ ./ssyft-run.sh [-smv | -tlsf] <filename> <workdir>	

3. Run tests:

  	$ cd test/smv
 	$ ./run-tests
 	$ cd ../test/tlsf
  	$ ./run-tests


## Docker container
It is possible to run Ssyft inside a Docker container.

      $ ssyft.sh [-smv | -tlsf] <filename>

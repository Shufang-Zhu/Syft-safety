Compilation Instructions for Syft using CMake

==== Install CUDD ====

0.1 Make sure CUDD is installed. CUDD can be found at:
    https://github.com/KavrakiLab/cudd.git

0.2 Install CUDD:
    ./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=[install location]
    sudo make install

    If you get an error about aclocal, this might be due to either
    a. Not having automake:
       sudo apt-get install automake
    b. Needing to reconfigure, do this before configuring:
       autoreconf -i

==== Install FLEX, BISON ====

0.3 Install flex and bison:
    sudo apt-get install flex bison

=== Install Boost ===
$ sudo apt-get intall libboost-all-dev

==== Install MONA ====

0.4 You probably want MONA if you are using Syft:
    sudo apt-get install mona

=== Install Syfco ===
curl -sSL https://get.haskellstack.org/ | sh
git clone https://github.com/reactive-systems/syfco.git
stack install

==== Install& Run SSyft ====

1. Compile the parser:

	cd parser/safe2fol
	make run

1. Compile SSyft using the makefile:

   make

2. Run SSyft:

  $ ./ssyft-run.sh [-smv | -tlsf] <filename> <workdir>	

3. Run tests:

  $ cd test/smv
  $ ./run-tests
  $ cd ../test/tlsf
  $ ./run-tests




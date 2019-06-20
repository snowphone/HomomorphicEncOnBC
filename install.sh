sudo apt install m4 -y

wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.bz2
tar -xvf ./gmp-6.1.2.tar.bz2 
cd ./gmp-6.1.2 
./configure && make && sudo make install && make check
cd ..

wget https://www.shoup.net/ntl/ntl-11.3.2.tar.gz
tar -xvf ntl-11.3.2.tar.gz
cd ./ntl-11.3.2/src
./configure NTL_GMP_LIP=on SHARED=on  TUNE=x86
make -j16
sudo make install
cd ../..

wget https://jaist.dl.sourceforge.net/project/arma/armadillo-9.500.2.tar.xz
tar -xvf ./armadillo-9.500.2.tar.xz 
cd ./armadillo-9.500.2 
./configure && make -j16 && sudo make install
cd ..

git clone https://github.com/homenc/HElib
cd HElib
mkdir build
cd build
cmake -ENABLE_ARMADILLO=on -BUILD_SHARED=on ..
make -j16
sudo make install
cd ..
cd ..

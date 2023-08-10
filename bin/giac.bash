#!/bin/bash
# This script is used to compile giac in a bash shell
#                    "${zdep_path}/png/libpng16.a"
#                    "${zdep_path}/zlib/libz.a"
#                    "${zdep_path}/giac/libsamplerate.a"
#                    "${zdep_path}/giac/libcocoa.a"
#                    "${zdep_path}/giac/libintl.a"
#                    "${zdep_path}/giac/libmpfr.a"
#                    "${zdep_path}/giac/libpari.a"
#                    "${zdep_path}/giac/libgmp.a"
#                    "${zdep_path}/giac/libgsl.a"
#                    "${zdep_path}/giac/libgiac.a"
GIAC_VERSION=1.9.0-41
#Strip off the -71
GIAC_VERSION_S=${GIAC_VERSION%-*}


# if install folder exists, delete it
if [ -d "install" ]; then
    rm -rf install
fi

# if install folder exists, delete it
if [ -d "linux" ]; then
    rm -rf linux
fi


# if install folder exists, delete it
if [ -d "working" ]; then
    rm -rf working
fi

# Create install directory if it doesn't exist
mkdir -p install

# Put current directory in var
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir -p working
mkdir -p download
cd download

# Download giac if giac_stable.tar.gz doesn't exist
if [ ! -f giac_stable.tar.gz ]; then
    echo "Downloading giac"
    curl -s https://www-fourier.ujf-grenoble.fr/~parisse/debian/dists/stable/main/source/giac_${GIAC_VERSION}.tar.gz -o giac_stable.tar.gz
fi
  # Extract giac
echo "Extracting giac"
tar -xzf giac_stable.tar.gz -C ../working

# Download gmp if gmp.tar.lz doesn't exist
if [ ! -f gmp.tar.lz ]; then
    echo "Downloading gmp"
    curl -s https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz -o gmp.tar.lz
fi
# Extract gmp
echo "Extracting gmp"
tar --lzip -xf gmp.tar.lz -C ../working

#Download mpfr if mpfr.tar.gz doesn't exist
if [ ! -f mpfr.tar.xz ]; then
  echo "Downloading mpfr"
  curl -s https://www.mpfr.org/mpfr-current/mpfr-4.2.0.tar.xz -o mpfr.tar.xz
fi
# Extract mpfr
echo "Extracting mpfr"
tar -xf mpfr.tar.xz -C ../working

# Download NTL if NTL.tar.gz doesn't exist
if [ ! -f NTL.tar.gz ]; then
  echo "Downloading NTL"
  curl -s https://libntl.org/ntl-11.5.1.tar.gz -o NTL.tar.gz
fi
# Extract NTL
echo "Extracting NTL"
tar -xf NTL.tar.gz -C ../working

# Download pari if pari.tar.gz doesn't exist
if [ ! -f pari.tar.gz ]; then
  echo "Downloading pari"
    curl -s http://pari.math.u-bordeaux.fr/pub/pari/unix/pari-2.15.3.tar.gz -o pari.tar.gz
fi
# Extract pari
echo "Extracting pari"
tar -xf pari.tar.gz -C ../working

# Download gsl if gsl.tar.gz doesn't exist
if [ ! -f gsl.tar.gz ]; then
  echo "Downloading gsl"
    curl -s https://mirror.ibcp.fr/pub/gnu/gsl/gsl-2.7.tar.gz -o gsl.tar.gz
fi
# Extract gsl
echo "Extracting gsl"
tar -xf gsl.tar.gz -C ../working

# Download mpfi if mpfi.tar.gz doesn't exist
if [ ! -f mpfi.tar.gz ]; then
  echo "Downloading mpfi"
    curl -s http://perso.ens-lyon.fr/nathalie.revol/softwares/mpfi-1.5.4.tar.bz2 -o mpfi.tar.bz2
fi
# Extract mpfi
echo "Extracting mpfi"
tar -xf mpfi.tar.bz2 -C ../working

#############################

cd ../working

######## MPFI ########
cd mpfi-1.5.4
echo "Configuring mpfi"
./configure --enable-static --disable-rpath --prefix=${DIR}/install/mpfi/bin --libdir=${DIR}/install/mpfi/lib > /dev/null 2>&1
echo "Compiling mpfi..."
make -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ..

######## GMP ########
cd gmp-6.2.1
echo "Configuring gmp"
./configure --enable-static --prefix=${DIR}/install/gmp/bin --libdir=${DIR}/install/gmp/lib > /dev/null 2>&1
echo "Compiling gmp..."
make -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ..

######## MPFR ########
cd mpfr-4.2.0
echo "Configuring mpfr"
./configure --enable-static --disable-rpath --prefix=${DIR}/install/mpfr/bin --libdir=${DIR}/install/mpfr/lib --disable-rpath > /dev/null 2>&1
echo "Compiling mpfr..."
make -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ..

######## GSL ########
cd gsl-2.7
echo "Configuring gsl"
./configure --enable-static --prefix=${DIR}/install/gsl/bin --libdir=${DIR}/install/gsl/lib --disable-rpath  > /dev/null 2>&1
echo "Compiling gsl..."
make -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ..

######## NTL ########
cd ntl-11.5.1/src
echo "Configuring ntl"
./configure DEF_PREFIX=${DIR}/install/ntl/bin PREFIX=${DIR}/install/ntl/lib SHARED=off > /dev/null 2>&1
echo "Compiling ntl..."
make -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ../..

######## PARI ########
cd pari-2.15.3
echo "Configuring pari"
./Configure -s --bindir=${DIR}/install/pari/bin --libdir=${DIR}/install/pari/lib --includedir=${DIR}/install/pari/bin/include > /dev/null 2>&1
echo "Compiling pari..."
make all -j32 -s > /dev/null 2>&1
make install -s > /dev/null 2>&1
cd ..

cd giac-${GIAC_VERSION_S}
echo "Configuring giac"
./configure --disable-micropy --disable-shared --disable-fltk --disable-gui --disable-cocoa --disable-gsl --disable-png --disable-rpath --enable-static --prefix=${DIR}/install/giac/bin --libdir=${DIR}/install/giac/lib > /dev/null 2>&1
echo "Compiling giac..."
make -j32 -s > /dev/null
make install -s > /dev/null 2>&1
cd ..

# Into main directory
cd ..
mkdir linux

mkdir -p ./linux/giac
cp ./install/giac/lib/libgiac.a ./linux/giac/
cp -r ./install/giac/bin/include ./linux/giac/
cd ./linux/giac
zip -r giac.zip libgiac.a
rm libgiac.a
cd ../..

mkdir -p ./linux/gmp
cp ./install/gmp/lib/libgmp.a ./linux/gmp/
cp -r ./install/gmp/bin/include ./linux/gmp/

mkdir -p ./linux/mpfr
cp ./install/mpfr/lib/libmpfr.a ./linux/mpfr/
cp -r ./install/mpfr/bin/include ./linux/mpfr/

mkdir -p ./linux/ntl
cp ./install/ntl/lib/lib/libntl.a ./linux/ntl/
cp -r ./install/ntl/lib/include ./linux/ntl/

mkdir -p ./linux/pari
cp ./install/pari/lib/libpari.a ./linux/pari/
cp -r ./install/pari/bin/include ./linux/pari/

mkdir -p ./linux/gsl
cp ./install/gsl/lib/libgsl.a ./linux/gsl/
cp -r ./install/gsl/bin/include ./linux/gsl/

mkdir -p ./linux/mpfi
cp ./install/mpfi/lib/libmpfi.a ./linux/mpfi/
cp -r ./install/mpfi/bin/include ./linux/mpfi/

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/giac/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/giac/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/gmp/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/gmp/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/gsl/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/gsl/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/mpfr/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/mpfr/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/ntl/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/ntl/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/pari/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/pari/
fi

# if install folder exists, delete it
if [ -d "/home/lukas/Desktop/zpath-deps/linux/mpfi/" ]; then
    rm -rf /home/lukas/Desktop/zpath-deps/linux/mpfi/
fi

cp -r ./linux/* /home/lukas/Desktop/zpath-deps/linux/
# Remove working directory
#rm -rf ${DIR}/working

cd /home/lukas/Desktop/zpath-deps/

#check if git status has changes
if [[ -n $(git status --porcelain) ]]; then
    echo "Changes detected, committing..."
    git add .
    git commit -m "Update linux dependencies"
    echo "Should I push? (y/n)"
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then
        git push
    fi
else
    echo "No changes detected, exiting..."
fi
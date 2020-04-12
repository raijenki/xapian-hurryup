# Proper script to install Xapian on Tailbench, so this can work
INSTALL_HOME=/home/cc
XAPIAN_HOME=$INSTALL_HOME/tailbench-v0.9/xapian
JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64

# Dependencies
sudo apt-get -y install openjdk-8-jdk uuid-dev zlib1g-dev bison pngcrush
pip install numpy
pip install scipy

# Download Tailbench and clean after extracting
cd $INSTALL_HOME
wget http://tailbench.csail.mit.edu/tailbench-v0.9.tgz -P $INSTALL_HOME/
wget http://tailbench.csail.mit.edu/tailbench.inputs.tgz -P $INSTALL_HOME/
tar xvf $INSTALL_HOME/tailbench-v0.9.tgz
tar xvf $INSTALL_HOME/tailbench.inputs.tgz
rm tailbench-v0.9.tgz
rm tailbench.inputs.tgz

# Change configuration files
cd $INSTALL_HOME/tailbench-v0.9
rm configs.sh
rm Makefile.config
mkdir scratch
echo 'DATA_ROOT=$INSTALL_HOME/tailbench.inputs' >> configs.sh
echo 'JDK_PATH=$JAVA_HOME' >> configs.sh
echo 'JDK_PATH=$JAVA_HOME' >> Makefile.config
echo 'SCRATCH_DIR=$INSTALL_HOME/scratch' >> configs.sh

# Build harness
cd harness
make -j16

# Install Xapian 1.2.25, 1.2.13 is garbage and doesn't work
cd $XAPIAN_HOME
rm xapian-core-1.2.13.tar.xz
wget https://oligarchy.co.uk/xapian/1.2.25/xapian-core-1.2.25.tar.xz
tar xvf xapian-core-1.2.25.tar.xz
cd $XAPIAN_HOME/xapian-core-1.2.25
mkdir install
./configure --prefix=$PWD/install
sudo make -j16
sudo make install

# Copy libxapian to Linux's library folder
cd $XAPIAN_HOME/xapian-core-1.2.25/install/lib
sudo cp libxapian.so.22.7.3 /usr/lib
sudo ldconfig -v

# Build Tailbench's Xapian
cd $XAPIAN_HOME
sudo sed -i 's/xapian-core-1.2.13/xapian-core-1.2.25/g' Makefile
make 

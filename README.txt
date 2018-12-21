This is an IMPERFECT log of the commands that I entered on each Pi (unless specified otherwise) within the Beowulf cluster. 
Additional software is needed, such as Windows Image Writer to flash Raspbian to each Pi after the master has been created,
and DHCP for Windows.  I found it necessary to use my Windows machine as a DHCP server, and hardline the cluster to it, because 
if the IP addresses change then MPI won't work.. and it's a nightmare to change all the IP addresses in the cluster once you set them. 


ssh pi@IP_ADDRESS
password: password

**on home wifi**
rp1 192.168.0.21
rp2 192.168.0.22
rp3 192.168.0.23
rp4 192.168.0.24
****************

**on switch with dhcpsrv running**
rp1 192.168.100.11
rp2 192.168.100.12
rp3 192.168.100.13
rp4 192.168.100.14
**********************************

**First burn Raspbian OS to one SD card**
**then**
sudo apt-get update
sudo mkdir mpich2
cd ~/mpich2
sudo wget http://www.mpich.org/static/downloads/3.2/mpich-3.2.tar.gz
sudo tar xfz mpich-3.2.tar.gz
sudo mkdir /home/rpimpi/
sudo mkdir /home/rpimpi/mpi-install
mkdir /home/pi/mpi-build
sudo apt-get install gfortran
sudo /home/pi/mpich2
sudo /home/pi/mpich2/mpich-3.2/configure -prefix=/home/rpimpi/mpi-install
sudo make
sudo make install
cd ..
sudo apt-get install vim
export PATH=$PATH:/home/rpimpi/mpi-install/bin
vim .profile
 *pasted in .bashrc* PATH=$PATH:/home/rpimpi/mpi-install/bin
sudo reboot
mpiexec -n 1 hostname
wget http://bitbucket.org/mpi4py/mpi4py/downloads/mpi4py-2.0.0.tar.gz
tar -zxf mpi4py-2.0.0.tar.gz
cd mpi4py-2.0.0
sudo aptitude install python-dev
python setup.py build
sudo python setup.py install
export PYTHONPATH=/home/pi/mpi4py-2.0.0

**Now copy this image to all other SD cards**
**then on master RP**
sudo apt-get update
sudo apt-get install nmap
ifconfig //this prints current ip
sudo nmap -sn 192.168.100.* //this prints all devices on network
mpiexec -n 1 hostname
mpiexec -f machinefile -n 4 hostname

--Adding keys to all pis--
-First:
ssh-keygen
cd~
cd .ssh
cp id_rsa.pub pi1
-Second: 
(for each pi)
ssh into the pi
ssh-keygen
cd .ssh
cp id_rsa.pub pi#

**do this to all pis** (should learn the for loop)
scp 192.168.100.11:/home/pi/.ssh/pi1 . 
cat pi1 >> authorized_keys
scp 192.168.100.12:/home/pi/.ssh/pi2 .
cat pi2 >> authorized_keys
scp 192.168.100.13:/home/pi/.ssh/pi3 .
cat pi3 >> authorized_keys
scp 192.168.100.14:/home/pi/.ssh/pi4 .
cat pi4 >> authorized_keys

vim test.py:
-----------------------------------------------
def is_prime(n):
        status = True
        if n < 2:
                status = False
        else:
                for i in range(2,n):
                        if n % i ==0:
                                status = False
        return status
for n in range(1,101):
        if is_prime(n):
                if n==97:
                        print n;
                else:
                        print n,",",
-------------------------------------------------
--for each pi after first--
scp -r test.py 192.168.100.11:/home/pi


mpiexec -f machinefile -n 5 python test5.py
mpiexec -f machinefile -n 12 ~/mpich2/examples/cpi

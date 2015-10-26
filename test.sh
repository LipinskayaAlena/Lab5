sudo rmmod timer
make
sudo insmod ./timer.ko
sudo echo "2" > /sys/hello/HelloWorld


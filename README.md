请先安装netmap,并于每次编译前加载netmap驱动
https://gitee.com/mirrors/netmap.git
./configure
make && make install
insmod netmap.ko

gcc -o UserProtocol UserProtocol.c  -I /你的netmap源码目录
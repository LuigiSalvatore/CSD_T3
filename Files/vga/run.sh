# module load riscv64-elf/14.1.0
cd ../..
make clean
make vga_ball F_CLK=50000000
make load SERIAL_DEV=/dev/ttyUSB1
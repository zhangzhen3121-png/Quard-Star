SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
$SHELL_FOLDER/qemu-8.0.2/output/qemu/bin/qemu-system-riscv64 \
	-M quard-star \
	-m 1G\
	-smp 8 \

# 获取当前脚本文件所在的目录
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# $0 获取当前脚本路径
# dirname $0 获取当前脚本路径的目录路径
cd qemu-8.0.2
if [ ! -d "$SHELL_FOLDER/qemu-8.0.2/output/qemu" ];then
./configure --prefix="$SHELL_FOLDER/qemu-8.0.2/output/qemu" --target-list=riscv64-softmmu --enable-gtk
fi
make -j8
sudo make install
cd ..


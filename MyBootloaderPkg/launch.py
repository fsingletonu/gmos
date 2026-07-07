import os
import subprocess
import shutil
import re

home_dir = os.path.expanduser("~")
workspace = os.path.join(home_dir, "桌面/edk2source")
loader_path = os.path.join(workspace, "MyBootloaderPkg")
kernel_src = os.path.join(workspace, "KernelPkg")

boot_folder = os.path.join(home_dir, "Launch/EFI/BOOT")
kernel_dst = os.path.join(home_dir, "Launch/EFI/KERNEL")

# ==================== 第一步：编译内核 ====================

print("Step 1: Building Kernel...")
print("make clean...")
subprocess.run(["make", "clean", "-C", kernel_src], check=True)

print("make mkbin...")
subprocess.run(["make", "mkbin", "-C", kernel_src], check=True)

# ==================== 第二步：获取 system.bin 大小 ====================

print("\nStep 2: Getting system.bin size...")
bin_path = os.path.join(kernel_src, "Build/system.bin")
bin_size = os.path.getsize(bin_path)
print(f"  system.bin = {bin_size} bytes (0x{bin_size:x})")
bin_size = (bin_size + 0xFFF) & (~0xFFF)
print(f"  rounding system.bin = {bin_size} bytes (0x{bin_size:x})")

# ==================== 第三步：更新 KERNELSIZE + 编译 Bootloader ====================

print("\nStep 3: Updating KERNELSIZE and building Bootloader...")
header_path = os.path.join(loader_path, "Include/BootInfo.h")

with open(header_path, "r", encoding="utf-8") as f:
    content = f.read()

content = re.sub(
    r"#define KERNELSIZE .*", f"#define KERNELSIZE 0x{bin_size:x}", content
)

with open(header_path, "w") as f:
    f.write(content)

os.chdir(workspace)
subprocess.run(
    [
        "bash",
        "-c",
        f"cd {workspace} && . edksetup.sh && build -p MyBootloaderPkg/MyBootloaderPkg.dsc -a X64 -b DEBUG",
    ],
    check=True,
)

# ==================== 第四步：部署 ====================

print("Step 4: Deploying...")
os.makedirs(boot_folder, exist_ok=True)
os.makedirs(kernel_dst, exist_ok=True)

shutil.copy2(
    os.path.join(workspace, "Build/MyBootloaderPkg/DEBUG_GCC/X64/MyBootloader.efi"),
    os.path.join(boot_folder, "BOOTX64.EFI"),
)
shutil.copy2(bin_path, kernel_dst)

# qemu-system-x86_64 -bios /home/fu/桌面/edk2source/Build/OvmfX64/DEBUG_GCC/FV/OVMF.fd -drive format=raw,file=fat:rw:/home/fu/Launch -net none -serial file:memmap.log -m 256M -machine q35
# qemu-system-x86_64 -s -S -bios /home/fu/桌面/edk2source/Build/OvmfX64/DEBUG_GCC/FV/OVMF.fd -drive format=raw,file=fat:rw:/home/fu/Launch -net none -serial file:memmap.log -m 256M -machine q35
# qemu-system-x86_64 -s -S -bios /home/fu/桌面/edk2source/Build/OvmfX64/DEBUG_GCC/FV/OVMF.fd -drive format=raw,file=fat:rw:/home/fu/Launch -net none -serial file:memmap.log -m 256M -machine pc
# qemu-system-x86_64 -s -S -bios /home/fu/桌面/edk2source/Build/OvmfX64/DEBUG_GCC/FV/OVMF.fd -drive format=raw,file=fat:rw:/home/fu/Launch -net none -serial file:memmap.log -m 256M -machine pc -smp 4
# -device ps2-keyboard
# qemu-system-x86_64 -s -S -bios /home/fu/桌面/edk2source/Build/OvmfX64/DEBUG_GCC/FV/OVMF.fd -net none -serial file:memmap.log -m 256M -machine pc -smp 4 -drive file=disk.img,format=raw,if=ide,index=0,media=disk -drive file=fat:rw:/home/fu/Launch,format=raw,if=ide,index=1,media=disk -monitor vc
# qemu-system-x86_64 -s -S -bios /home/fu/桌面/edk2source/KernelPkg/OVMF/x64/old_OVMF.fd -machine pc -m 256M -smp 4 -drive file=fat:rw:/home/fu/Launch,format=raw,if=ide,id=boot_disk -drive file=disk.img,format=raw,if=ide,id=main_disk -monitor vc -net none -serial file:memmap.log
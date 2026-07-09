#dir set
BUILD = Build

# head set
INCLUDE:= -I Include

# tool-flags set
CFLAGS:= -m64
CFLAGS+= -O0
CFLAGS+= -g3
CFLAGS+= -gdwarf-4
CFLAGS+= -fno-omit-frame-pointer
CFLAGS+= -fno-optimize-sibling-calls
CFLAGS+= -fno-builtin
# CFLAGS+= --mcmodel=kernel
CFLAGS+= -mcmodel=large
CFLAGS+= -mno-red-zone
CFLAGS+= -nostdinc
CFLAGS+= -fno-pic
CFLAGS+= -fno-pie
CFLAGS+= -nostdlib
CFLAGS+= -fno-stack-protector
CFLAGS+= -mgeneral-regs-only
CFLAGS:=$(strip $(CFLAGS))

DEBUG:= -g

$(BUILD)/%.o : %.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf64 $(DEBUG) $< -o $@

$(BUILD)/%.o : %.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/core/%.o : /core/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/drivers/blk_drv/%.o : /drivers/blk_drv/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf64 $(DEBUG) $< -o $@

$(BUILD)/drivers/blk_drv/%.o : /drivers/blk_drv/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/drivers/chr_drv/%.o : /drivers/chr_drv/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf64 $(DEBUG) $< -o $@

$(BUILD)/drivers/chr_drv/%.o : /drivers/chr_drv/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/drivers/video/%.o : /drivers/video/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/fs/fat32/%.o : /fs/fat32/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf64 $(DEBUG) $< -o $@

$(BUILD)/fs/fat32/%.o : /fs/fat32/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/init/%.o : /init/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf64 $(DEBUG) $< -o $@

$(BUILD)/init/%.o : /init/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/lib/%.o : /lib/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/mm/%.o : /mm/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/x86_64/%.o : /x86_64/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel.bin :  $(BUILD)/start.o \
	$(BUILD)/main.o \
	$(BUILD)/core/smp.o \
	$(BUILD)/drivers/blk_drv/disk.o \
	$(BUILD)/drivers/chr_drv/iox64.o \
	$(BUILD)/drivers/chr_drv/iox86.o \
	$(BUILD)/drivers/chr_drv/keyboard.o \
	$(BUILD)/drivers/video/VGAVideo.o \
	$(BUILD)/fs/fat32/fat32_dir.o \
	$(BUILD)/fs/fat32/fat32_fat.o \
	$(BUILD)/fs/fat32/fat32_file.o \
	$(BUILD)/fs/fat32/fat32_mount.o \
	$(BUILD)/fs/fat32/fat32_ops.o \
	$(BUILD)/init/acpi.o \
	$(BUILD)/init/apic.o \
	$(BUILD)/init/idt.o \
	$(BUILD)/init/pic.o \
	$(BUILD)/init/x86_isr.o \
	$(BUILD)/lib/math.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/mm/early_alloc.o \
	$(BUILD)/mm/numa.o \
	$(BUILD)/mm/page.o \
	$(BUILD)/mm/slab.o \
	$(BUILD)/mm/zone.o \
	$(BUILD)/x86_64/asm.o \
	$(BUILD)/x86_64/desc.o \
	$(BUILD)/x86_64/tss.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_x86_64 -static -T kernel.ld $^ -o $@ 

$(BUILD)/system.bin : $(BUILD)/kernel.bin
	objcopy -O binary $< $@

$(BUILD)/system.map : $(BUILD)/kernel.bin
	nm $< | sort > $@

.PHONY: mkbin
mkbin : $(BUILD)/system.bin \
	$(BUILD)/system.map
	@echo "imgmk successfully"

.PHONY: clean
clean:
	rm -rf Build
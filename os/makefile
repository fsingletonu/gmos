# dir set
BUILD = build
SRC = src

# head set
INCLUDE:= -I $(SRC)/include

# tool-flags set
CFLAGS:= -m32
CFLAGS+= -fno-builtin
CFLAGS+= -nostdinc
CFLAGS+= -fno-pic
CFLAGS+= -fno-pie
CFLAGS+= -nostdlib
CFLAGS+= -fno-stack-protector
CFLAGS:=$(strip $(CFLAGS))

DEBUG:= -g

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/%.o: $(SRC)/kernel/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/blk_drv/%.o: $(SRC)/kernel/blk_drv/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/blk_drv/%.o: $(SRC)/kernel/blk_drv/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/chr_drv/%.o: $(SRC)/kernel/chr_drv/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/chr_drv/%.o: $(SRC)/kernel/chr_drv/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/fs/%.o: $(SRC)/kernel/fs/ext/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/fs/%.o: $(SRC)/kernel/fs/ext/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/lib/%.o: $(SRC)/kernel/lib/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/lib/%.o: $(SRC)/kernel/lib/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/mm/%.o: $(SRC)/kernel/mm/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/mm/%.o: $(SRC)/kernel/mm/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/sched/%.o: $(SRC)/kernel/sched/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/sched/%.o: $(SRC)/kernel/sched/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/sys/%.o: $(SRC)/kernel/sys/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/sys/%.o: $(SRC)/kernel/sys/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/usr/%.o: $(SRC)/kernel/usr/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/usr/%.o: $(SRC)/kernel/usr/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel/init/%.o: $(SRC)/kernel/init/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/init/%.o: $(SRC)/kernel/init/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel.bin: $(BUILD)/kernel/blk_drv/dma.o \
	$(BUILD)/kernel/chr_drv/console.o \
	$(BUILD)/kernel/chr_drv/io.o \
	$(BUILD)/kernel/chr_drv/keyboard.o \
	$(BUILD)/kernel/fs/ext/super_block.o \
	$(BUILD)/kernel/init/acpi.o \
	$(BUILD)/kernel/init/ards.o \
	$(BUILD)/kernel/init/desc.o \
	$(BUILD)/kernel/init/handler.o \
	$(BUILD)/kernel/init/i386.o \
	$(BUILD)/kernel/init/ide.o \
	$(BUILD)/kernel/init/interrupt.o \
	$(BUILD)/kernel/init/panic.o \
	$(BUILD)/kernel/init/pci.o \
	$(BUILD)/kernel/init/trap.o \
	$(BUILD)/kernel/lib/bitmap.o \
	$(BUILD)/kernel/lib/math.o \
	$(BUILD)/kernel/lib/rbtree.o \
	$(BUILD)/kernel/lib/string.o \
	$(BUILD)/kernel/mm/alloc_pages.o \
	$(BUILD)/kernel/mm/memory.o \
	$(BUILD)/kernel/mm/mmzone.o \
	$(BUILD)/kernel/mm/numa.o \
	$(BUILD)/kernel/mm/page.o \
	$(BUILD)/kernel/mm/slab.o \
	$(BUILD)/kernel/sched/cfs.o \
	$(BUILD)/kernel/sched/fcfs.o \
	$(BUILD)/kernel/sched/task.o \
	$(BUILD)/kernel/sys/fpu.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/start.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -T kernel.ld

$(BUILD)/system.bin : $(BUILD)/kernel.bin
	objcopy -O binary $< $@

$(BUILD)/system.map : $(BUILD)/kernel.bin
	nm $< | sort > $@

$(BUILD)/osdev.img: $(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system.bin \
	$(BUILD)/system.map
	yes | bximage -q -func=create -hd=32M -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=8 seek=2 conv=notrunc
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=240 seek=10 conv=notrunc

.PHONY: imgmk
imgmk: $(BUILD)/osdev.img
	@echo "imgmk successfully"

.PHONY: clean
clean:
	rm -rf $(BUILD)
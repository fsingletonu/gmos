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

$(BUILD)/drivers/video/%.o : /drivers/video/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/init/%.o : /init/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/lib/%.o : /lib/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/mm/%.o : /mm/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel.bin :  $(BUILD)/start.o \
	$(BUILD)/main.o \
	$(BUILD)/drivers/video/VGAVideo.o \
	$(BUILD)/init/acpi.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/mm/early_alloc.o \
	$(BUILD)/mm/numa.o \
	$(BUILD)/mm/page.o \
	$(BUILD)/mm/slab.o \
	$(BUILD)/mm/zone.o
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
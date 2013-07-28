all: BootLoader Kernel32 Disk.img

BootLoader:
	@echo
	@echo ================= Build Boot Loader ================= 
	@echo

	make -C 00.BootLoader

	@echo
	@echo ================= Build Complete ================= 
	@echo

Kernel32:
	@echo
	@echo ================= Build 32bit Kernel ================= 
	@echo

	make -C 01.Kernel32

	@echo
	@echo ================= Build Complete ================= 
	@echo

ImageMaker: 04.Utility/00.ImageMaker/ImageMaker.c
	@echo
	@echo ================= Build ImageMaker ================= 
	@echo

	make -C 04.Utility/00.ImageMaker
	cp 04.Utility/00.ImageMaker/ImageMaker.exe .

	@echo
	@echo ================= Build Complete ================= 
	@echo

Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin
	@echo
	@echo ================= Disk.img Build Start ================= 
	@echo

	./ImageMaker.exe $^

	@echo
	@echo ================= All Build Complete ================= 
	@echo

clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	rm -f Disk.img
	make -C 04.Utility/00.ImageMaker clean
	rm -f ImageMaker.exe

run: Disk.img
	qemu-system-x86_64.exe -m 64 -fda ./Disk.img -localtime -M pc

all: ImageMaker

ImageMaker:
	@echo
	@echo ================= Build ImageMaker ================= 
	@echo

	make -C 00.ImageMaker
	cp 00.ImageMaker/ImageMaker ..

	@echo
	@echo ================= Build Complete ================= 
	@echo

clean:
	make -C 00.ImageMaker clean
	rm ../ImageMaker

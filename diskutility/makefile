#Change this to the depth of the project folders
#if needed, add a preffix for a common project folder
SOURCE = $(wildcard mfstool/*/*/*.cs mfstool/*/*.cs mfstool/*.cs)

all: diskutility

diskutility: $(SOURCE)
	mcs $(SOURCE) /reference:System.Management.dll /reference:DiscUtils.dll -out:$@
	cp *.dll ../


img:
	mono diskutility -auto -target img -scheme mbr
	qemu-system-x86_64 -drive format=raw,file=mollenos.img

clean:
	rm -f ../diskutility
	rm -f /../*.dll

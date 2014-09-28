xdetect:
		gcc -o xdetect xdetect.c
all:
	  make clean
		make xdetect
		xdetect TrainingData.bin
clean:
	  rm xdetect


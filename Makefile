all: generate

generate: generate.h generate.cpp MFSGrayscale.h MFSGrayscale.cpp MarkerField.h MarkerField.cpp MFSColor.h MFSColor.cpp MFSImage.h MFSImage.cpp MFSquare.h MFSquare.cpp MFHexagonal.h MFHexagonal.cpp MFHGrayscale.h MFHGrayscale.cpp MFHColor.h MFHColor.cpp
	g++ -o generate generate.cpp MarkerField.cpp MFSquare.cpp MFHexagonal.cpp MFSColor.cpp MFSGrayscale.cpp MFSImage.cpp MFHGrayscale.cpp MFHColor.cpp `pkg-config opencv --cflags --libs` -g -W -Wall -lboost_system -pthread

clean:
	rm generate

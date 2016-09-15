all:
	gcc -o project1 project1.c
testimages:
	./project1 3 imageP3o.ppm out_3_to_3.ppm
	./project1 3 imageP6o.ppm out_6_to_3.ppm
	./project1 6 imageP3o.ppm out_3_to_6.ppm
	./project1 6 imageP6o.ppm out_6_to_6.ppm

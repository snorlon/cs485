Instructions:

    For all parts

        To setup makefile:
            cmake CMakeLists.txt
            cmake CMakeLists.txt
                second time resolves c++11 not being applied

    Part 1
        reset; make clean; make
        /DisplayImage <file path>

        ex: ./DisplayImage Data/Graf/img1.pgm

    Part 2
        reset; make clean; make
        /DisplayImage <file path> <scale to view (0 to 16)>

        ex: ./DisplayImage Data/Graf/img1.pgm 5
            To view scale 6 on the image in the provided folder

        interestImages holds the images of their interest points of each scale drawn on top of the original image.

        

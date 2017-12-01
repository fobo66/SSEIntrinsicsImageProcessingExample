#include <cstdio>
#include "im_lib.h"

#include <mach/mach.h>
#include <mach/mach_time.h>


#define COLOR_NUMBER 2
#define ITER_NUMBER  10

int main(int argc, char* argv[])
{
    char const *src_file = "BaboonRGB.bmp";	
	char const *proc_file = "BaboonProcessed.bmp";

	__uint8_t * src_img;
	__uint8_t  color_map[1024];

	BmpFileHeader im_header;
	BmpInfoHeader im_info;

	float **fImage;	
	float **colorBook;	

    uint64_t before = 0;
    uint64_t after = 0;
    uint64_t sseTimer = 0;


	src_img = imread(src_file, im_info, im_header, color_map);
	
	fImage = im2floatMtx(src_img, im_info.width, im_info.height);

	// Initialization of color book
	colorBook = new float*[4];
	for (int i = 0; i < 4; i++){
		colorBook[i] = new float[COLOR_NUMBER];
	}
	for (int i = 0; i < COLOR_NUMBER; i++){
		int ind = rand() % (im_info.width * im_info.height);
		colorBook[0][i] = fImage[0][ind];
		colorBook[1][i] = fImage[1][ind];
		colorBook[2][i] = fImage[2][ind];
		colorBook[3][i] = 0.0;
	}

	int *idx = new int[im_info.width * im_info.height];
	memset(idx, 0, im_info.height * im_info.width);	//Zero array

	// Image processing //
	for (int iteration = 0; iteration < ITER_NUMBER; iteration++){

		std::cout << "Iteration #" << iteration << "..." << std::endl;

		// STEP 1: Find Closest centroid
        before = mach_absolute_time();
		findClosestCentroid(fImage, im_info.width, im_info.height, colorBook, COLOR_NUMBER, idx);
        after = mach_absolute_time();

        sseTimer = after - before;

        std::cout << "Image processing on SSE took " << sseTimer << " ticks" << std::endl;

		// STEP 2: Move centroids
		for (int i=0; i<COLOR_NUMBER; i++){			
			colorBook[0][i] = 0.0; 	colorBook[1][i] = 0.0;
			colorBook[2][i] = 0.0;  colorBook[3][i] = 0.0;
		}
		for (int i=0; i < im_info.height * im_info.width; i++){
			colorBook[0][idx[i]] += fImage[0][i];
			colorBook[1][idx[i]] += fImage[1][i];
			colorBook[2][idx[i]] += fImage[2][i];
			colorBook[3][idx[i]]++;
		}
		//Average
		for (int i=0; i < COLOR_NUMBER; i++) {			
			colorBook[0][i] = floor(colorBook[0][i] / colorBook[3][i]);
			colorBook[1][i] = floor(colorBook[1][i] / colorBook[3][i]);
			colorBook[2][i] = floor(colorBook[2][i] / colorBook[3][i]);
		}
	}

	// STEP 3: Quantize Image
    before = mach_absolute_time();
	findClosestCentroid(fImage, im_info.width, im_info.height, colorBook, COLOR_NUMBER, idx);
    after = mach_absolute_time();

	for (int i = 0; i < im_info.height * im_info.width; i++){
		fImage[0][i] = colorBook[0][idx[i]];
		fImage[1][i] = colorBook[1][idx[i]];
		fImage[2][i] = colorBook[2][idx[i]];
	}

	// End of processing
	floatMtx2im(src_img, im_info.width, im_info.height, fImage);
	imwrite(proc_file, src_img, im_info, im_header, color_map);

    sseTimer = after - before;

    std::cout << "Image processing on SSE took " << sseTimer << " ticks" << std::endl;
	
	std::cout << "Press any key..." << std::endl;
    std::cin.get();

	delete [] src_img;
	delete [] idx;
	for(int i = 0; i < 4; ++i) 
		delete [] colorBook[i];	
	delete [] colorBook;
    return 0;
}
//---------------------------------------------------------------------------
 

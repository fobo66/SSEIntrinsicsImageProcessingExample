#include "im_lib.h"


__uint8_t* imread(char const *file_name, BmpInfoHeader &im_info, BmpFileHeader &im_header, __uint8_t* color_map) {
	std::fstream	bmpFile;	
	__uint8_t *data;

	bmpFile.open(file_name, std::ios::binary | std::ios::in);

	if (!bmpFile){
		std::cout <<"IMREAD: Bmp-file is not opened." <<std::endl;
		return NULL;
	}
	
	// Read the header of file
	bmpFile.seekg(0, std::ios::beg);
	bmpFile.read((char*)&im_header, sizeof(im_header));
		
	std::cout<<"Size of bmp-file:"<<im_header.size1 + (im_header.size2<<16)<<std::endl;

	// Read file information	
	bmpFile.read((char*)&im_info, sizeof(im_info));
	if (!im_info.compression)
		std::cout << "Image is not compressed. Color depth: " << im_info.bits << " bits." << std::endl;
	else
		std::cout << "Image is compressed." << std::endl;
	
	if (!im_info.ncolors)
		std::cout << "Full color image." << std::endl;
	else
		std::cout << "Image uses restricted number of colors: " << im_info.ncolors << std::endl;
	
	if (im_info.ncolors == 256)
		bmpFile.read((char*) color_map, 1024);
	
	// Read the image
	data = new __uint8_t[im_info.height * im_info.width * (im_info.bits / 8)];
	memset(data, 0, im_info.height * im_info.width);	//Zero array
	bmpFile.read((char*)data,im_info.height * im_info.width * (im_info.bits / 8));


	bmpFile.close();
	return data;

}

int imwrite(char const *file_name, __uint8_t* image, BmpInfoHeader &im_info, BmpFileHeader &im_header, __uint8_t* color_map) {
	std::fstream bmpFile;

	bmpFile.open(file_name, std::ios::binary | std::ios::out);
	
	if (!bmpFile){
		std::cout <<"IMWRITE: BMP-file is not opened." <<std::endl;
		return 1;
	}
	
	bmpFile.seekg(0, std::ios::beg);
	bmpFile.write((char*)&im_header, sizeof(im_header));
	
	bmpFile.write((char*)&im_info, sizeof(im_info));
	if (im_info.ncolors==256)
		bmpFile.write((char*)color_map, 1024);

	bmpFile.seekg(0, std::ios::end);
	bmpFile.write((char*)image, im_info.height * im_info.width * (im_info.bits/8));
	
	bmpFile.close();
	return 0;
}

float** im2floatMtx(__uint8_t *data, __int32_t width, __int32_t height) {
	float **fMtx;
	fMtx = new float*[3];
	for (int i = 0; i < 3; i++){
		fMtx[i] = new float[width*height];
	}

	
	for (int n=0; n<(width*height); n++){
		fMtx[0][n]=(float)data[3*n];
		fMtx[1][n]=(float)data[3*n+1];
		fMtx[2][n]=(float)data[3*n+2];
	}

	return fMtx;
}


void floatMtx2im(__uint8_t *data, __int32_t width, __int32_t height, float **fMtx) {

	for (int n = 0; n < (width * height); n++){
		data[3*n] = (__uint8_t) fMtx[0][n];
		data[3*n+1] = (__uint8_t) fMtx[1][n];
		data[3*n+2] = (__uint8_t) fMtx[2][n];
	}

	for(int i = 0; i < 3; ++i) {
		delete [] fMtx[i];
	}
	delete [] fMtx;

}

void findClosestCentroid(float **fImage, __int32_t width, __int32_t height, float **cBook, int nColor, int* idx) {
	float *distances;
    
    __m128 min_dist;
    __m128 base, *distancesSSE;
    __m128 diff1, diff2, diff3;
    __m128 pow1, pow2, pow3;
    __m128 fImageFirstElement, cBookFirstElement,
           fImageSecondElement, cBookSecondElement,
           fImageThirdElement, cBookThirdElement;

	if ((distances = (float*) _mm_malloc(nColor * sizeof(float), 16)) == NULL) {
        std::cerr << "Error allocating buffer\n";
        exit(1);
    }

    distancesSSE = (__m128*) distances;
 
	for (int i = 0; i < height * width; i++) {
        min_dist = _mm_set_ss(1000.0);
		// Loop over colors in colorBook
		for (int j = 0; j < nColor; j++) {
            fImageFirstElement = _mm_set_ss(fImage[0][i]);
            cBookFirstElement = _mm_set_ss(cBook[0][j]);
            fImageSecondElement = _mm_set_ss(fImage[1][i]);
            cBookSecondElement = _mm_set_ss(cBook[1][j]);
            fImageThirdElement = _mm_set_ss(fImage[2][i]);
            cBookThirdElement = _mm_set_ss(cBook[2][j]);      
            
            diff1 = _mm_sub_ss(fImageFirstElement, cBookFirstElement);
            diff2 = _mm_sub_ss(fImageSecondElement, cBookSecondElement);
            diff3 = _mm_sub_ss(fImageThirdElement, cBookThirdElement);

            pow1 = _mm_mul_ss(diff1, diff1);
            pow2 = _mm_mul_ss(diff2, diff2);
            pow3 = _mm_mul_ss(diff3, diff3);

            base = _mm_add_ss(pow1, pow2);
            base = _mm_add_ss(base, pow3);

            distancesSSE[j] = _mm_sqrt_ss(base);

            // find min distance
			if (_mm_comilt_ss(distancesSSE[j], min_dist) == 1) {
				min_dist = distancesSSE[j];
				idx[i] = j;
			}
		}
	}
    _mm_free(distances);	
}

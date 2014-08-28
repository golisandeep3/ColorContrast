/*
 * main.c
 *
 *  Created on: May 8, 2014
 *      Author: Sandeep Goli
 */

#include<stdio.h>
#include<malloc.h>
#include <stdint.h>
#include"lodepng.h"
#include<math.h>
#include<stdlib.h>

unsigned w; // width of the image
unsigned h; // height of the image
unsigned new_width;
unsigned new_height;
unsigned new_x;
unsigned new_y;
unsigned char* image; // the resulting image, as a set of white or gray pixels on a black background
unsigned char* imageCheck; // a copy of the results after an interation, used to check if all calcualtions are needed in the next iteration
unsigned char* imageBuffer;
unsigned  char* pix;
int iterations; // the number of pixels in the radius to search

int totalCalculations = 0; // the total number of calculations needed to complete the analysis
int calculationsCompleted = 0; // the total number of calculations completed so far

double contrastLevel; // the contrast ratio level to check for (passed from the




int evaluateColorContrast(int r1, int g1, int b1, int r2, int g2, int b2) {
	// This function is an optimized version of the algorithm found at
	// https://github.com/gdkraus/wcag2-color-contrast
	// It is optimized to reduce function calls. This is for speed performance
	// because this function is called for every single pixel comparison. The code could
	// be written more cleanly, but the overhead of the function calls adds significantly
	// to the processing time.

	double ratio;
	double l1; //luminosity of color 1
	double r, g, b;
	r = r1 / 255.0;
	g = g1 / 255.0;
	b = b1 / 255.0;
	if (r <= 0.03928) {
		r = r / 12.92;
	} else {
		r = pow(((r + 0.055) / 1.055), 2.4);
	}

	if (g <= 0.03928) {
		g = g / 12.92;
	} else {
		g = pow(((g + 0.055) / 1.055), 2.4);
	}

	if (b <= 0.03928) {
		b = b / 12.92;
	} else {
		b = pow(((b + 0.055) / 1.055), 2.4);
	}

	l1 = 0.2126 * r + 0.7152 * g + 0.0722 * b;

	double l2; //luminosity of color 2
	r = r2 / 255.0;
	g = g2 / 255.0;
	b = b2 / 255.0;
	if (r <= 0.03928) {
		r = r / 12.92;
	} else {
		r = pow(((r + 0.055) / 1.055), 2.4);
	}

	if (g <= 0.03928) {
		g = g / 12.92;
	} else {
		g = pow(((g + 0.055) / 1.055), 2.4);
	}

	if (b <= 0.03928) {
		b = b / 12.92;
	} else {
		b = pow(((b + 0.055) / 1.055), 2.4);
	}

	l2 = 0.2126 * r + 0.7152 * g + 0.0722 * b;

	if (l1 > l2) {
		ratio = ((l1 + 0.05) / (l2 + 0.05));
	} else {
		ratio = ((l2 + 0.05) / (l1 + 0.05));
	}

	if (round(ratio*10)/10.0 >= contrastLevel) {
		return 1; // two colors have enough contrast
	} else {
		return 0; // two colors do not have enough contrast
	}
}

void iterativeAnalyze(int radius) {
	int x = 0; // the counter in the width dimension
	int y = 0; // the counter in the height dimension
	int xMax = w; // the maximum width to check for, when iterating
	//int yMax = h; // the maximum height to check for, when iterating
	int success; // number of success
	//int failure; // number of failures
	//int foundContrastBorder = 0; // if an existing contrast border is found
	unsigned long int i2,n2;


	/////
	// check if any contrast borders already exist in the image processed so far for a given radius around a pixel
	//printf("String length: %d\n",w*h);
	for (i2 = 0, n2 = w*h; i2 < n2; i2 += 1) {

		success = 0;
		int j,k;



		for ( j = 0; j <= radius; j += 1) {

			for ( k = 0; k <= radius; k += 1) {
				if (!(j == 0 && k == 0) && (image[y * w + x] == 0 || radius == 1)) {

					// the base pixel to compare all of the others to, in RGB
					int basePixelRed = pix[i2 * 4];
					int basePixelGreen = pix[i2 * 4 + 1];
					int basePixelBlue = pix[i2 * 4 + 2];
					//printf("Blue pixel: %d\n",basePixelBlue);
					int temp =i2 * 4 + (w * j * 4) + (k * 4) + 2;

					// + + direction
					if(temp < w*h*4)
					{
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 + (w * j * 4) + (k * 4)], pix[i2 * 4 + (w * j * 4) + (k * 4) + 1], pix[i2 * 4 + (w * j * 4) + (k * 4) + 2])) {
							//printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}
					temp =i2 * 4 + (w * j * 4) - (k * 4);

					// + - direction
					if(temp < w*h*4)
					{
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 + (w * j * 4) - (k * 4)], pix[i2 * 4 + (w * j * 4) - (k * 4) + 1], pix[i2 * 4 + (w * j * 4) - (k * 4) + 2])) {
							//printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}

					temp =i2 * 4 - (w * j * 4) + (k * 4);
					//printf("Blue pixel2: %d\n",temp);
					// - + direction
					if(temp > 0)
					{
						//printf("inside if\n");
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 - (w * j * 4) + (k * 4)], pix[i2 * 4 - (w * j * 4) + (k * 4) + 1], pix[i2 * 4 - (w * j * 4) + (k * 4) + 2])) {
							//printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}
					temp =i2 * 4 - (w * j * 4) - (k * 4);

					// - - direction
					if(temp >0)
					{
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 - (w * j * 4) - (k * 4)], pix[i2 * 4 - (w * j * 4) - (k * 4) + 1], pix[i2 * 4 - (w * j * 4) - (k * 4) + 2])) {
							//printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}

					if(success > 0){
						break; // if a border if found, stop
					}
				}
			}
		}

		// draw the result as a pixel
		if (image[y * w + x] == 0 || radius == 1) {
			if (success > 0) {
				if (radius == 1) {
					//printf("Assigned white : x:%d  y:%d\n",x,y);
					image[y * w + x] = 255; // white
				} else if (radius == 2) {
					//printf("Assigned ,light gray\n");
					image[y * w + x] = 170; // light gray
				} else if (radius == 3) {
					//printf("Assigned medium gray\n");
					image[y * w + x] = 85; // medium gray
				}

			} else {
				//printf("Assigned black\n");
				image[y * w + x] = 0; // black

			}
		}
		//	}

		//updateStatus(radius);
		x += 1; // increment the counter in the width dimension
		if (x >= xMax) { // if we reach the end of the line, go the next line
			x = 0; // reset the counter in the width dimension
			y += 1; // in crement the counter in the height dimension
		}
		//printf("iteration\n");

	}
	//int i,j;
	/*for(i=0;i<h;i++)
		for(j=0;j<w;j++)
			imageCheck[i * (w) + j]=image[i * (w) + j];*/
	//strcpy(imageCheck,image); // copy the array to imageCheck so it can be used in the next iteration
	printf("Finished analyzing image\n");
}


void decodeImage(char *filename)
{
	unsigned error;
	int i;

	error = lodepng_decode32_file(&pix, &w, &h, filename);
	image  = malloc(w*h);
	//imageCheck  = malloc(strlen(pix));
	bzero(image,w*h);
	//bzero(imageCheck,strlen(pix));
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	for (i = 1; i <= iterations; i += 1) {
		// the size of the pixel radius to analyze
		iterativeAnalyze(i);
	}
	printf("Finished decoding image\n");

}
void encodeOneStep(const char* filename)
{
	/*Encode the image*/
	int m,n,pixelData,pixelLocation;
	unsigned char *data  = malloc(w*h*4);
	/*for (m = 0; m < h; m += 1) {
		for (n = 0; n < w; n += 1) {
			pixelData = image[m * (w) + n];
			pixelLocation = (m * (w) + n)*4;
								data[pixelLocation + 0] = pixelData;
								data[pixelLocation + 1] = pixelData;
								data[pixelLocation + 2] = pixelData;
								data[pixelLocation + 3] = 255; //opacity of 75% 192

	}
	}
	unsigned error = lodepng_encode32_file(filename, data, w, h);
	*/
	//unsigned error = lodepng_encode32_file(filename, image, w, h);

	/*for (m = 0, n = w*h; m < n; m += 1) {
					//pixelLocation = (m * (w) + n)*4;
			pixelData = image[m];
			data[m * 4 + 0] = pixelData;
			data[m * 4 + 1] = pixelData;
			data[m * 4 + 2] = pixelData;
			data[m * 4 + 3] = 255; //opacity of 75% 192

	}*/
	//unsigned error = lodepng_encode32_file(filename, data, w, h);
	/*if there's an error, display it*/
	int i,j;

	for (m = new_x,i=0; (m-new_x) < new_height && m<h; m += 1,i++) {
			for (n = new_y,j=0; (n-new_y) < new_width && n<w; n += 1,j++) {
				pixelData = image[m * (w) + n];
				pixelLocation = (i * (new_width) + j)*4;
									data[pixelLocation + 0] = pixelData;
									data[pixelLocation + 1] = pixelData;
									data[pixelLocation + 2] = pixelData;
									data[pixelLocation + 3] = 255; //opacity of 75% 192

		}
		}

	unsigned error = lodepng_encode32_file(filename, data, new_width, new_height);

		if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	printf("Finished encoding image\n");
}

int main(int argc,char *argv[])
{
	if(argc < 4)
	{
		printf("Enter correct number of parameters\n");
		printf("[input filename] [radius] [contrast level] [output filename] [X] [Y] [Width] [Height]");
		return 0;

	}
	char *input_filename = argv[1];
	iterations = atoi(argv[2]);
	contrastLevel =atof(argv[3]);
	char *output_filename = argv[4];
	new_x = atoi(argv[5]);
	new_y = atoi(argv[6]);
	new_width = atoi(argv[7]);
	new_height = atoi(argv[8]);
	//printf("%f\n",contrastLevel);
	decodeImage(input_filename);
	encodeOneStep(output_filename);
	return 0;


	//image_analyze(iterations);

}


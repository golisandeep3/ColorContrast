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
unsigned char* image; // the resulting image, as a set of white or gray pixels on a black background
unsigned char* imageCheck; // a copy of the results after an interation, used to check if all calcualtions are needed in the next iteration
unsigned char* imageBuffer;
unsigned  char* pix;
int iterations; // the number of pixels in the radius to search

int totalCalculations = 0; // the total number of calculations needed to complete the analysis
int calculationsCompleted = 0; // the total number of calculations completed so far

int contrastLevel; // the contrast ratio level to check for (passed from the


int evaluateColorContrast(double r1, double g1, double b1, double r2, double g2, double b2) {
	// This function is an optimized version of the algorithm found at
	// https://github.com/gdkraus/wcag2-color-contrast
	// It is optimized to reduce function calls. This is for speed performance
	// because this function is called for every single pixel comparison. The code could
	// be written more cleanly, but the overhead of the function calls adds significantly
	// to the processing time.

	float ratio;
	double l1; //luminosity of color 1
	double r, g, b;
	r = r1 / 255;
	g = g1 / 255;
	b = b1 / 255;
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

	int l2; //luminosity of color 2
	r = r2 / 255;
	g = g2 / 255;
	b = b2 / 255;
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

	if (round(ratio*10)/10 >= contrastLevel) {
		return 1; // two colors have enough contrast
	} else {
		return 0; // two colors do not have enough contrast
	}
}

void iterativeAnalyze(radius) {
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
	printf("String length: %d\n",w*h);
	for (i2 = 0, n2 = w*h; i2 < n2; i2 += 1) {
		//printf("i2: %d n2:%d\n",i2,n2);

		//foundContrastBorder = 0;

		/*for (j2 = 0; j2 <= radius; j2 += 1) {
			if(foundContrastBorder){
				break; // stop checking if a border is found
			}
			for (k2 = 0; k2 <= radius; k2 += 1) {
				if(foundContrastBorder){
					break; // stop checking if a border is found
				}

				// check to see if a contrast border had been found on a previous iteration
				//
				// + + direction
				if (imageCheck[i2 + (w * j2) + (k2)] > 0) {
					foundContrastBorder = 1;
				}
				// + - direction
				if (imageCheck[i2 + (w * j2) - (k2)] > 0) {
					foundContrastBorder = 1;
				}
				// - + direction
				if (imageCheck[i2 - (w * j2) + (k2)] > 0) {
					foundContrastBorder = 1;
				}
				// - - direction
				if (imageCheck[i2 - (w * j2) - (k2)] > 0) {
					foundContrastBorder = 1;
				}
			}
		}*/
		printf("Inside foundContrastBorder\n");
		success = 0;
		//failure = 0;
		int j,k;

		//	if (!foundContrastBorder) {


		for ( j = 0; j <= radius; j += 1) {

			for ( k = 0; k <= radius; k += 1) {
				if (!(j == 0 && k == 0) && (image[y * w + x] == 0 || radius == 1)) {

					// the base pixel to compare all of the others to, in RGB
					int basePixelRed = pix[i2 * 4];
					int basePixelGreen = pix[i2 * 4 + 1];
					int basePixelBlue = pix[i2 * 4 + 2];
					printf("Blue pixel: %d\n",basePixelBlue);
					int temp =i2 * 4 + (w * j * 4) + (k * 4) + 2;

					// + + direction
					if(temp < w*h*4)
					{
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 + (w * j * 4) + (k * 4)], pix[i2 * 4 + (w * j * 4) + (k * 4) + 1], pix[i2 * 4 + (w * j * 4) + (k * 4) + 2])) {
							printf("Inside Success\n");
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
							printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}

					temp =i2 * 4 - (w * j * 4) + (k * 4);
					printf("Blue pixel2: %d\n",temp);
					// - + direction
					if(temp > 0)
					{
						printf("inside if\n");
						if (evaluateColorContrast(basePixelRed, basePixelGreen, basePixelBlue, pix[i2 * 4 - (w * j * 4) + (k * 4)], pix[i2 * 4 - (w * j * 4) + (k * 4) + 1], pix[i2 * 4 - (w * j * 4) + (k * 4) + 2])) {
							printf("Inside Success\n");
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
							printf("Inside Success\n");
							success = +1;
						}
					}//else {
					//failure = +1;
					//}

					if(success > 0){
						break; // if a border if found, stop
					}
					if(success > 0){
						break;
					}
				}
			}
		}

		// draw the result as a pixel
		if (image[y * w + x] == 0 || radius == 1) {
			if (success > 0) {
				if (radius == 1) {
					printf("Assigned white\n");
					image[y * w + x] = 255; // white
				} else if (radius == 2) {
					image[y * w + x] = 170; // light gray
				} else if (radius == 3) {
					image[y * w + x] = 85; // medium gray
				}

			} else {
				printf("Assigned black\n");
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
		printf("iteration\n");

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
	for (m = 0; m < h; m += 1) {
		for (n = 0; n < w; n += 1) {
			pixelData = image[m * (w) + n];
			pixelLocation = (m * (w) + n)*4;
			data[pixelLocation + 0] = pixelData;
			data[pixelLocation + 1] = pixelData;
			data[pixelLocation + 2] = pixelData;
			data[pixelLocation + 3] = 192; //opacity of 75%
		}
	}
	unsigned error = lodepng_encode32_file(filename, data, w, h);

	/*if there's an error, display it*/
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	printf("Finished encoding image\n");
}

int main(int argc,char *argv[])
{
	if(argc < 3)
	{
		printf("Enter correct number of parameters\n");
		return 0;

	}
	char *filename = argv[1];
	iterations = atoi(argv[2]);
	contrastLevel =atoi(argv[3]);
	decodeImage(filename);
	encodeOneStep("test1.png");
	return 0;


	//image_analyze(iterations);

}


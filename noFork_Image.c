/* noFork_Image.c

   This program converts an input image to gray, red, blue 
   or green scale depending on the provided command 
   line inputs.

  Compile:
  cc noFork_Image.c imageUtils.c -o noFork -std=c99 -lm

  Run:
  ./noFork input.jpg output.jpg 2

 */
#include <stdlib.h>
#include <stdio.h>
#include "colorUtils.h"
#include "imageUtils.h"
int main(int argc, char **argv) {

    char *inputFile  = NULL;
    char *outputFile = NULL;
    int mode = 1;
    if( argc == 3 || argc == 4 ) {
        inputFile  = argv[1];
        outputFile = argv[2];
        if( argc == 4 ) {
            mode = atoi(argv[3]);
        }
    } 
    else {
        fprintf(stderr, "Usage:" );
        fprintf(stderr, " %s inputFileName outputFileName [mode]\n", argv[0]);
        fprintf(stderr, "  [mode: 1 = Gray, 2 = Red,\n" );
        fprintf(stderr, "         3 = Blue,        4 = Green]\n" );
        exit(1);
    }
    int h, w;
    Pixel **image = LoadImage(inputFile, &h, &w);
    Pixel **newImage;
    if( mode == 1 ) {
        ImageToGrayScale( image, h, w, AVERAGE);
    } 
    else if( mode == 2 ) { 
        ExtractRGB(image, h, w, RED);
    } 
    else if (mode == 3){
        ExtractRGB(image, h, w, BLUE);
    } 
    else if (mode == 4){
        ExtractRGB(image, h, w, GREEN);
    } 
    else {
        fprintf(stderr, "Invalid mode\n");
    }
    // show numerical values DumpImage( image, h, w );
    // save resulting image
    SaveImage(outputFile, image, h, w);
    return 0;
}

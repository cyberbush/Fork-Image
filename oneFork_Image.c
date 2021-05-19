/* oneFork_Image.c

   This program converts an input image to gray, red, blue 
   or green scale depending on the provided command 
   line inputs.

  Compile:
  cc oneFork_Image.c imageUtils.c -o oneFork -lm

  Run:
  ./oneFork input.jpg output.jpg 2

 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "colorUtils.h"
#include "imageUtils.h"

int main(int argc, char **argv) {

    char *inputFile  = NULL;
    char *outputFile = NULL;
    int mode = 1;
    int h, w; // height and width of main image
    pid_t pid; // process id
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

    Pixel **startImage = LoadImage(inputFile, &h, &w);
    unsigned char *part1 = mmap(NULL, (h/2)*w*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part1 == MAP_FAILED){
        printf("Mapping Failed\n");
        return 1;
    }
    Pixel **childImage;
    Pixel **parentImage;
    if( mode == 1 ) {
        ImageToGrayScale( startImage, h, w, AVERAGE);
    } 
    else if( mode == 2 ) { 
        // fork here
        pid = fork(); 
        if(pid == 0){ // its the child
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w, RED);
            ConvertPixelto1D(otherImage, &part1, h/2, w);
            exit(0);
        }
        else if(pid > 0){ //parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0, w, RED );
            waitpid(pid, NULL, 0);
            childImage = Convert1DtoPixel(part1, h/2, w);
        }
    } 
    else if (mode == 3){
         // fork here
        pid = fork(); 
        if(pid == 0){ // its the child
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w, BLUE);
            ConvertPixelto1D(otherImage, &part1, h/2, w);
            exit(0);
        }
        else if(pid > 0){ //parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0, w, BLUE );
            waitpid(pid, NULL, 0);
            childImage = Convert1DtoPixel(part1, h/2, w);
        }   
    }
    else if (mode == 4){
        // fork here
        pid = fork(); 
        if(pid == 0){ // its the child
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w, GREEN);
            ConvertPixelto1D(otherImage, &part1, h/2, w);
            exit(0);
        }
        else if(pid > 0){ //parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0, w, GREEN );
            waitpid(pid, NULL, 0);
            childImage = Convert1DtoPixel(part1, h/2, w);
        }
    } 
    else {
        fprintf(stderr, "Invalid mode\n");
    }

    // show numerical values DumpImage( startImage, h, w );

    // Parent process should combine images and save output
    if(pid>0){
        Pixel **finalImage = (Pixel **)malloc(sizeof(Pixel *) * h);
        finalImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));
        for( int i = 1 ; i < h ; i++ ) {
            finalImage[i] = (*finalImage + (w * i));
        }
        // combine parts
        CombineImagePieces(finalImage, childImage, 0, h/2, 0, w);
        CombineImagePieces(finalImage, parentImage, h/2, h, 0, w);
        // save resulting image
        SaveImage(outputFile, finalImage, h, w);
    }
    return 0;
}

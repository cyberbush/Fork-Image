/* twoFork_Image.c

   This program converts an input image to gray, red, blue 
   or green scale depending on the provided command 
   line inputs.

  Compile:
  cc twoFork_Image.c imageUtils.c -o twoFork -lm

  Run:
  ./twoFork input.jpg output.jpg 2

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
    pid_t pid1, pid2; // process id
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

    Pixel **startImage = LoadImage(inputFile, &h, &w);\
    // define mapped parts (going to store 1D version of image in)
    unsigned char *part1 = mmap(NULL, (h/2)*(w/2)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part1 == MAP_FAILED){
        printf("Mapping Failed Part1\n");
        return 1;
    }
    unsigned char *part2 = mmap(NULL, (h/2)*(w/2)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part2 == MAP_FAILED){
        printf("Mapping Failed Part2\n");
        return 1;
    }
    unsigned char *part3 = mmap(NULL, (h/2)*(w/2)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part3 == MAP_FAILED){
        printf("Mapping Failed Part3\n");
        return 1;
    }    
    Pixel **childImage1;
    Pixel **childImage2;
    Pixel **childImage3;
    Pixel **parentImage;
    if( mode == 1 ) {
        ImageToGrayScale( startImage, h, w, AVERAGE);
    } 
    else if( mode == 2 ) { 
        // fork here (making 4 processes)
        pid1 = fork(); 
        pid2 = fork();
        if(pid1 == 0 && pid2 == 0){ //child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/2, RED);
            ConvertPixelto1D(otherImage, &part1, h/2, w/2);
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0) { //child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, w, RED);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/2)+(w%2));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0) { //parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/2, RED);
            ConvertPixelto1D(otherImage, &part3, (h/2)+(h%2), w/2);
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0){ // parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, w/2, w, RED );
            waitpid(pid2, NULL, 0);
            waitpid(pid1, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/2);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/2)+(w%2));
            childImage3 = Convert1DtoPixel(part3, (h/2)+(h%2), w/2);
        }
    } 
    else if (mode == 3){
        // fork here (making 4 processes)
        pid1 = fork(); 
        pid2 = fork();
        if(pid1 == 0 && pid2 == 0){ //child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/2, BLUE);
            ConvertPixelto1D(otherImage, &part1, h/2, w/2);
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0) { //child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, w, BLUE);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/2)+(w%2));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0) { //parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/2, BLUE);
            ConvertPixelto1D(otherImage, &part3, (h/2)+(h%2), w/2);
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0){ // parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, w/2, w, BLUE );
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/2);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/2)+(w%2));
            childImage3 = Convert1DtoPixel(part3, (h/2)+(h%2), w/2);
        }    
    }
    else if (mode == 4){
        // fork here (making 4 processes)
        pid1 = fork(); 
        pid2 = fork();
        if(pid1 == 0 && pid2 == 0){ //child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/2, GREEN);
            ConvertPixelto1D(otherImage, &part1, h/2, w/2);
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0) { //child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, w, GREEN);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/2)+(w%2));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0) { //parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/2, GREEN);
            ConvertPixelto1D(otherImage, &part3, (h/2)+(h%2), w/2);
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0){ // parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, w/2, w, GREEN );
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/2);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/2)+(w%2));
            childImage3 = Convert1DtoPixel(part3, (h/2)+(h%2), w/2);
        }
    } 
    else {
        fprintf(stderr, "Invalid mode\n");
    }

    // show numerical values DumpImage( startImage, h, w );

    // Parent process should combine images and save output
    if(pid1>0 && pid2 >0){
        Pixel **finalImage = (Pixel **)malloc(sizeof(Pixel *) * h);
        finalImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));
        for( int i = 1 ; i < h ; i++ ) {
            finalImage[i] = (*finalImage + (w * i));
        }
        // combine parts
        CombineImagePieces(finalImage, childImage1, 0, h/2, 0, w/2);
        CombineImagePieces(finalImage, childImage2, 0, h/2, w/2, w);
        CombineImagePieces(finalImage, childImage3, h/2, h, 0, w/2);
        CombineImagePieces(finalImage, parentImage, h/2, h, w/2, w);
        // save resulting image
        SaveImage(outputFile, finalImage, h, w);
    }
    return 0;
}
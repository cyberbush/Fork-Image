/* threeFork_Image.c

   This program converts an input image to gray, red, blue 
   or green scale depending on the provided command 
   line inputs.

  Compile:
  cc threeFork_Image.c imageUtils.c -o threeFork -lm

  Run:
  ./threeFork input.jpg output.jpg 2

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
    pid_t pid1, pid2, pid3; // process id
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
    unsigned char *part1 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part1 == MAP_FAILED){
        printf("Mapping Failed Part1\n");
        return 1;
    }
    unsigned char *part2 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part2 == MAP_FAILED){
        printf("Mapping Failed Part2\n");
        return 1;
    }
    unsigned char *part3 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part3 == MAP_FAILED){
        printf("Mapping Failed Part3\n");
        return 1;
    }
    unsigned char *part4 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part4 == MAP_FAILED){
        printf("Mapping Failed Part4\n");
        return 1;
    }  
    unsigned char *part5 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part5 == MAP_FAILED){
        printf("Mapping Failed Part5\n");
        return 1;
    }   
    unsigned char *part6 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part6 == MAP_FAILED){
        printf("Mapping Failed Part6\n");
        return 1;
    }    
    unsigned char *part7 = mmap(NULL, (h/2)*(w/4)*3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
    if(part7 == MAP_FAILED){
        printf("Mapping Failed Part7\n");
        return 1;
    }          
    Pixel **childImage1, **childImage2, **childImage3, **childImage4, **childImage5, **childImage6, **childImage7;
    Pixel **parentImage;
    if( mode == 1 ) {
        ImageToGrayScale( startImage, h, w, AVERAGE);
    } 
    else if( mode == 2 ) { 
        // fork here (making 8 processes)
        pid1 = fork(); 
        pid2 = fork();
        pid3 = fork();
        if(pid1 == 0 && pid2 == 0 && pid3 == 0){ //child/child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/4, RED);
            ConvertPixelto1D(otherImage, &part1, h/2, w/4);
            exit(0);
        }
        else if(pid1 == 0 && pid2 == 0 && pid3 > 0) { //child/child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/4, w/2, BLUE);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 == 0) { //child/parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, (0.75)*w, GREEN);
            ConvertPixelto1D(otherImage, &part3, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 > 0) { //child/parent/parent *part4
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0.75*w, w, RED);
            ConvertPixelto1D(otherImage, &part4, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 == 0) { //parent/child/child *part5
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/4, BLUE);
            ConvertPixelto1D(otherImage, &part5, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 > 0) { //parent/child/parent *part6
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/4, w/2, GREEN);
            ConvertPixelto1D(otherImage, &part6, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 & pid3 == 0) { //parent/parent/child *part7
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/2, 0.75*w, RED);
            ConvertPixelto1D(otherImage, &part7, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 && pid3 > 0){ // parent/parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0.75*w, w, BLUE );
            waitpid(pid3, NULL, 0);
            waitpid(pid2, NULL, 0);
            waitpid(pid1, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/4);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/4)+(w%4));
            childImage3 = Convert1DtoPixel(part3, h/2, (w/4)+(w%4));
            childImage4 = Convert1DtoPixel(part4, h/2, (w/4)+(w%4));
            childImage5 = Convert1DtoPixel(part5, (h/2)+(h%2), (w/4)+(w%4));
            childImage6 = Convert1DtoPixel(part6, (h/2)+(h%2), (w/4)+(w%4));
            childImage7 = Convert1DtoPixel(part7, (h/2)+(h%2), (w/4)+(w%4));
        }
    } 
    else if (mode == 3){
        // fork here (making 8 processes)
        pid1 = fork(); 
        pid2 = fork();
        pid3 = fork();
        if(pid1 == 0 && pid2 == 0 && pid3 == 0){ //child/child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/4, BLUE);
            ConvertPixelto1D(otherImage, &part1, h/2, w/4);
            exit(0);
        }
        else if(pid1 == 0 && pid2 == 0 && pid3 > 0) { //child/child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/4, w/2, BLUE);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 == 0) { //child/parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, (0.75)*w, BLUE);
            ConvertPixelto1D(otherImage, &part3, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 > 0) { //child/parent/parent *part4
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0.75*w, w, BLUE);
            ConvertPixelto1D(otherImage, &part4, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 == 0) { //parent/child/child *part5
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/4, BLUE);
            ConvertPixelto1D(otherImage, &part5, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 > 0) { //parent/child/parent *part6
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/4, w/2, BLUE);
            ConvertPixelto1D(otherImage, &part6, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 & pid3 == 0) { //parent/parent/child *part7
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/2, 0.75*w, BLUE);
            ConvertPixelto1D(otherImage, &part7, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 && pid3 > 0){ // parent/parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0.75*w, w, BLUE );
            waitpid(pid3, NULL, 0);
            waitpid(pid2, NULL, 0);
            waitpid(pid1, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/4);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/4)+(w%4));
            childImage3 = Convert1DtoPixel(part3, h/2, (w/4)+(w%4));
            childImage4 = Convert1DtoPixel(part4, h/2, (w/4)+(w%4));
            childImage5 = Convert1DtoPixel(part5, (h/2)+(h%2), (w/4)+(w%4));
            childImage6 = Convert1DtoPixel(part6, (h/2)+(h%2), (w/4)+(w%4));
            childImage7 = Convert1DtoPixel(part7, (h/2)+(h%2), (w/4)+(w%4));
        }
    }
    else if (mode == 4){
        // fork here (making 8 processes)
        pid1 = fork(); 
        pid2 = fork();
        pid3 = fork();
        if(pid1 == 0 && pid2 == 0 && pid3 == 0){ //child/child/child *part1
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0, w/4, GREEN);
            ConvertPixelto1D(otherImage, &part1, h/2, w/4);
            exit(0);
        }
        else if(pid1 == 0 && pid2 == 0 && pid3 > 0) { //child/child/parent *part2
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/4, w/2, GREEN);
            ConvertPixelto1D(otherImage, &part2, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 == 0) { //child/parent/child *part3
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, w/2, (0.75)*w, GREEN);
            ConvertPixelto1D(otherImage, &part3, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 == 0 && pid2 > 0 & pid3 > 0) { //child/parent/parent *part4
            Pixel ** otherImage = ExtractPartRGB(startImage, 0, h/2, 0.75*w, w, GREEN);
            ConvertPixelto1D(otherImage, &part4, h/2, (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 == 0) { //parent/child/child *part5
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, 0, w/4, GREEN);
            ConvertPixelto1D(otherImage, &part5, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 == 0 & pid3 > 0) { //parent/child/parent *part6
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/4, w/2, GREEN);
            ConvertPixelto1D(otherImage, &part6, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 & pid3 == 0) { //parent/parent/child *part7
            Pixel ** otherImage = ExtractPartRGB(startImage, h/2, h, w/2, 0.75*w, GREEN);
            ConvertPixelto1D(otherImage, &part7, (h/2)+(h%2), (w/4)+(w%4));
            exit(0);
        }
        else if(pid1 > 0 && pid2 > 0 && pid3 > 0){ // parent/parent/parent
            parentImage = ExtractPartRGB( startImage, h/2, h, 0.75*w, w, GREEN );
            waitpid(pid3, NULL, 0);
            waitpid(pid2, NULL, 0);
            waitpid(pid1, NULL, 0);
            childImage1 = Convert1DtoPixel(part1, h/2, w/4);
            childImage2 = Convert1DtoPixel(part2, h/2, (w/4)+(w%4));
            childImage3 = Convert1DtoPixel(part3, h/2, (w/4)+(w%4));
            childImage4 = Convert1DtoPixel(part4, h/2, (w/4)+(w%4));
            childImage5 = Convert1DtoPixel(part5, (h/2)+(h%2), (w/4)+(w%4));
            childImage6 = Convert1DtoPixel(part6, (h/2)+(h%2), (w/4)+(w%4));
            childImage7 = Convert1DtoPixel(part7, (h/2)+(h%2), (w/4)+(w%4));
        }
    } 
    else {
        fprintf(stderr, "Invalid mode\n");
    }

    // show numerical values DumpImage( startImage, h, w );

    // Parent process should combine images and save output
    if(pid1>0 && pid2 >0 && pid3 > 0){
        Pixel **finalImage = (Pixel **)malloc(sizeof(Pixel *) * h);
        finalImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));
        for( int i = 1 ; i < h ; i++ ) {
            finalImage[i] = (*finalImage + (w * i));
        }
        // combine parts
        CombineImagePieces(finalImage, childImage1, 0, h/2, 0, w/4); //part1
        CombineImagePieces(finalImage, childImage2, 0, h/2, w/4, w/2); //part2
        CombineImagePieces(finalImage, childImage3, 0, h/2, w/2, 0.75*w); //part3
        CombineImagePieces(finalImage, childImage4, 0, h/2, 0.75*w, w); //part4
        CombineImagePieces(finalImage, childImage5, h/2, h, 0, w/4); //part5
        CombineImagePieces(finalImage, childImage6, h/2, h, w/4, w/2); //part6
        CombineImagePieces(finalImage, childImage7, h/2, h, w/2, 0.75*w); //part7
        CombineImagePieces(finalImage, parentImage, h/2, h, 0.75*w, w); //part8
        // save resulting image
        SaveImage(outputFile, finalImage, h, w);
    }
    return 0;
}
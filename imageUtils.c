/* imageUtils.c
 */

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "colorUtils.h"
#include "imageUtils.h"


Pixel **LoadImage(const char *filePath, int *height, int *width) 
{
  int x, y, n;

  unsigned char *data = stbi_load(filePath, &x, &y, &n, 4); 
                        // 4 = force RGBA channels
  *height = y;
  *width  = x;

  // contiguous allocation:
  Pixel **image = (Pixel **)malloc(sizeof(Pixel *) * y);
  image[0] = (Pixel *)malloc(sizeof(Pixel) * (y * x));
  for( int i = 1 ; i < y ; i++ ) {
    image[i] = (*image + (x * i));
  }

  int r = 0;  // row
  int c = 0;  // col

  for( int i = 0; i < (x * y * 4); i += 4 ) { // for each row
      image[r][c].red   = data[i+0];
      image[r][c].green = data[i+1];
      image[r][c].blue  = data[i+2];
      c++;
      if( c == x ) {
           //  next row, reset column index
        r++;
        c = 0;
      }
  }

  stbi_image_free(data);

  return image;
}


void SaveImage(const char *fileName, Pixel **image, int height, int width) {

    // Convert Pixel array (2D) to single array with
    // three (RGB) channels. 
    // Ignore alpha channel and assume 100% opaque
  unsigned char *data = (unsigned char *) malloc(height * width * 3);

  int x = 0;
  for( int i = 0 ; i < height ; i++ ) {
    for( int j = 0 ; j < width ; j++ ) {
      data[x+0] = image[i][j].red;
      data[x+1] = image[i][j].green;
      data[x+2] = image[i][j].blue;
      x += 3;
    }
  }
     //  write
  stbi_write_jpg(fileName, width, height, 3, data, 100);

  free(data);
}

Pixel ** CopyImage(Pixel **image, int height, int width) 
{
  Pixel **copy = (Pixel **)malloc(sizeof(Pixel *) * height);

  copy[0] = (Pixel *)malloc(sizeof(Pixel) * (height * width));
  for( int i = 1 ; i < height ; i++ ) {
    copy[i] = (*copy + (width * i));
  }

  for( int i = 0 ; i < height ; i++ ) {
    for( int j = 0 ; j < width ; j++ ) {
      copy[i][j] = image[i][j];
    }
  }

  return copy;
}


#define MIN_MACRO(a,b) a < b ? a : b
#define MAX_MACRO(a,b) a > b ? a : b

/*
  https://docs.gimp.org/2.6/en/gimp-tool-desaturate.html
 */
void ImageToGrayScale( Pixel **image, int height, int width, Mode m )
{
  for( int i = 0 ; i < height ; i++ ) {
    for( int j = 0 ; j < width ; j++ ) {
      int r, g, b;
      int gray;

      r = image[i][j].red;
      g = image[i][j].green;
      b = image[i][j].blue;

      if( m == AVERAGE )  // average
      {
         double dGray = (r + g + b) / 3.0;
         //gray = MIN_MACRO( (int)dGray, 255 );
         gray = (int)dGray;
         image[i][j].red   = gray;
         image[i][j].green = gray,
         image[i][j].blue  = gray;
      }
    }
  }
}


void ExtractRGB( Pixel **image, int height, int width, Mode m ) 
{
  for( int i = 0 ; i < height ; i++ ) {
    for( int j = 0 ; j < width ; j++ ) {
       if( m == RED )
       {
          //image[i][j].red;     // unchanged
          image[i][j].green = 0;
          image[i][j].blue  = 0;
       }
       else if( m == GREEN )
       {
          image[i][j].red = 0;
          //image[i][j].green = 0; // unchanged
          image[i][j].blue  = 0;
       }
       else if( m == BLUE )
       {
          image[i][j].red = 0;
          image[i][j].green = 0;
          //image[i][j].blue  = 0;  // unchanged
       }
    }
  }
}

Pixel ** ExtractPartRGB( Pixel **image, int heightStart, int heightEnd, int widthStart, int widthEnd, Mode m ) 
{
  int h = heightEnd-heightStart;
  int w = widthEnd-widthStart;
  Pixel **newImage = (Pixel **)malloc(sizeof(Pixel *) * h);
  newImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));   
  for( int i = 1 ; i < h ; i++ ) {
    newImage[i] = (*newImage + (w * i));
  }
  for( int i = heightStart; i < heightEnd; i++ ) {
    for( int j = widthStart; j < widthEnd; j++ ) {
       if( m == RED )
       {
          //image[i][j].red;     // unchanged
          image[i][j].green = 0;
          image[i][j].blue  = 0;
       }
       else if( m == GREEN )
       {
          image[i][j].red = 0;
          //image[i][j].green = 0; // unchanged
          image[i][j].blue  = 0;
       }
       else if( m == BLUE )
       {
          image[i][j].red = 0;
          image[i][j].green = 0;
          //image[i][j].blue  = 0;  // unchanged
       }
       newImage[i-heightStart][j-widthStart] = image[i][j];
       //printf("i: %d, j: %d\t", i, j);
    }
  }
  return newImage;
}

void ConvertPixelto1D(Pixel **image, unsigned char** arr1D, int height, int width){
  int x = 0;
  for( int i = 0 ; i < height ; i++ ) {
    for( int j = 0 ; j < width ; j++ ) {
      //printf("i: %d, j: %d\n", i, j);
      (*arr1D)[x+0] = image[i][j].red;
      (*arr1D)[x+1] = image[i][j].green;
      (*arr1D)[x+2] = image[i][j].blue;
      x += 3;
    }
  }
}

Pixel ** Convert1DtoPixel(unsigned char *arr1D, int height, int width){
  Pixel **newImage = (Pixel **)malloc(sizeof(Pixel *) * height);
  newImage[0] = (Pixel *)malloc(sizeof(Pixel) * (height * width));   
  for( int i = 1 ; i < height ; i++ ) {
    newImage[i] = (*newImage + (width * i));
  }
  int z = 0;
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      newImage[i][j].red = arr1D[z+0];
      newImage[i][j].green = arr1D[z+1];
      newImage[i][j].blue = arr1D[z+2];
      z += 3;
    }
  }
  return newImage;        
}

void CombineImagePieces(Pixel **mainImage, Pixel **partImage, int startHeight, int endHeight, int startWidth, int endWidth){
  for(int i=startHeight; i<endHeight; i++){
    for(int j=startWidth; j<endWidth; j++){
      //printf("i: %d, j: %d \n", i, j);
      mainImage[i][j] = partImage[i-startHeight][j-startWidth];
    }
  }
  return;
}

Pixel ** MergeImageHorizontal(Pixel **topImage, Pixel **botImage, int h1, int h2, int w){
  int h = h1 + h2;
  Pixel **newImage = (Pixel **)malloc(sizeof(Pixel *) * h);
  newImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));
  for( int i = 1 ; i < h ; i++ ) {
    newImage[i] = (*newImage + (w * i));
  }
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      //printf("i: %d, j: %d\t", i, j);
      if(i < h1){
        newImage[i][j] = topImage[i][j];
      }
      else{
        newImage[i][j] = botImage[i][j];
      }
    }
    //printf("\n");
  }
  return newImage;
}

Pixel ** MergeImageVertical(Pixel **leftImage, Pixel **rightImage, int w1, int w2, int h){
  int w = w1 + w2;
  Pixel **newImage = (Pixel **)malloc(sizeof(Pixel *) * h);
  newImage[0] = (Pixel *)malloc(sizeof(Pixel) * (h * w));
  for( int i = 1 ; i < h ; i++ ) {
    newImage[i] = (*newImage + (w * i));
  }
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      //printf("i: %d, j: %d\t", i, j);
      if(i < w1){
        newImage[i][j] = leftImage[i][j];
      }
      else{
        newImage[i][j] = rightImage[i][j];
      }
    }
    //printf("\n");
  }
  return newImage;
}

void DumpImage( Pixel **image, int height, int width )
{
   printf( "%d %d\n", height, width );
   
   for(int i = 0; i<height; i++) {
    for(int j = 0; j<width; j++) {
      //printf( "%d ", image[i][j].red );
      //printf( "%d", image[i][j].green;
      //printf( "%d", image[i][j].blue;
    }
    //printf( "\n" );
  }
}

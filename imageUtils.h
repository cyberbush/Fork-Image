/* imageUtils.h
 */

//#include "colorUtils.h"

/**
 * A structure that represents a single pixel value using
 * the RGB (red-blue-green) color model.  Each integer
 * value is in the range [0, 255].
 */
typedef struct {
  int red;
  int green;
  int blue;
} Pixel;

/* Loads an image file specified by the given file name.
   The height and width are indicated in the two pass-by-pointer variables.
   The image is returned as a two-dimensional array of Pixel values
   of dimension (height x width) where the pixel at [0][0] corresponds to
   the top-left most pixel value.
 */
Pixel **LoadImage(const char *filePath, int *height, int *width);

/* Saves an image (same format as LoadImage() and is of the given 
   height x width) to the file specified by the given name.
 */
void SaveImage(const char *filePath, Pixel **image, int height, int width);


/* Creates a deep copy of the given image of the given height/width,
   returning the new copy as a 2-dimensional array of pixels.
 */
Pixel **CopyImage(Pixel **image, int height, int width);

/* Convert image to grayscale using specified mode.
 */
void ImageToGrayScale( Pixel **image, int height, int width, Mode m );


/* Extract specific colors
 */
void ExtractRGB( Pixel **image, int height, int width, Mode m );
// Returns Pixel** and allows for start height and width
Pixel** ExtractPartRGB( Pixel **image, int heightStart, int heightEnd, int widthStart, int widthEnd, Mode m );
/* Prints image values 
*/
void DumpImage( Pixel **image, int height, int width );

/* Combine different smaller images into main image
*/
void CombineImagePieces(Pixel **mainImage, Pixel **partImage, int startHeight, int endHeight, int startWidth, int endWidth);

/* Merges two horzontal images
*/
Pixel ** MergeImageHorizontal(Pixel **image1, Pixel **image2, int h1, int h2, int w);

/* Merges two vertical images
*/
Pixel ** MergeImageVertical(Pixel **leftImage, Pixel **rightImage, int w1, int w2, int h);

/* Convert from 2D Pixel to 1D char array
*/
void ConvertPixelto1D(Pixel **image, unsigned char** part1, int height, int width);

/* Convert from 1D char array to 2d Pixel 
*/
Pixel ** Convert1DtoPixel(unsigned char *arr1D, int height, int width);


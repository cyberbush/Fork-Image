# Fork-Image
This repository contains 4 different versions of an image processing program. Each one uses fork() zero, one, two, or three times. 

The progams use fork() to create multiple child processes that all work concurrently on extracting the RGB components from an image. By extracting the RGB components it sets the image to gray, red, blue, or green scale. The programs also utilize inter process communication with mmap() to share each part and combine them back together at the end. 

## Usage
Once you have downloaded the code in this repository.

Use the line below to compile the code and produce an excutable:
```
cc threeFork_image.c imageUtils.c -o threeFork -lm
```
To run the program use the line below with any image you want to change and the mode number:
```
./threeFork input.jpg output.jpg 1
```

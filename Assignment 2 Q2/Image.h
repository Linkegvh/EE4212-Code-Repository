#include <stdint.h>
#include <stdlib.h>
#include <cstdio>

enum ImageType{
    PNG, JPG, BMP, TGA
};


struct Image{
    uint8_t* data = NULL;
    size_t size = 0;
    int w;
    int h;
    int channels; // How many colours are there

    Image(const char* filename); // constructor that takes in the file name
    Image(int w, int h, int channels); // Create a blank image
    Image(const Image& img); // Copy images
    ~Image();

    bool read(const char* filename); // Read in the image file
    bool write(const char* filename); // Write to an image file

    ImageType getFileType(const char* filename);

    Image& grayscale_avg();
    Image& grayscale_lum();
};
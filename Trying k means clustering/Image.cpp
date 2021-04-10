// This code was done following the instruction of code-break0
// His github: https://github.com/Code-Break0
#define STB_IMAGE_IMPLEMENTATION // Demanded by the header file
#define STB_IMAGE_WRITE_IMPLEMENTATION // Demanded by the header file
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

// constructor that takes in the file name
Image::Image(const char* filename){
    if(read(filename)){ // if managed to read the file without issue
        printf("Read %s\n", filename);
        size = w*h*channels;
    }else{
        printf("Failed to read %s\n", filename);
    }
}

// Create a blank image
Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels){
    size = w*h*channels;
    data = new uint8_t[size];

}

// Copy images
Image::Image(const Image& img) : Image(img.w, img.h, img.channels){
    memcpy(data, img.data, size);
}

// Remove the image
Image::~Image(){
    stbi_image_free(data);
}

// Read in the image file
bool Image::read(const char* filename){
    data = stbi_load(filename, &w, &h, &channels, 0); // the 0 at the end is to force the color channel to be a certain number
    return data != NULL; // Do not return the data if it is null
}

// Write to an image file
bool Image::write(const char* filename){
    // Check what the extension is
    ImageType type = getFileType(filename);

    int success;
    switch(type){
        case PNG:
            success = stbi_write_png(filename, w, h, channels, data, w*channels); 
            break;
        case JPG:
            success = stbi_write_jpg(filename, w, h, channels, data, 100); // Quality = 100, we want best quality
            break;
        case BMP:
            success = stbi_write_bmp(filename, w, h, channels, data);
            break;
        case TGA:
            success = stbi_write_tga(filename, w, h, channels, data);
            break;
    }
    return success != 0; // if success = 0, we have issues
}

ImageType Image::getFileType(const char* filename){
    const char* ext = strrchr(filename, '.'); // get the extension
    // Check it is valid first
    if (ext != nullptr){
        if (strcmp(ext, ".png") == 0){
            return PNG;
        }else if (strcmp(ext, ".jpg") == 0){
            return JPG;
        }else if (strcmp(ext, ".bmp") == 0){
            return BMP;
        }else if (strcmp(ext, ".tga") == 0){
            return TGA;
        }
    }
    return JPG; // if there are issues with the extension, treat it as jpg
}

// Normal grayscale 
Image& Image::grayscale_avg(){
    if(channels<3) {
        printf("Image %p has less than 3 channels, it is assumed to already be in greyscale", this);
    }else{
        for (int i = 0; i < size; i += channels){
            // (r+g+b)/3
            int gray = (data[i] + data[i+1] + data[i+2]) / 3;
            memset(data+i, gray, 3);
        }
    }
    return *this;
}

Image& Image::grayscale_lum(){
    // (r+g+b)/3 but a bit different
    if(channels<3) {
        printf("Image %p has less than 3 channels, it is assumed to already be in greyscale", this);
    }else{
        for (int i = 0; i < size; i += channels){
            // (r+g+b)/3
            int gray = (0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2]) / 3;
            memset(data+i, gray, 3);
        }
    }
    return *this;
}
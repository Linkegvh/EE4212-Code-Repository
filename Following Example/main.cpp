#include "Image.h"

int main(int argc, char** argv){
    Image test("denoise_input.jpg");
    
    Image gray_avg = test;
    gray_avg.grayscale_lum();
    gray_avg.write("gray_avg.jpg");
    /*
    test.write("new.jpg");

    Image copy = test;
    
    // make the first row white
    
    for (int i = 0; i < copy.w*copy.channels; ++i){
        copy.data[i] = 255;
    }
    

    copy.write("copy.jpg");
    Image blank(100, 100, 3);
    blank.write("blank.jpg");
    */
    return 0;
}


// apt install libglfw3-dev
// gcc main.c -o display -lglfw -lGL -lm

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>


#define WIDTH  640
#define HEIGHT 480
#define COMP 3


typedef struct __attribute__((packed)) {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} t_bmp_fileheader;

typedef struct __attribute__((packed)) {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} t_bmp_infoheader;

typedef struct {
    void *ptr;
    size_t size;
    t_bmp_fileheader *fileheader;
    t_bmp_infoheader *infoheader;
    void * data;
} t_image;


void invalidateImage(t_image *image) {
    if (image == NULL)
        return;

    free(image->ptr);

    image->ptr = NULL;
    image->size = 0;
    image->fileheader = NULL;
    image->infoheader = NULL;
    image->data = NULL;
}

//copies a file into the memory
//@param filename the filename to be read
//@return object containig pointer and size of the memory segment
t_image readFileToMemory(const char * filename){
  FILE *fp = fopen(filename, "rb"); 
  
  if(fseek(fp,0L, SEEK_END) != 0){
    //fprintf(stderr, "fseek failed\n");
    printf("fseek failed\n");
    fclose(fp);
    exit(1);
  }
  t_image mm;
  
  mm.size = ftell(fp);
  rewind(fp);
  mm.ptr = malloc(mm.size);
  mm.fileheader = (t_bmp_fileheader *) mm.ptr;
  mm.infoheader = (t_bmp_infoheader *) (mm.ptr + 14);
  mm.data = mm.ptr + mm.fileheader->bfOffBits;
  
  fread(mm.data, mm.size, 1, fp);
  fclose(fp);
  return mm;
}

void printImageInfo(t_image *image){
  printf("BITMAPFILEHEADER\n");
  printf("Offset: 0x00 Byte: 2 bfType : %x\n", image->fileheader->bfType);
  printf("Offset: 0x02 Byte: 4 bfSize : 0x%x\n",  image->fileheader->bfSize);
  printf("Offset: 0x06 Byte: 2 bfReserved1 : 0x%x\n", image->fileheader->bfReserved1);
  printf("Offset: 0x08 Byte: 2 bfReserved2 : 0x%x\n", image->fileheader->bfReserved2);
  printf("Offset: 0x0A Byte: 4 bfOffBits : 0x%x\n", image->fileheader->bfOffBits);

  printf("BITMAPINFOHEADER\n");  
  printf("Offset: 0x0E Byte: 4 biSize : 0x%x\n", image->infoheader->biSize);
  printf("Offset: 0x12 Byte: 4 biWidth : 0x%x (%u)\n", image->infoheader->biWidth, image->infoheader->biWidth);
  printf("Offset: 0x16 Byte: 4 biHeight : 0x%x (%d)\n", image->infoheader->biHeight, image->infoheader->biHeight);
  printf("Offset: 0x1A Byte: 2 biPlanes : 0x%x\n", image->infoheader->biPlanes);
  printf("Offset: 0x1C Byte: 2 biBitCount : 0x%x\n", image->infoheader->biBitCount);
  printf("Offset: 0x1E Byte: 4 biCompression : 0x%x\n", image->infoheader->biCompression);
  printf("Offset: 0x22 Byte: 4 biSizeImage : 0x%x\n", image->infoheader->biSizeImage);
  printf("Offset: 0x26 Byte: 4 biXPelsPerMeter : 0x%x\n", image->infoheader->biXPelsPerMeter);
  printf("Offset: 0x2A Byte: 4 biYPelsPerMeter : 0x%x\n", image->infoheader->biYPelsPerMeter);
  printf("Offset: 0x2E Byte: 4 biClrUsed : 0x%x\n", image->infoheader->biClrUsed);
  printf("Offset: 0x32 Byte: 4 biClrImportant : 0x%x\n",image->infoheader->biClrImportant); 
}

// pxl auslesen
void getPixel(t_image *image, int x, int y, uint8_t *r, uint8_t *g, uint8_t *b) 
{
    int width = image->infoheader->biWidth;
    int height = abs(image->infoheader->biHeight);

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int rowSize = width * 3; // wegen rgb * 3   --> byte größe
    while (rowSize % 4 != 0) {
        rowSize++;  }  // "padding bits" werden dann eh nd eingelesen beim lesen aber man muss 
        // laut bmp immer auf 4 bits auffüllen damit zeilen gleich groß --> leichter zum lesen

    
    int row = y;
    
    // line finden
    if (image->infoheader->biHeight > 0){
        row = height - 1 - y;}

    // pxl finden
    int position = row * rowSize + x * 3;

    
}







int main(void) {
    //https://www.glfw.org/docs/latest/quick.html
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Framebuffer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    
    //https://esahubble.org/images/heic1509a/
    clock_t start = clock();
    t_image image = readFileToMemory("heic1509a.bmp");

    printImageInfo(&image);

    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("time: %f\n",seconds);
    
    size_t biWidth = image.infoheader->biWidth;
    size_t biHeight = image.infoheader->biHeight;
    
    if(image.infoheader->biBitCount != 24 || image.infoheader->biCompression != 0){
      printf("unsupported\n");
      invalidateImage(&image);
      exit(3);
    }

    while (!glfwWindowShouldClose(window)) {

      int fbw, fbh;
      glfwGetFramebufferSize(window, &fbw, &fbh);
      glViewport(0, 0, fbw, fbh);

      //as we only support bottom->top bmp: start from -1/-1 and draw to 0/0 
      //setting this to -1/1 +  setting the pigelzoom to -1*fbh would invert it
      //https://learnopengl.com/Getting-Started/Coordinate-Systems
      glRasterPos2f(-1.0f, -1.0f);
      glPixelZoom((float)fbw / biWidth, (float)fbh / biHeight);
      //https://docs.gl/gl3/glDrawPixels
      glDrawPixels(biWidth, biHeight, GL_BGR, GL_UNSIGNED_BYTE, image.data);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glfwTerminate();
    invalidateImage(&image);
//    invalidateImage(&image1);
    return 0;
}

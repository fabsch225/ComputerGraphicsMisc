//
// Created by fabian on 9/16/25.
//

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <GL/freeglut.h>
#include <cuda_runtime.h>

int window_width = 1024;
int window_height = 768;

// Starting Point of the Complex Plane
double centerX = -0.75;
double centerY = 0.0;
double scale = 1.0;
int maxIter = 500;

// Util
unsigned char *d_img = nullptr;
unsigned char *h_img = nullptr;

#define CUDA_CHECK(call) do { \
    cudaError_t err = call; \
    if (err != cudaSuccess) { \
        fprintf(stderr, "CUDA error %s at %s:%d\n", cudaGetErrorString(err), __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

__device__ inline unsigned int color_map(float t) {
    float r = 9*(1-t)*t*t*t;
    float g = 15*(1-t)*(1-t)*t*t;
    float b = 8.5*(1-t)*(1-t)*(1-t)*t;
    unsigned char R = (unsigned char)(fminf(1.0f, r) * 255.0f);
    unsigned char G = (unsigned char)(fminf(1.0f, g) * 255.0f);
    unsigned char B = (unsigned char)(fminf(1.0f, b) * 255.0f);
    return (0xFFu << 24) | (B << 16) | (G << 8) | R; // ABGR for glDrawPixels with GL_RGBA + GL_UNSIGNED_BYTE might be platform dependent
}

// CUDA kernel: compute per-pixel Mandelbrot and write RGBA into output buffer
__global__ void mandelbrot_kernel(unsigned char *img, int width, int height,
                                  double centerX, double centerY, double scale,
                                  int maxIter)
{
    int px = blockIdx.x * blockDim.x + threadIdx.x;
    int py = blockIdx.y * blockDim.y + threadIdx.y;
    if (px >= width || py >= height) return;

    int idx = (py * width + px) * 4;

    double aspect = (double)width / (double)height;
    double x0 = centerX + ( ( (double)px / (double)width ) - 0.5 ) * scale * aspect;
    double y0 = centerY + ( ( (double)py / (double)height ) - 0.5 ) * scale;

    double x = 0.0;
    double y = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;

    int iter = 0;
    while (x2 + y2 <= 4.0 && iter < maxIter) {
        y = 2.0 * x * y + y0;
        x = x2 - y2 + x0;
        x2 = x * x;
        y2 = y * y;
        ++iter;
    }

    if (iter >= maxIter) {
        img[idx + 0] = 0;
        img[idx + 1] = 0;
        img[idx + 2] = 0;
        img[idx + 3] = 255;
    } else {
        double mag = sqrt(x2 + y2);
        double mu = iter + 1 - log(log(mag))/log(2.0);
        float t = (float)(mu / (float)maxIter);
        float rr = 9*(1-t)*t*t*t;
        float gg = 15*(1-t)*(1-t)*t*t;
        float bb = 8.5*(1-t)*(1-t)*(1-t)*t;
        unsigned char R = (unsigned char)(fminf(1.0f, rr) * 255.0f);
        unsigned char G = (unsigned char)(fminf(1.0f, gg) * 255.0f);
        unsigned char B = (unsigned char)(fminf(1.0f, bb) * 255.0f);
        img[idx + 0] = R;
        img[idx + 1] = G;
        img[idx + 2] = B;
        img[idx + 3] = 255;
    }
}

void compute_on_gpu()
{
    size_t numBytes = (size_t)window_width * (size_t)window_height * 4u;
    dim3 block(16, 16);
    dim3 grid((window_width + block.x - 1)/block.x, (window_height + block.y - 1)/block.y);

    mandelbrot_kernel<<<grid, block>>>(d_img, window_width, window_height, centerX, centerY, scale, maxIter);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(h_img, d_img, numBytes, cudaMemcpyDeviceToHost));
}

void display()
{
    compute_on_gpu();

    glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos2i(-1, 1);
    glPixelZoom(1, -1);

    glDrawPixels(window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, h_img);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    window_width = w;
    window_height = h;
    glViewport(0, 0, w, h);

    size_t numBytes = (size_t)window_width * (size_t)window_height * 4u;
    if (h_img) free(h_img);
    CUDA_CHECK(cudaFree(d_img));

    h_img = (unsigned char*)malloc(numBytes);
    if (!h_img) {
        fprintf(stderr, "Failed to allocate host image buffer\n");
        exit(EXIT_FAILURE);
    }
    CUDA_CHECK(cudaMalloc((void**)&d_img, numBytes));
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case '+':
        case '=':
            maxIter = (int)(maxIter * 1.2) + 1;
            printf("maxIter = %d\n", maxIter);
            break;
        case '-':
            maxIter = (int)(maxIter / 1.2);
            if (maxIter < 10) maxIter = 10;
            printf("maxIter = %d\n", maxIter);
            break;
        case 'w': centerY -= scale * 0.1; break;
        case 's': centerY += scale * 0.1; break;
        case 'a': centerX += scale * 0.1; break;
        case 'd': centerX -= scale * 0.1; break;
        case 'z': scale *= 0.8; break;
        case 'x': scale /= 0.8; break;
        case 'r':
            centerX = -0.75; centerY = 0.0; scale = 3.0; maxIter = 500; break;
        default:
            return;
    }
    glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Mandelbrot Set CUDA");

    size_t numBytes = (size_t)window_width * (size_t)window_height * 4u;
    h_img = (unsigned char*)malloc(numBytes);
    if (!h_img) {
        fprintf(stderr, "Failed to allocate host image buffer\n");
        return EXIT_FAILURE;
    }
    CUDA_CHECK(cudaMalloc((void**)&d_img, numBytes));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    printf("Controls: w/s/a/d pan, z/x zoom, +/- iterations, r reset, ESC exit\n");

    glutMainLoop();

    CUDA_CHECK(cudaFree(d_img));
    if (h_img) free(h_img);

    return 0;
}

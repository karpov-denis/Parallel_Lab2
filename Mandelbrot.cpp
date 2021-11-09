// Mandelbrot.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "gif.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include "mpi.h"
#include <ctime>

using namespace std;
#pragma comment(lib, "msmpi.lib")

bool mandelbrot(float imC, float reC, int& i)
{
    const int maxIter = 80;
    float d = 0.0f;
    float imZ = 0.0f;
    float reZ = 0.0f;
    for (i = 0; i < maxIter && d <= 2.0f; ++i)
    {
        float im = imZ * imZ - reZ * reZ + imC;
        float re = 2.0f * imZ * reZ + reC;
        imZ = im;
        reZ = re;
        d = sqrt(imZ * imZ + reZ * reZ);
    }
    return d < 2;
}

float mapToReal(int x, int imageWidth, float minR, float maxR)
{
    float range = maxR - minR;
    return x * (range / imageWidth) + minR;

}
float mapToImaginary(int y, int imageHeight, float minI, float maxI)
{
    float range = maxI - minI;
    return y * (range / imageHeight) + minI;

}
float GetSize(double OrSize, int frame)
{
    frame--;
    for (int i = 0; i < frame; i++)
    {
        OrSize /= 1.05;
    }
    return OrSize;
}

vector<uint8_t> GetFrame(int height, int width, int frame)
{

    
    int i = 0;
    int count = 0;
    uint8_t colors[5][3] =
    {
    {0xFF, 0x0, 0x0},
    {0x0, 0xFF, 0x0},
    {0x0, 0x0, 0xFF},
    {0x0,0xFF,0xFF},
    {0xFF,0xFF,0x0}
    };
    vector<uint8_t> pixel(static_cast<__int64>(width) * static_cast<__int64>(height) * 4 * 10);
    for (int iter = 0; iter < 10; iter++)
    {
        double yTek = GetSize(1, frame);
        double xTekPos = GetSize(1.3, frame);
        double xTekNeg = GetSize(1.4, frame);

        for (int y = 0; y < height; y++) //строки
        {
            for (int x = 0; x < width; x++) //пиксели в строке
            {

                float re = mapToReal(y, width, -1 * yTek, 1 * yTek);
                float im = mapToImaginary(x, height, -1 * xTekNeg, 1 * xTekPos);

                if (mandelbrot(im, re, i))
                {
                    pixel[count++] = pixel[count++] = pixel[count++] = 0x20;
                    pixel[count++] = 0xFF;
                }
                else
                {
                    pixel[count++] = colors[i % 5][0];
                    pixel[count++] = colors[i % 5][1];
                    pixel[count++] = colors[i % 5][2];
                    pixel[count++] = 0xFF;
                }

            }
        }
        frame++;
    }
    return pixel;
}

int main(int* argc, char** argv)
{
    int numtasks, rank;
    MPI_Init(argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    float centerX = -0.7;
    float centerY = 0;
 //   int amount = 8;
    unsigned int end_MPI;
    int height = 2000;

    int width = height;

    vector<uint8_t> pixel(static_cast<__int64>(width) * static_cast<__int64>(height) * 4 * 10);

    float im = 0;
    float re = 0;
    int x = 0;
    int y = 0;
    

    auto fileName = "outputMPI.gif";
    int delay = 10;
    GifWriter g;
    double xCenter = -0.7;
    unsigned int start_time = clock();
    unsigned int startMPI = MPI_Wtime();

    if (rank == 0)
    {
        GifBegin(&g, fileName, width, height, delay);
        //pixel = GetFrame(height, width, 0);
        //for (int i = 0; i < 10; i++)
        //{
        //    vector<uint8_t> pixel2;
        //    pixel2.insert(pixel2.begin(), pixel[i * width * height * 4], pixel[(i + 1) * width * height * 4]);
        //    GifWriteFrame(&g, pixel2.data(), width, height, delay);
        //}
        for (int i = 1; i < numtasks; i++)
        {
            MPI_Status status;
            MPI_Recv(pixel.data(), width * height * 4 * 10, MPI_UINT8_T, i, 1, MPI_COMM_WORLD, &status);
            vector<uint8_t>::iterator start;
            vector<uint8_t>::iterator end;
            for (int j = 0; j < 10; j++)
            {
                start = pixel.begin();
                end = pixel.begin();
                advance(start, width * height * 4 *j);
                advance(end, width * height * 4 *(j+1) );
                vector<uint8_t> pixel2;
                pixel2.insert(pixel2.begin(), start,end);
                GifWriteFrame(&g, pixel2.data(), width, height, delay);

            }
        }
        GifEnd(&g);
        unsigned int end_time = clock();
        end_MPI = MPI_Wtime();
        unsigned int search_time = end_time - start_time;
        printf("Time: %i",search_time);
        printf("TimeWithMPI: %i", end_MPI);
        printf("TimeDiffMPI: %i", end_MPI - startMPI);
    }
    else
    {
        pixel = GetFrame(height, width, (rank-1)*10);
        MPI_Send(pixel.data(), width * height * 4*10, MPI_UINT8_T, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 1;
}


int main2(int *argc, char **argv)
{
    float centerX = -0.7;
    float centerY = 0;
    int amount = 8;
    int frames = 6;
    int height = 1000;

    int width = height;

    vector<uint8_t> pixel(static_cast<__int64>(width) * height * 4*10);

    float im = 0;
    float re = 0;
    int x = 0;
    int y = 0;


    auto fileName = "output.gif";
    int delay = 100;
    GifWriter g;
    double xCenter = -0.7;
    unsigned int start_time = clock();


        GifBegin(&g, fileName, width, height, delay);
        pixel = GetFrame(height, width, 0);
        vector<uint8_t>::iterator start;
        vector<uint8_t>::iterator end;
        //for (int i = 0; i < 10; i++)
        //{
        //    start = pixel.begin();
        //    end = pixel.begin();
        //    advance(start, width * height * 4 * i);
        //    advance(end, width * height * 4 * (i + 1));
        //    vector<uint8_t> pixel2;
        //    pixel2.insert(pixel2.begin(), start, end);
        //    GifWriteFrame(&g, pixel2.data(), width, height, delay);
        //}
        for (int j = 1; j < frames; j++)
        {
            pixel = GetFrame(height, width, 0);
            for (int i = 0; i < 10; i++)
            {
                start = pixel.begin();
                end = pixel.begin();
                advance(start, width * height * 4 * i);
                advance(end, width * height * 4 * (i + 1));
                vector<uint8_t> pixel2;
                pixel2.insert(pixel2.begin(), start, end);
                GifWriteFrame(&g, pixel2.data(), width, height, delay);
            }
        }

        GifEnd(&g);
        unsigned int end_time = clock();
        unsigned int search_time = end_time - start_time;

        printf("Time: %i", search_time);


    return 1;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

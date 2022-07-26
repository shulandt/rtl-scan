#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//------------------------------------------------------------------------------
#define MAX_LEN_BUFF 1024 * 1024
//------------------------------------------------------------------------------
void spectreAnalysis(float* dB, int n, float beginFreq, float endFreq, float stepFreq, char* date, char* time, FILE* outFile);
//------------------------------------------------------------------------------
float nextFloat(char** delimPtr)
{
    *delimPtr = strchr(*delimPtr, 0);
    *delimPtr += 2;
    return atof(*delimPtr);
}
//------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    char inFileName[255]  = "rtl-power.csv";
    char outFileName[255] = "found-signals.csv";

    if(argc > 1)
    {
        strcpy(inFileName, argv[1]);
    }
    if(argc > 2)
    {
        strcpy(outFileName, argv[2]);
    }
    FILE* inFile = fopen(inFileName, "rt");
    if(inFile == NULL)
    {
        printf("no input file\n");
        return -1;
    }
    char* strBuff = (char*)malloc(MAX_LEN_BUFF);
    if(strBuff == NULL)
    {
        printf("out of memory\n");
        return -1;
    }
    FILE* outFile = fopen(outFileName, "rt");
    if(outFile == NULL)
    {
        outFile = fopen(outFileName, "wt");
        if(outFile == NULL)
        {
            printf("access denied\n");
            return -1;
        }
        fprintf(outFile, "date,time,freq,dB\n");
    }
    else
    {
        fclose(outFile);
        outFile = fopen(outFileName, "at");
        if(outFile == NULL)
        {
            printf("access denied\n");
            return -1;
        }
    }
    int numStr = 0;
    while(fgets(strBuff, MAX_LEN_BUFF, inFile))
    {
        int i = 0;
        int count = 0;
        while(strBuff[i] != 0)
        {
            if(strBuff[i] == ',')
            {
                strBuff[i] = 0;
                count++;
            }
            i++;
        }
        float* dB = (float*)malloc((count - 5) * sizeof(float));
        if(dB == NULL)
        {
            printf("out of memory\n");
            return -1;
        }
        char date[11];
        strcpy(date, strBuff);
        char* delimPtr = strchr(strBuff, 0);
        delimPtr += 2;
        char time[9];
        strcpy(time, delimPtr);
        float beginFreq = nextFloat(&delimPtr);
        float endFreq = nextFloat(&delimPtr);
        float stepFreq = nextFloat(&delimPtr);
        float samples = nextFloat(&delimPtr);
        for(int k = 0; k < count - 5; k++)
          dB[k] = nextFloat(&delimPtr);
        spectreAnalysis(dB, count - 5, beginFreq, endFreq, stepFreq, date, time, outFile);
        free(dB);
        numStr++;
    }
    free(strBuff);
    fclose(inFile);
    fclose(outFile);
	return 0;
}
//------------------------------------------------------------------------------
void spectreAnalysis(float* dB, int n, float beginFreq, float endFreq, float stepFreq, char* date, char* time, FILE* outFile)
{
    // коррекция выборки после фильтра (убираем ненужный всплеск посередине)
    dB[n / 2 - 1] = dB[n / 2] = dB[n / 2 + 1] = dB[n / 2 + 2];
    // расчет минимального, максимального и среднего значения
    float maxdB = -100.f;
    int maxIndex = -1;
    float mindB = 0.f;
    int minIndex = -1;
    float middB = 0.f;
    for(int i = 0; i < n; i++)
    {
        middB += dB[i];
        if(dB[i] > maxdB)
        {
            maxdB = dB[i];
            maxIndex = i;
        }
        if(dB[i] < mindB)
        {
            mindB = dB[i];
            minIndex = i;
        }
    }
    middB /= n;
    // расчет СКО
    float rmsdB = 0.f;
    for(int i = 0; i < n; i++)
    {
        rmsdB += (dB[i] - middB) * (dB[i] - middB);
    }
    rmsdB = sqrt(rmsdB / n);
    // проверка найденного максимума на похожесть сигнала
    float threshold = middB + rmsdB * 3.f;
    if((maxdB > threshold + rmsdB * 3.f) &&
       (dB[maxIndex - 1] > threshold + rmsdB * 2.0f) &&
       (dB[maxIndex - 2] > threshold + rmsdB * 1.5f) &&
       (dB[maxIndex - 3] > threshold + rmsdB * 1.0f) &&
       (dB[maxIndex - 4] > threshold + rmsdB * 0.5f) &&
       (dB[maxIndex - 5] > threshold) &&
       (dB[maxIndex + 1] > threshold + rmsdB * 2.0f) &&
       (dB[maxIndex + 2] > threshold + rmsdB * 1.5f) &&
       (dB[maxIndex + 3] > threshold + rmsdB * 1.0f) &&
       (dB[maxIndex + 4] > threshold + rmsdB * 0.5f) &&
       (dB[maxIndex + 5] > threshold))
    {
        printf("%s %s found signal, freq = %.3f MHz, snr = %.1f dB\n", date, time, (beginFreq + maxIndex * stepFreq) * 1e-6, maxdB - middB);
        fprintf(outFile,"%s,%s,%.3f,%.1f\n", date, time, (beginFreq + maxIndex * stepFreq) * 1e-6, maxdB - middB);
    }
}
//------------------------------------------------------------------------------

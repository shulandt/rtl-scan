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
#define MAX_LEN_BUFF                   1024 * 1024
#define MAX_SIGNALS                    30
#define FREQ_MIN_INTERVAL_DEFAULT      0.1   // MHz
#define FREQ_MIN_BANDWIDTH_DEFAULT     0.003 // MHz
#define MIN_BANDWIDTH_STRENGTH_DEFAULT 10    // dB
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
	return 0;}
//------------------------------------------------------------------------------
void spectreAnalysis(float* dB, int n, float beginFreq, float endFreq, float stepFreq, char* date, char* time, FILE* outFile)
{
	int maxArray[MAX_SIGNALS];
	for(int i = 0; i < MAX_SIGNALS; i++)
		maxArray[i] = -1;
	// коррекци€ выборки после фильтра (убираем ненужный всплеск посередине)
	dB[n / 2 - 1] = dB[n / 2] = dB[n / 2 + 1] = dB[n / 2 + 2];
	// расчет среднего и минимального значени€
	float mindB = 0.f;
	int minIndex = -1;
	float middB = 0.f;
	for(int i = 0; i < n; i++)
	{
		if((i >= (n / 2) - 10) && (i <= (n / 2) + 10))
			continue;
		middB += dB[i];
		if(dB[i] < mindB)
		{
			mindB = dB[i];
			minIndex = i;
		}
	}
	middB /= n;
	// расчет — ќ
	float rmsdB = 0.f;
	for(int i = 0; i < n; i++)
	{
		rmsdB += (dB[i] - middB) * (dB[i] - middB);
	}
	rmsdB = sqrt(rmsdB / n);
	// заполнение массива максимальных сигналов с учетом минимального частотного
	// интервала, расчет и контроль ширины спектра сигнала
	for(int j = 0; j < MAX_SIGNALS; j++)
	{
		float maxdB = -100.f;
		for(int i = 0; i < n; i++)
		{
			if((dB[i] > middB + 10.f) && (dB[i] > maxdB))
			{
				int k = 0;
				for(; k < j; k++)
				{
					if(fabs(((i - maxArray[k]) * stepFreq) * 1e-6) < FREQ_MIN_INTERVAL_DEFAULT)
						 break;
				}
				if(k == j)
				{
					maxdB = dB[i];
					maxArray[j] = i;
                }
			}
		}
		if(maxArray[j] >= 0)
		{
			printf("\r%s %s found signal, freq = %.3f MHz, snr = %.1f dB\n", date, time, (beginFreq + maxArray[j] * stepFreq) * 1e-6, maxdB - middB);
			fprintf(outFile,"%s,%s,%.3f,%.1f\n", date, time, (beginFreq + maxArray[j] * stepFreq) * 1e-6, maxdB - middB);
		}
	}
}
//------------------------------------------------------------------------------

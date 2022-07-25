#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//------------------------------------------------------------------------------
#define MAX_LEN_BUFF 1024 * 1024
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
    char fileName[255] = "rtl-power.csv";

    if(argc > 1)
    {
        strcpy(fileName, argv[1]);
    }
    FILE* inFile = fopen(fileName, "rt");
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
        printf("numStr=%d count=%d\n", numStr, count);
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
        free(dB);
        numStr++;
    }
    getchar();
    free(strBuff);
	return 0;
}
//------------------------------------------------------------------------------
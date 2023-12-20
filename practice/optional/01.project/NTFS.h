#include <windows.h>
#include <iostream>
#include<math.h>
#include<bitset>
#include<string>
using namespace std;

class NTFS{
    // NTFS boot sector infomation
    private: 
        char* type;
        int bytePerSector;
        int sectorPerCluster;
        int mediaDescription;
        int sectorPerTrack;
        int numberOfHead;
        int hiddenSector;
        int totalSector;
        int MFT;
        int MFTMirror;
        int clusterPerRecord;   // giá trị này được biểu diễn ở dạng bù 2
        int clusterPerIndex; 
        BYTE volumeNumber[8];
        int checksum;
    public:


    // main method
        bool ReadSector(LPCSTR, int, BYTE[], int);      // read size byte from one position from the volume 
        void ExtractInfomation(BYTE[]);   // read the boot sector infomation

    // support method
        long long ConvertToDecimal(BYTE[], int, int);       // byte to decimal
        char* ConvertToString(BYTE[], int, int);            // byte to char*

};

bool NTFS::ReadSector(LPCSTR drive, int readPoint, BYTE sector[], int size){
     int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,OPEN_EXISTING,0,NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 0;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, size, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
        return 0;
    }
    else
    {
        printf("Success!!\n");
        return 1;
    }
}



long long NTFS::ConvertToDecimal(BYTE x[], int start, int end){     
    long long output = 0;
   

    for(int i = end ; i >= start; --i ){
        long long temp = 0;
       
        output += (long long)pow(16,((i-start)*2))* x[i];
    }
    return output; 
}
char* NTFS::ConvertToString(BYTE x[], int start, int end){    
    char* output = new char[end-start+1];
    for(int i = start; i <= end; ++i){
        int temp = 0;
        output[i-start] =  x[i]; 
    }
    return output;
}

void NTFS::ExtractInfomation(BYTE sector[]){
    type = ConvertToString(sector,3,9);
    bytePerSector = ConvertToDecimal(sector,11,12);
    sectorPerCluster = ConvertToDecimal(sector,13,13);
    mediaDescription = ConvertToDecimal(sector,21,21);
    sectorPerTrack = ConvertToDecimal(sector,24,25);
    numberOfHead = ConvertToDecimal(sector,26,27);
    hiddenSector = ConvertToDecimal(sector,28,31);
    totalSector = ConvertToDecimal(sector,40,47);
    MFT = ConvertToDecimal(sector,48,55);
    MFTMirror = ConvertToDecimal(sector,56,63);
    int t = sector[64];
    if(t >=128)
        t -= 256;
    clusterPerRecord = pow(2,abs(t));
    clusterPerIndex = ConvertToDecimal(sector,68,68);
  
    for(int i = 72; i <= 79; ++i){
        volumeNumber[i-72] = sector[i];
    }
    checksum = ConvertToDecimal(sector,80,83);


    //print infomation

    printf("================== THE IMPORTANT INFOMATION IN BOOT SECTOR NTFS =================\n\n\n");
    printf("\tType : %s\n",type);
    printf("\tByte per sector: %d \n", bytePerSector);
    printf("\tSector per cluster: %d \n", sectorPerCluster);
    printf("\tMedia Scription: %d \n", mediaDescription);
    printf("\tSector per track:  %d \n", sectorPerTrack);
    printf("\tTotal sector: %d \n", totalSector);
    printf("\tNumber of head: %d \n", numberOfHead);
    printf("\tHiden sector: %d \n", hiddenSector);
    printf("\tLogical cluster number for the file MFT: %d \n", MFT);
    printf("\tLogical cluster number for the file MFT Mirror: %d\n",MFTMirror);
    printf("\tCluster per file record segment: %d \n", clusterPerRecord );
    printf("\tCluster per index block: %d \n",clusterPerIndex);
    printf("\tVolume serial number: " );
    for(int i = 0 ; i < 8; ++i)
        printf("%3x",volumeNumber[i]);
    printf("\n");
    printf("\tChecksum: %d \n", checksum);
}
#include "NTFS.h"

int main(int argc, char** argv){
   BYTE sector[1024];
   NTFS test;
   test.ReadSector(reinterpret_cast<LPCSTR>("\\\\.\\D:"),0,sector,512);
   test.ExtractInfomation(sector);
   system("pause");
}


#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
int main(int argc, char *argv[]) {
  Disk disk_run;

  unsigned char buffer[BLOCK_SIZE];
  Disk::readBlock(buffer, 7000);
  char message[] = "hello";
  memcpy(buffer + 20, message, 6);
  Disk::writeBlock(buffer, 7000);

  unsigned char buffer2[BLOCK_SIZE];
  char message2[6];
  Disk::readBlock(buffer2, 7000);
  memcpy(message2, buffer2 + 20, 6);
  std::cout << message2;

  // for(int i = 0; i < 4; i++) {
  //   unsigned char buffer[BLOCK_SIZE];
  //   Disk::readBlock(buffer, i);
  //   std::cout << "Block " << i << ": ";
  //   for(int j = 0; j < BLOCK_SIZE; j++) {
  //     std::cout << (int)buffer[j] << " ";
  //   }
  //   std::cout << std::endl;
  // }

  return 0;
}
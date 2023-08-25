#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <math.h>
//sizes
#define size 256
#define pagemasak 0xFFFF
#define offsetmask 0xFF
#define adresslen 10
#define tlbsize 16

 FILE *addresses;
    FILE *bakingstore;
    struct page_table
    {
        int pageNumber;
        int frameNumber;
    };
    int physicalMemory [size][size];
    struct page_table tlb[tlbsize];
    struct page_table pt[size];
   
    signed char buf[size];
    int currentavilFrame=0;
    int currentavilPTIN=0;
    signed char v;
    void getPage(int virtual_address);
    int readstore(int pageNum);
    void inserttotlb(int pagenum,int framenum);
    void getPage(int virtual_address){
        int pageNum=(virtual_address & pagemasak)>>8;
        int offset=(virtual_address & offsetmask);
        int framenum=-1;
           
            for(int i=0;i<currentavilPTIN;i++){
                if(pt[i].pageNumber==pageNum){
                    framenum=pt[i].frameNumber;
                    break;
                }
            }
        // printf("framenum:%d",framenum);
        if(framenum==-1){
          framenum=readstore(pageNum);
        }
       
        v=physicalMemory[framenum][offset];
        printf("virtual address: %d    physical address:%d     value:%d\n",virtual_address,(framenum << 8)| offset, v);

    }
    int readstore(int pageNum){
        signed char lbuff[size];
if(fseek(bakingstore,pageNum*size,SEEK_SET)!=0){
    printf("error seeking baking store\n");
exit(-1);
}
if(fread(lbuff,sizeof(signed char),size,bakingstore)==0){
printf("error reading from baking store \n");
exit(-1);
}
for(int i=0;i<size;i++){
    physicalMemory[currentavilFrame][i]=lbuff[i];
}
pt[currentavilPTIN].pageNumber=pageNum;
pt[currentavilPTIN].frameNumber=currentavilFrame;
currentavilFrame++;
currentavilPTIN++;
return currentavilFrame-1;
    }
int main(int argc,char *argv[]){
   
    if(argc != 2){
        printf("no file entered\n");
        exit(-1);
    }
    if((addresses=fopen(argv[1],"r"))==NULL){
        printf("error opening addresses\n");
    exit(-1);
    }
    if((bakingstore=fopen("BACKING_STORE.bin","r"))==NULL){
        printf("error opening baking store \n");
        exit(-1);
    }
    int virtual_address;
    char address[adresslen];
    while(fgets(address,adresslen,addresses)!=NULL){
        virtual_address=atoi(address);
        getPage(virtual_address);

    }
    fclose(addresses);
    fclose(bakingstore);
    
       

}
//
//  main.cpp
//  DefconChal2
//
//  Created by thing2 on 5/20/16.
//  Copyright © 2016 thing2. All rights reserved.
//

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
unsigned int dhBase = 3789571307;
unsigned int dhGenerator =2;
void * halloc(uint32_t size){
	return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
}
void hfree(void * ptr){
	HeapFree(GetProcessHeap(),NULL,ptr);
}

unsigned long long mod_pow(unsigned long long  base, unsigned long long exponent, unsigned long modulus){
    if (modulus == 1){
        return 0;
    }
    unsigned long long result =1;
    base = base % modulus;
    while (exponent > 0) {
        if(exponent % 2 == 1){
            result = (result * base) % modulus;
        }
        exponent >>=1;
        base = (base * base) % modulus;
    }
    return result;
    
}
void encipher(uint32_t v[2], uint32_t const k[4]) {
#ifdef DEBUGCIPHER
	return;
#endif
    int num_rounds=64;
    unsigned int i;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 3925015707;
    for (i = 0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}


void decipher(uint32_t v[2], uint32_t const k[4]) {
#ifdef DEBUGCIPHER
	return;
#endif
    int num_rounds=64;
    unsigned int i;
	unsigned int t1,t2,t3,t4;
    uint32_t v0 = v[0], v1 = v[1], delta = 3925015707, sum = delta*num_rounds;
    for (i = 0; i < num_rounds; i++) {
		t1=(((v0 << 4) ^ (v0 >> 5)) + v0);
		t2=(sum + k[(sum >> 11) & 3]);
        v1 -=  t1 ^ t2 ;
        sum -= delta;
		t3 =(((v1 << 4) ^ (v1 >> 5)) + v1);
        t4 =(sum + k[sum & 3]);
        v0 -= t3 ^ t4;
    }
    v[0] = v0;
    v[1] = v1;
}
//BUG ONE Shitty leak
void writen(uint32_t * buf, uint32_t n, uint32_t * key){
    for(int i =0; i < n; i+=2){
		if(key != NULL)
			encipher(buf,key);
		printf("%08x %08x ",buf[i],buf[i+1]);
		if(key != NULL)
			decipher(buf,key);
    }
    printf("\n");
}
void readn(uint32_t * buf, uint32_t n,uint32_t*  key){
    for(int i =0; i < n; i++){
        scanf("%ux",&buf[i]);
    }
	if(n==1){
		printf("N=%08x\n",buf[1]);
	}
	for(int i =0; i < n; i=i+2){
		if(key != NULL)
	        decipher(&buf[i], key);
    }

}
uint32_t * keyAgreement(uint32_t * agreeKey){
    srand(time(NULL));
    uint32_t dhGen[4];
    uint32_t key[4] ;
	agreeKey = (uint32_t *) halloc(sizeof(uint32_t)*4);
    for(int i=0; i <4; i++){
		key[i] = rand()<<16| rand();
		key[i] = key[i]% dhBase;
        dhGen[i] = mod_pow(dhGenerator, key[i], dhBase);
    }
    printf("%08x %08x %08x %08x\n", dhGen[0],dhGen[1],dhGen[2],dhGen[3]);
    readn(agreeKey,4,NULL);
    for(int i=0; i <4; i++){
		if(agreeKey[i]<2)
			exit(-1);
        agreeKey[i] = mod_pow( agreeKey[i],key[i], dhBase);
    }
    return agreeKey;
}


void decipherBuffer (uint32_t * buf, uint32_t * key, unsigned int numBlocks){
    for(int i =0; i < numBlocks; i=i+2){
        decipher(&buf[i], key);
    }
}
void encipherBuffer (uint32_t * buf, uint32_t * key, unsigned int numBlocks){
    for(int i =0; i < numBlocks; i=i+2){
        encipher(&buf[i], key);
    }
}




int main(int argc, const char * argv[]) {
    // insert code here...
	setvbuf(stdout, NULL, _IONBF, 0);
	uint32_t agreeKey[4] = {1,2,3,4};
#ifndef DEBUGCIPHER
	keyAgreement(agreeKey);
#endif
    uint32_t ** buffers = (uint32_t**)halloc(512);
    uint32_t bufferPointer =0;
    while(1){
        uint32_t buf[2] = {0};
        int whichBuffer;
		readn(buf,2,agreeKey);
		whichBuffer = buf[1];
        uint32_t * toUse;
        if(bufferPointer>63){
            exit(-1);
        }
        switch(buf[0]){
            case 1:
				 if(whichBuffer < 2048){
                    toUse =(uint32_t*) halloc(whichBuffer+4);
                }else{
                    exit(-1);
                }
                toUse[0] = buf[1];
				readn(&toUse[1],buf[1]/4,agreeKey);
                buffers[bufferPointer++] = toUse;
                break;
            case 2:
                //Send back the buffer
                if(buf[1] < 64 && buffers[buf[1]] !=0){
                    writen(&(buffers[buf[1]][1]),
                           buffers[buf[1]][0]/4,
						   agreeKey);
                }
                break;
            case 3:
                //Free buffer, 5 not there
                if(buf[1] < 64 && buffers[buf[1]] !=0){
                    hfree(buffers[buf[1]]);
                    buffers[buf[1]]=0;
                }
                else{
                    exit(-1);
                }
                break;
            case 4:
				{
				HMODULE * hm = (HMODULE*)halloc(sizeof(HMODULE)+4);
				*(uint32_t*)hm = (uint32_t) sizeof(HMODULE);
				hm+=4;
				*hm = LoadLibrary(L"ntdll.dll");
				}
                break;
            case 5:
            {// enables bug two
                int b1 =buf[1]&64;
                int b2 =(buf[1]>>8)&63;
                if(buffers[b1]==NULL || buffers[b2]==NULL){
                    exit(-1);
                }
				
                memcpy(&buffers[b1][1],&buffers[b2][1], buffers[b1][0]);
				buffers[b1][0] = buffers[b2][0];
                hfree(buffers[b2]);
                break;
            }
            case 6: // read one
            {
                uint32_t offset[2];
                uint32_t * myBuf = buffers[buf[1]];
				char toEnc[8] = {0};
				readn(offset,1,agreeKey);
                if (offset[0] < myBuf[0]/4){
                    sprintf(toEnc, "%08x\n",  myBuf[offset[0]+1]);
                }
                
            }   break;
            case 7:
            {
                uint32_t offset[2] ;
				uint32_t * myBuf = buffers[buf[1]];
				readn(offset,1,agreeKey);
                if (offset[0] < myBuf[0]/4){
					readn( &myBuf[offset[0]+1], 1,agreeKey);
                }
            }   break;
            case 8:
			{
				uint32_t * val;
				LPSYSTEM_INFO lpinfo = (LPSYSTEM_INFO) halloc(sizeof(SYSTEM_INFO)+4);
				val = (uint32_t *)lpinfo;
				lpinfo+=4;
				val[0] = sizeof(SYSTEM_INFO);
				GetSystemInfo(lpinfo);
				buffers[bufferPointer*2] =(uint32_t*) lpinfo;
				bufferPointer++;
			}
                break;
            default:
				exit(-1);
                break;
                
                
        }
        
    }
    std::cout << "Hello, World!\n";
    return 0;
}

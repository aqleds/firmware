
typedef unsigned long int  DWORD;
typedef unsigned short  WORD;

#define NUM_ITERATIONS            32u

void XTEA_Decode(DWORD *data, WORD len);

DWORD        cbc[2];

const DWORD        key[4]={0xc8919304, 0xc1009441,0x09180002,0xacc8718a};

void XTEA_InitCBC()
{
    cbc[0]=0;
    cbc[1]=0;
}

void XTEA_Decode(DWORD* data, WORD len)
 {
    DWORD t1,t2;
    DWORD        DELTA;
    int         decode_i;
    DWORD        x1,x2,sum;

     DELTA = 0x9E3739B9;

     decode_i=0;

     while(decode_i < len)
     {
         sum = DELTA * NUM_ITERATIONS;
         x1=*data;
         x2=*(data+1);
         t1 = x1;
         t2 = x2;
         while(sum != 0u)
         {
             x2 -= ((x1<<4 ^ x1>>5) + x1) ^ (sum + *(key+(sum>>11&0x03)));
             sum-=DELTA;
             x1 -= ((x2<<4 ^ x2>>5) + x2) ^ (sum + *(key+(sum&0x03)));
         }
         *(data++)=x1^cbc[0];
         *(data++)=x2^cbc[1];

         cbc[0]^=t1;
         cbc[1]^=t2;

         decode_i += 8;
     }
}


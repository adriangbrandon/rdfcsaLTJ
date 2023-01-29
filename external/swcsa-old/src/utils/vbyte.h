#ifndef _VBYTE
#define _VBYTE

#define DEC_TO_BC(bc, pos, num)             \
{ while (num >= 128L) {                       \
 bc[pos++] = (byte)((num & 127L) + 128L);     \
 num = (num >> 7L);                          \
}                                           \
bc[pos++] = (byte) num;                     \
}

#define SIZE_DEC_TO_BC(num,size)            \
{size_t numtmp=num;                           \
  size=0;                                   \
  while (num >= 128L) {                      \
   size++;                                  \
   num = (num >> 7L);                        \
  }                                         \
size++; num=numtmp;                         \
}			
			
#define BC_TO_DEC(bc,pos,num) \
 { \
 	 register size_t _pot, _x; \
 	 _pot=1L; \
 	 _x = bc[pos++]; \
 	 num=0; \
 	 while (_x >= 128L) { \
 	 	num = num + (_x-128L)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot<<=7L; \
 	 } \
 	 num=num + _x*_pot; \
 }



//void check_BC_works(size_t limit){
//	size_t num, size,i;
//	num=0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//
//	num=128*128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=128*128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	num=128*128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6lu is encoded with %lu bytes",(ulong)num,(ulong)size);
//	
//	
//	byte code[10];
//	for (i=0;i<limit;i+=100L) {
//		size_t pos =0;
//		num =i;
//		DEC_TO_BC(code, pos, num);
//		pos =0;
//		BC_TO_DEC(code,pos,num);
//		if (i != num) {fprintf(stderr,"\n DISTINTOS !!!!");
//			return ;
//		}
//	}	
//	fflush(stderr);
//	if (i != num) {fprintf(stderr,"\n ENCODE/DECODE VBYTE WORKED FROM 0 to %lu  !!!!\n", (ulong)limit);
//
//}

#endif

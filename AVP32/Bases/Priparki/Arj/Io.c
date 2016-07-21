
static void NEAR
init_getbits()
{
	bitbuf = 0;
	subbitbuf = 0;
	bitcount = 0;
	fillbuf(2 * CHAR_BIT);
}

static uint incnt=0;
static uchar *inptr;



static void NEAR
fillbuf(n)                /* Shift bitbuf n bits left, read n bits */
int n;
{
//Printf("Fillbuff %d",n);

	if(n>(2 * CHAR_BIT)){
#ifdef _DEBUG
Printf("Error 1");
#endif
		corrupt=1;
	}
	bitbuf = (bitbuf << n) & 0xFFFF;  /* lose the first n bits */
	while (n > bitcount && !corrupt)
	{
		bitbuf |= subbitbuf << (n -= bitcount);
		if (compsize != 0)
		{
			compsize--;
			if (incnt == 0) {
				uint rsiz=(last_len>INBUFSIZ)?INBUFSIZ:(uint)last_len;
//#ifdef _DEBUG
//Printf("P_Read %d",rsiz);
//#endif
				incnt = P_Read(inbuf, (WORD)rsiz);
				if(incnt != rsiz){
#ifdef _DEBUG
Printf("Error 2");
#endif
		corrupt=1;
				}
				last_len-=incnt;
				inptr = inbuf;
			}
			subbitbuf = *inptr++;
			--incnt;
		}
		else
			subbitbuf = 0;
		bitcount = CHAR_BIT;
	}
	bitbuf |= subbitbuf >> (bitcount -= n);
}

static ushort NEAR getbits(int n)
{
	ushort x;
	x = bitbuf >> (2*CHAR_BIT - n);
	fillbuf(n);
	return x;
}
/*
 * ---------------------------------------------------------------------------
 * The data compression procedures are located in this module.
 *
 */

#include "arjpack.h"
#include <string.h>


tVOID flush_compdata(ArjPackData* data);
tPTR ArjAlloc(ArjPackData* data, tINT a);
tVOID ArjFree(ArjPackData* data, tPTR);
tPTR  __cdecl _memmove(tPTR, const tPTR, tUINT);
//tUINT check_multivolume(tUINT increment);
tDWORD fread_crc(tPTR buffer, tDWORD count, hSEQ_IO stream);
tVOID init_putbits(ArjPackData* data);
tVOID shutdown_putbits(ArjPackData* data);
tDWORD file_write(tBYTE* out_buffer, tDWORD out_bytes, hSEQ_IO hOutputIo);
tQWORD _ftell(hSEQ_IO hOutputIo);

//--
//tDWORD uncompsize;
//tINT unpackable;
//--


/* Inline functions */

#define error(a) {return ;}

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


#define encode_c(c)                     \
        putbits(data, data->c_len[c], data->c_code[c])

#define encode_p(p)                     \
{                                       \
 tUINT qc, q;                    \
 qc=0;                                  \
 q=p;                                   \
 while(q)                               \
 {                                      \
  q>>=1;                                \
  qc++;                                 \
 }                                      \
 putbits(data, data->pt_len[qc], data->pt_code[qc]);      \
 if(qc>1)                               \
  putbits(data, qc-1, p);                     \
}

/* Bitwise output routine */

tVOID putbits(ArjPackData* data, tINT n_c, tWORD n_x)
{
tINT p_n;
tINT bt;

	p_n=n_c;
	n_c = 16 - n_c;
	n_x <<= n_c;
	n_c = data->bitcount;
	data->bitbuf|=(n_x>>n_c);
	n_c+=p_n;
	if(n_c<8)
	{
		data->bitcount = n_c;
		return;
	}
	bt = data->out_bytes;
	if(bt >= data->out_avail)
	{
		flush_compdata(data);
		bt = data->out_bytes;
	}
	data->out_buffer[bt++] = data->bitbuf>>8;
	n_c-=8;
	if(n_c<8)
	{
		data->bitbuf<<=8;
		data->out_bytes = bt;
		data->bitcount = n_c;
		return;
	}
	if(bt >= data->out_avail)
	{
		data->out_bytes = bt;
		flush_compdata(data);
		bt = data->out_bytes;
	}
	data->out_buffer[bt++] = (tBYTE)data->bitbuf;
	n_c-=8;
	p_n-=n_c;
	data->bitbuf=n_x<<p_n;
	data->out_bytes = bt;
	data->bitcount=n_c;
}

/* Quick fill routine */

__inline tVOID fill_fpbuf(ArjPackData* data)
{
tUINT i;

	for(i=0; i<data->fpcount; i++)
		data->fpbuf[i]=-1;
}

/* Reduces the number of bits for smaller files */

__inline tVOID adjust_dicbit(ArjPackData* data)
{
	//if(uncompsize<16384)
	if(data->qwSize<16384)
		data->dicbit=12;
}

__inline tDWORD fread_crc(tPTR buffer, tDWORD count, hSEQ_IO stream)
{
tDWORD dwResult;
	CALL_SeqIO_Read(stream, &dwResult, buffer, count);
	return dwResult;
}

/* Reads data from the source file or a memory region */

__inline tINT fetch_uncomp(tCHAR *dest, tINT n, ArjPackData* data)
{
//tUINT fetch_size;

//	if(file_packing)
		return(fread_crc(dest, n, data->hInputIo));
/*	else
	{
		if(encmem_remain==0)
			return(0);
		fetch_size=min((tUINT)n, encmem_remain);
		_memmove((tCHAR  *)dest, encblock_ptr, fetch_size);
		crc32_for_block(dest, fetch_size);
		encmem_remain-=fetch_size;
		encblock_ptr+=fetch_size;
		return(fetch_size);
	}*/
}

/* Fills the length table depending on the leaf depth (call with i==root) */

static tVOID count_len(ArjPackData* data, tINT i)
{

	if(i<data->st_n)
		data->len_cnt[(data->s_depth<16) ? data->s_depth:16]++;
	else
	{
		data->s_depth++;
		count_len(data, data->left[i]);
		count_len(data, data->right[i]);
		data->s_depth--;
	}
}

/* Makes length counter table */

static tVOID make_len(ArjPackData* data, tINT root)
{
tINT i, k;
tWORD cum;

	for(i=0; i<=16; i++)
		data->len_cnt[i]=0;
	count_len(data, root);
	cum=0;
	for(i=16; i>0; i--)
	cum+=data->len_cnt[i]<<(16-i);
	while(cum!=0)
	{
//		if(debug_enabled&&strchr(debug_opt, 'f')!=NULL)
//			msg_fprintf(new_stdout, M_HDF_FIX);
		data->len_cnt[16]--;
		for(i=15; i>0; i--)
		{
			if(data->len_cnt[i]!=0)
			{
				data->len_cnt[i]--;
				data->len_cnt[i+1]+=2;
				break;
			}
		}
		cum--;
	}
	for(i=16; i>0; i--)
	{
		k=data->len_cnt[i];
		while(--k>=0)
		data->len[*data->sortptr++]=i;
	}
}

/* Sends i-th entry down the data->heap */

static tVOID downheap(ArjPackData* data, tINT i)
{
tINT j, k;

	k = data->heap[i];
	while((j=2*i) <= data->heapsize)
	{
		if( j <data->heapsize && data->freq[data->heap[j]] > data->freq[data->heap[j+1]])
			j++;
		if(data->freq[k]<=data->freq[data->heap[j]])
			break;
		data->heap[i]=data->heap[j];
		i=j;
	}
	data->heap[i]=k;
}

/* Encodes a length table element */

static tVOID make_code(ArjPackData* data, tINT n, tBYTE *len, tWORD  *code)
{
tINT i;
tWORD start[18];

	start[1]=0;
	for(i=1; i<=16; i++)
		start[i+1]=(start[i]+data->len_cnt[i])<<1;
	for(i=0; i<n; i++)
		code[i] = start[len[i]]++;
}

/* Makes data->tree, calculates data->len[], returns root */

static tINT make_tree(ArjPackData* data, tINT nparm, tWORD *freqparm, tBYTE *lenparm, tWORD *codeparm)
{
tINT i, j, k, avail;

	data->st_n=nparm;
	data->freq=freqparm;
	data->len=lenparm;
	avail=data->st_n;
	data->heapsize = 0;
	data->heap[1]=0;
	for(i=0; i<data->st_n; i++)
	{
		data->len[i]=0;
		if(data->freq[i]!=0)
		data->heap[++data->heapsize]=i;
	}
	if(data->heapsize<2)
	{
		codeparm[data->heap[1]]=0;
		return(data->heap[1]);
	}
	for(i=data->heapsize>>1; i>=1; i--)
		downheap(data, i);                          /* Make priority queue */
	data->sortptr=codeparm;
	/* While queue has at least two entries */
	do
	{
		i=data->heap[1];                     	/* Take out least-data->freq entry */
		if(i<data->st_n)
			*(data->sortptr++)=i;
		data->heap[1]=data->heap[data->heapsize--];
		downheap(data, 1);
		j=data->heap[1];                            /* Next least-data->freq entry */
		if(j<data->st_n)
			*(data->sortptr++)=j;
		k=avail++;                     	/* Generate new node */
		data->freq[k]=data->freq[i]+data->freq[j];
		data->heap[1]=k;
		downheap(data, 1);                   	/* Put into queue */
		data->left[k]=i;
		data->right[k]=j;
	} while(data->heapsize>1);
	data->sortptr=codeparm;
	make_len(data, k);
	make_code(data, nparm, lenparm, codeparm);
	return(k);                            /* Return root */
}

/* Counts the cumulative frequency */

tVOID count_t_freq(ArjPackData* data)
{
tINT i, k, n, count;

	for(i=0; i<NT; i++)
		data->t_freq[i]=0;
	n=NC;
	while(n>0&&data->c_len[n-1]==0)
		n--;
	i=0;
	while(i<n)
	{
		k=data->c_len[i++];
		if(k==0)
		{
			count=1;
			while(i<n&&data->c_len[i]==0)
			{
				i++;
				count++;
			}
			if(count<=2)
			data->t_freq[0]+=count;
			else if(count<=18)
				data->t_freq[1]++;
			else if(count==19)
			{
				data->t_freq[0]++;
				data->t_freq[1]++;
			}
			else
				data->t_freq[2]++;
		}
		else
			data->t_freq[k+2]++;
	}
}

/* Writes the encoded length */

tVOID write_pt_len(ArjPackData* data, tINT n, tINT nbit, tINT i_special)
{
tINT i, k;

	while(n>0&&data->pt_len[n-1]==0)
		n--;
	putbits(data, nbit, (tWORD)n);
	i=0;
	while(i<n)
	{
		k=(tINT)data->pt_len[i++];
		if(k<=6)
		{
			putbits(data, 3, (tWORD)k);
		}
		else
			putbits(data, k-3, 0xFFFE);
		if(i==i_special)
		{
			while(i<6&&data->pt_len[i]==0)
				i++;
			putbits(data, 2, (tWORD)(i-3));
		}
	}
}

/* Writes character length */

tVOID write_c_len(ArjPackData* data)
{
tINT i, k, n, count;

	n=NC;
	while(n>0&&data->c_len[n-1]==0)
		n--;
	putbits(data, CBIT, (tWORD)n);
	i=0;
	while(i<n)
	{
		k=(tINT)data->c_len[i++];
		if(k==0)
		{
			count=1;
			while(i<n&&data->c_len[i]==0)
			{
				i++;
				count++;
			}
			if(count<=2)
			{
				for(k=0; k<count; k++)
					 putbits(data, data->pt_len[0], data->pt_code[0]);
			}
			else if(count<=18)
			{
				putbits(data, data->pt_len[1], data->pt_code[1]);
				putbits(data, 4, (tWORD)(count-3));
			}
			else if(count==19)
			{
				putbits(data, data->pt_len[0], data->pt_code[0]);
				putbits(data, data->pt_len[1], data->pt_code[1]);
				putbits(data, 4, 15);
			}
			else
			{
				putbits(data, data->pt_len[2], data->pt_code[2]);
				putbits(data, CBIT, (tWORD)(count-20));
			}
		}
		else
			putbits(data, data->pt_len[k+2], data->pt_code[k+2]);
	}
}

/* Encodes a block and writes it to the output file */

tVOID send_block(ArjPackData* data)
{
tUINT i, k, flags=0, root, pos, size;
tUINT c;

	//if(!unpackable)
	{
		root = make_tree(data, NC, data->c_freq, data->c_len, data->c_code);
		size = data->c_freq[root];
		putbits(data, 16, (tWORD)size);
		if(root>=NC)
		{
			count_t_freq(data);
			root=make_tree(data, NT, data->t_freq, data->pt_len, (tWORD  *)data->pt_code);
			if(root>=NT)
				write_pt_len(data, NT, TBIT, 3);
			else
			{
				putbits(data, TBIT, 0);
				putbits(data, TBIT, (tWORD)root);
			}
			write_c_len(data);
		}
		else
		{
			putbits(data, TBIT, 0);
			putbits(data, TBIT, 0);
			putbits(data, CBIT, 0);
			putbits(data, CBIT, (tWORD)root);
		}
		root=make_tree(data, NP, data->p_freq, data->pt_len, (tWORD  *)data->pt_code);
		if(root>=NP)
			write_pt_len(data, NP, PBIT, -1);
		else
		{
			putbits(data, PBIT, 0);
			putbits(data, PBIT, (tWORD)root);
		}
		pos=0;
		for(i=0; i<size; i++)
		{
//			if(unpackable)
//				return;
			if(i % CHAR_BIT == 0)
				flags = data->buf[pos++];
			else
				flags<<=1;
			if(flags&(1U<<(CHAR_BIT-1)))
			{
				c=(tUINT)data->buf[pos++]+(1<<CHAR_BIT);
				encode_c(c);
				k=data->buf[pos++];
				k|=(tUINT)data->buf[pos++]<<CHAR_BIT;
				encode_p((tWORD)k);
			}
			else
			{
				c=data->buf[pos++];
				encode_c(c);
			}
		}
		for(i=0; i<NC; i++)
			data->c_freq[i]=0;
		for(i=0; i<NP; i++)
			data->p_freq[i]=0;
	}
}

/* Handy macro for retrieval of two bytes (don't care about the portability) */

#define word_ptr(c) (*(tWORD *)(c))

/* Tree update routine. Possibly the most time-consuming one. */

static tINT upd_tree(ArjPackData* data, tINT n_c)
{
short *dptr;
short r_bx, r_dx;
tWORD r_ax;
short r_bp=1;
tBYTE *tptr;
tBYTE *tdptr;
tBYTE *prev_tptr, *prev_tdptr;
tINT c;
tCHAR diff;
tINT remainder;

	dptr=data->dtree;
	if((r_bx=dptr[n_c])>=0)
	{
		tptr=data->tree+n_c;
		tdptr=data->tree+r_bx;
		r_ax=word_ptr(tptr);
		r_dx=data->numchars;
		if(--r_dx>=0)
			goto ut_loop;
ut_fetch:
		r_ax=word_ptr(tptr+r_bp-1);
select_pos:
	tdptr=data->tree+r_bp;
	do
	{
		if(--r_dx<0)
			goto upd_finish;
		r_bx=dptr[r_bx];
		if(r_bx<0)
			goto upd_finish;
	} while(r_ax!=word_ptr(tdptr+r_bx-1));
	tdptr+=r_bx-r_bp;
ut_loop:
	if(word_ptr(tptr)!=word_ptr(tdptr))
		goto select_pos;
	prev_tptr=tptr;
	prev_tdptr=tdptr;
	tptr+=2;
	tdptr+=2;
	for(c=128; c>0; c--)
	{
		if(word_ptr(tptr)!=word_ptr(tdptr))
			break;
		tptr+=2;
		tdptr+=2;
	}
	diff=*(tdptr)-*(tptr);
	remainder=_toi32(tdptr-prev_tdptr);
	if(diff==0)
		remainder+=1;
	tptr=prev_tptr;
	tdptr=prev_tdptr;
	if(remainder<=r_bp)
		goto ut_fetch;
	if(tptr-tdptr<=data->dicsiz_cur)
	{
		data->dicpos=_toui16(tptr-tdptr-1);
		r_bp=remainder;
		if(r_bp<256)
			goto ut_fetch;
	}
upd_finish:
	if(r_bp>256)
		r_bp=256;
	}
	return(data->tc_passes=r_bp);
}

/* Optimized output routine */

static tVOID output(ArjPackData* data, tINT c, tWORD p)
{
tBYTE *bptr;
tBYTE cy;
tWORD r_dx;
tWORD r_bx;

	bptr=data->buf;
	r_dx=data->cpos;
	cy=data->output_mask&1;
	data->output_mask=(cy?0x80:0)|((data->output_mask&0xFF)>>1);
	if(cy)
	{
		if(r_dx>=data->bufsiz)
		{
			send_block(data);
/*			if(unpackable)
			{
				data->cpos=bptr-data->buf;
				return;
			}*/
			r_dx=0;
		}
		data->output_pos=r_dx;
		data->buf[r_dx]=0;
		r_dx++;
	}
	bptr+=r_dx;
	*bptr++=c;
	data->c_freq[c]++;
	if(c>=256)
	{
		data->buf[data->output_pos]|=data->output_mask;
		*bptr++=p&0xFF;
		*bptr++=(p>>8);
		for(r_bx=0; p!=0; p>>=1)
			r_bx++;
		data->p_freq[r_bx]++;
	}
	data->cpos=_toui16(bptr-data->buf);
}

/* Unstubbed optimized output routine */

static tVOID output_opt(ArjPackData* data, tBYTE c)
{
tBYTE *bptr, *cptr;
tWORD r_dx;
tBYTE cy;

	cptr=bptr=data->buf;
	r_dx=data->cpos;
	cy=data->output_mask&1;
	data->output_mask=(cy?0x80:0)|((data->output_mask&0xFF)>>1);
	if(cy)
	{
		if(r_dx>=data->bufsiz)
		{
			send_block(data);
			r_dx=0;
/*			if(unpackable)
			{
				data->cpos=r_dx;
				return;
			}*/
		}
		data->output_pos=r_dx;
		cptr[r_dx]=0;
		r_dx++;
	}
	bptr+=r_dx;
	*bptr++=c;
	data->c_freq[c]++;
	data->cpos=_toui16(bptr-cptr);
}

/* Initializes memory for encoding */

tVOID allocate_memory(ArjPackData* data)
{
tINT i;

	if((data->c_freq=ArjAlloc(data, (NC*2-1)*sizeof(*data->c_freq)))==NULL)
		error(M_OUT_OF_NEAR_MEMORY);
	if((data->c_code=ArjAlloc(data, NC * sizeof(*data->c_code)))==NULL)
		error(M_OUT_OF_MEMORY);
	if((data->heap=ArjAlloc(data, (NC+1) * sizeof(*data->heap)))==NULL)
		error(M_OUT_OF_MEMORY);
	for(i=0; i<NP; i++)
		data->p_freq[i]=0;
	data->depth=0;
	data->bufsiz = data->current_bufsiz;
	if(data->bufsiz >= MAX_BUFSIZ-BUFSIZ_INCREMENT)
		data->bufsiz=MAX_BUFSIZ-1;

#ifdef FINETUNE_BUFSIZ
	i=1;
#endif
	/* Adjust the buffer size if there's not enough memory for it */
	while((data->buf=ArjAlloc(data, data->bufsiz))==NULL)
	{
#ifndef FINETUNE_BUFSIZ
		data->bufsiz=data->bufsiz/10U*9U;
#else
		if(i<2048)
			data->bufsiz-=i++;
		else
			data->bufsiz=data->bufsiz/16U*15U;
#endif
		if(data->bufsiz<MIN_BUFSIZ)
			error(M_OUT_OF_MEMORY);
	}
//	if(debug_enabled&&strchr(debug_opt, 'v')!=NULL)
//		msg_printf(M_BUFSIZ, data->bufsiz);
	init_putbits(data);
	data->output_mask=1;
	data->output_pos=0;
	data->cpos=0;
	data->buf[data->output_pos]='\0';
	data->bufsiz-=30;
}

/* Shutdown the encoding */

tVOID huf_encode_end(ArjPackData* data)
{
//	if(!unpackable)
		send_block(data);
	shutdown_putbits(data);
	ArjFree(data, data->c_freq);
	ArjFree(data, data->c_code);
	ArjFree(data, data->heap);
	ArjFree(data, data->buf);
	data->bufsiz=0;
	data->cpos=0;
}

/* Basic encoding routine */

static tVOID huf_encode(ArjPackData*data)
{
tINT hash_bits;
tWORD fp_max;
tINT nchars;
tINT i, j, m=0;
short k, l;
tWORD t;                      /* Exchange variable */
tINT tree_el;
tUINT n_passes;
tUINT f_dicpos;
tUINT fetch;
tUINT max_fetch;                /* For comparision */
short *fptr;
short *dtptr;
tBYTE *tptr;
tWORD r_cx, r_dx, r_ax;
tINT pm;

	hash_bits=(data->dicbit+2)/3;
	data->fpcount=1U<<data->dicbit;
	fp_max=data->fpcount-1;
	if(data->tree == NULL)
	{
		data->tree = ArjAlloc(data, (data->treesize+2) * sizeof(*data->tree));
		data->ftree = data->dtree = ArjAlloc(data, ((tDWORD)data->treesize+16L)*sizeof(*data->ftree));
		data->fpbuf = ArjAlloc(data, ((tDWORD)data->fpcount+4L)*2L);

		if(data->ftree==NULL || data->fpbuf == NULL || data->tree == NULL)
			error(M_OUT_OF_MEMORY);
	}
	if(data->dic_alloc<1024)
		data->dic_alloc=1024;
	allocate_memory(data);
	nchars=(UCHAR_MAX+THRESHOLD)*2;
	data->tc_passes=0;
	data->dicpos=0;
	i=j=0;
	while(1)//(!unpackable)
	{
		tree_el=0;
		k=0;
		if(j!=0)
		{
			tree_el=data->dic_alloc;
			if((k=j-tree_el)<=0)
			{
				k=0;
				tree_el=j;
			}
			else
				_memmove(data->tree, data->tree+k, tree_el);
		}
		max_fetch=fetch=(tUINT)(data->treesize-tree_el);
//		if(multivolume_option)
//			fetch=check_multivolume(fetch);
		if(max_fetch!=fetch)
			nchars=4;
		if((fetch=fetch_uncomp(data->tree+tree_el, fetch, data))==0)
		{
			if(tree_el==0||k==0)
				break;
			_memmove(data->tree+k, data->tree, tree_el);
			data->dicsiz_cur=min(tree_el-i-1, data->dicsiz_cur);
			break;
		}
		j=fetch+tree_el;
		m=0;
		if(k<=0)
			fill_fpbuf(data);
		else
		{
			fptr=data->fpbuf;
			for(l=data->fpcount>>2; l>0; l--)
			{
				*fptr=max(*fptr-k, -1);
				fptr++;
				*fptr=max(*fptr-k, -1);
				fptr++;
				*fptr=max(*fptr-k, -1);
				fptr++;
				*fptr=max(*fptr-k, -1);
				fptr++;
			}
			dtptr=data->dtree;
			for(l=tree_el>>3; l>0; l--)
			{
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
			}
		/* Store the remainder */
			for(l=tree_el%8; l>0; l--)
			{
				*dtptr=max(dtptr[k]-k, -1);
				dtptr++;
			}
			m+=tree_el;
			if(m>=2)
				m-=2;
		}
		tptr=&data->tree[m];
		r_dx=(tWORD)*(tptr++);
		r_cx=(fp_max&0xFF00)|(hash_bits&0xFF);
		r_dx<<=(hash_bits&0xFF);
		r_dx^=(tWORD)*(tptr++);
		r_dx&=(r_cx|0xFF);
		for(l=j-2; m<l; m++)
		{
			r_dx<<=(r_cx&0xFF);
			r_dx^=*(tptr++);
			r_dx&=(r_cx|0xFF);
			t=data->fpbuf[r_dx];
			data->fpbuf[r_dx]=m;
			data->dtree[m]=t;
		}
		data->dtree[m]=-1;
		data->dtree[m+1]=-1;
		m=tree_el-i;
		i+=fetch;
		while(i>nchars)
		{
			i--;
			pm=m;
			m++;
			n_passes=(tUINT)data->tc_passes;
			f_dicpos=(tINT)data->dicpos;
			if((r_ax=upd_tree(data, m))>i)
				r_ax=data->tc_passes=i;
			if((n_passes < THRESHOLD) || 
               ((n_passes == THRESHOLD) && 
                (f_dicpos > 16384)) || 
               (--r_ax > n_passes) ||
               ((r_ax == n_passes) && 
                ((data->dicpos >> 1) < (tWORD)f_dicpos)))
			{
				output_opt(data, data->tree[pm]);
			}
			else
			{
				i-=n_passes-1;
				m+=n_passes-1;
				r_ax=n_passes+UCHAR_MAX-2;
				output(data, (tINT)r_ax, (tWORD)f_dicpos);
				upd_tree(data, m);
				if(data->tc_passes>i)
					data->tc_passes=i;
			}
		}
	}
	while(i>0)
	{
		i--;
		pm=m;
		m++;
		n_passes=data->tc_passes;
		f_dicpos=data->dicpos;
		if((r_ax=upd_tree(data, m))>i)
			r_ax=data->tc_passes=i;
		if((n_passes<THRESHOLD) ||
           ((n_passes==THRESHOLD) && 
            (f_dicpos>16384)) ||
           (--r_ax>n_passes) ||
           ((r_ax==n_passes) &&
            ((data->dicpos>>1) < (tWORD)f_dicpos)))
		{
			output_opt(data, data->tree[pm]);
		}
		else
		{
			i-=n_passes-1;
			m+=n_passes-1;
			r_ax=n_passes+UCHAR_MAX-2;
			output(data, r_ax, (tWORD)f_dicpos);
			if((r_ax=upd_tree(data, m))>i)
				data->tc_passes=i;
		}
	}
	huf_encode_end(data);
	ArjFree(data, data->ftree);
	ArjFree(data, data->fpbuf);
	ArjFree(data, data->tree);
	data->tree=NULL;
}

/* Encoding stub for -m3 */

static tVOID huf_encode_m3(ArjPackData* data)
{
tINT hash_bits;
tWORD fp_max;
tINT i, j, m;
tWORD t;                      /* Exchange variable */
short k, l;
tINT tree_el;
tUINT fetch;
tBYTE *tptr;
tWORD r_cx, r_dx;
short r_ax;

	hash_bits=(data->dicbit+2)/3;
	data->fpcount=1U<<data->dicbit;
	fp_max=data->fpcount-1;
	if(data->tree==NULL)
	{
		if((data->tree=ArjAlloc(data, (data->treesize+2)*sizeof(*data->tree)))==NULL)
			error(M_OUT_OF_NEAR_MEMORY);
		data->ftree=data->dtree=ArjAlloc(data, ((tDWORD)data->treesize+16L)*sizeof(*data->ftree));
		data->fpbuf=ArjAlloc(data, ((tDWORD)data->fpcount+4L)*2L);
		if(data->ftree==NULL||data->fpbuf==NULL)
			error(M_OUT_OF_MEMORY);
	}
	allocate_memory(data);
	j=0;
	while(1)//(!unpackable)
	{
		tree_el=0;
		if(data->dic_alloc!=0&&j!=0)
		{
			tree_el=data->dic_alloc;
			if((k=j-tree_el)<=0)
			{
				k=0;
				tree_el=j;
			}
			else
				_memmove(data->tree, data->tree+k, tree_el);
		}
		fetch=(tUINT)(data->treesize-tree_el);
//		if(multivolume_option)
//			fetch=check_multivolume(fetch);
		if((fetch=fetch_uncomp(data->tree+tree_el, fetch, data))==0)
			break;
		else
		{
			j=fetch+tree_el;
			fill_fpbuf(data);
			l=fetch;
			tptr=data->tree;
			r_dx=(tWORD)*(tptr++);
			r_cx=(fp_max&0xFF00)|(hash_bits&0xFF);
			r_dx<<=(hash_bits&0xFF);
			r_dx^=(tWORD)*(tptr++);
			r_dx&=(r_cx|0xFF);
			for(m=0; m<j; m++)
			{
				r_dx<<=(r_cx&0xFF);
				r_dx^=*(tptr++);
				r_dx&=(r_cx|0xFF);
				t=data->fpbuf[r_dx];
				data->fpbuf[r_dx]=m;
				data->dtree[m]=t;
			}
			i=l;
			m=tree_el;
			while(i>0)
			{
				r_ax=upd_tree(data, m);
				if((r_ax=upd_tree(data, m))>i)
					r_ax=data->tc_passes=i;
				if(r_ax<THRESHOLD)
				{
					r_ax=(tWORD)data->tree[m];
					output(data, r_ax, 0);
					m++;
					i--;
				}
				else
				{
					r_ax+=UCHAR_MAX-2;
					output(data, r_ax, data->dicpos);
					m+=data->tc_passes;
					i-=data->tc_passes;
				}
			}
		}
	}
	huf_encode_end(data);
	ArjFree(data, data->ftree);
	ArjFree(data, data->fpbuf);
	ArjFree(data, data->tree);
	data->tree=NULL;
}

/* Encodes a single file. */

tVOID encode(ArjPackData* data, tINT method, tINT max_compression)
{
//tCHAR *dsw;                             /* Debug switch (-hd) pointer */
//tCHAR a;

	data->numchars = UCHAR_MAX+5;
	data->dicbit = 14;
	data->dic_alloc = DICSIZ;
	data->treesize = 31744;
	data->dicsiz_cur = DICSIZ-2;
	adjust_dicbit(data);
		/* Method 0 (store) is already filtered away at this point */
	switch(method)
	{
		case DEFLATED1:
			data->numchars = UCHAR_MAX+5;
			break;
		case DEFLATED2:
			data->treesize = 30720;
			data->numchars = 72;
			data->dic_alloc = 20480;
			break;
		case DEFLATED3:
			data->treesize = 30720;
			data->numchars = 32;
			data->dic_alloc = 8192;
			break;
		default:
			error(M_UNKNOWN_METHOD);
	}
	switch(max_compression)
	{
		case 1:
			data->numchars = 3000;
			data->dic_alloc = DICSIZ+512;
			break;
		case 2:
			data->numchars = 512;
			data->dic_alloc = DICSIZ+512;
			break;
		case 3:
			data->numchars = 1024;
			data->dicbit = 12;
			data->treesize = 20480;
			data->dicsiz_cur = data->dic_alloc=16384;
			break;
		case 4:
			data->numchars = 1024;
			data->dicbit = 12;
			data->treesize = 12288;
			data->dicsiz_cur = data->dic_alloc=8192;
	}
	if(data->dicsiz_cur > DICSIZ_MAX)
		error(M_LARGE_DICTIONARY);
	if(data->dic_alloc > data->treesize)
		error(M_LARGE_GSIZE);
	if(method == 3)
		huf_encode_m3(data);
	else
		huf_encode(data);
}

/* Fast search stub for method 4 */

static tVOID enc4_pass1(ArjPackData* data, tINT n_c)
{
short r_bx=1;
tINT r_cx=0;

	while(n_c>=r_bx)
	{
		n_c-=r_bx;
		r_cx++;
		r_bx<<=1;
	}
	if(r_cx!=0)
		putbits(data, r_cx, -1);
	if(r_cx<7)
		r_cx++;
	putbits(data, r_cx, (tWORD)n_c);
}

/* Dictionary position lookup */

static tVOID enc4_pass2(ArjPackData* data, tINT n_c)
{
tWORD r_bx=1<<9;
tINT r_cx=9;

	while(n_c>=r_bx)
	{
		n_c-=r_bx;
		r_cx++;
		r_bx<<=1;
	}
	if(r_cx!=9)
		putbits(data, r_cx-9, -1);
	if(r_cx<13)
		r_cx++;
	putbits(data, r_cx, (tWORD)n_c);
}

/* Encodes a single file, using method 4 */

tVOID encode_f(ArjPackData* data)
{
tINT fetch;
tINT hash_bits;
tWORD fp_max;
tBYTE *tptr;
tINT i, m;
tWORD r_cx, r_dx, r_ax;
tWORD t;

	data->dicbit=14;
	data->numchars=32;
	data->dicsiz_cur=15800;
	data->treesize=30720;
	adjust_dicbit(data);
	hash_bits=(data->dicbit+2)/3;
	data->fpcount=1U<<data->dicbit;
	fp_max=data->fpcount-1;
	if(data->tree==NULL)
	{
		if((data->tree = ArjAlloc(data, (data->treesize+2)*sizeof(*data->tree)))==NULL)
			error(M_OUT_OF_NEAR_MEMORY);
		data->ftree=data->dtree=ArjAlloc(data, ((tDWORD)data->treesize+16L)* sizeof(*data->ftree));
		data->fpbuf=ArjAlloc(data, ((tDWORD)data->fpcount+4L)*2L);
		if(data->ftree==NULL||data->fpbuf==NULL)
			error(M_OUT_OF_MEMORY);
	}
	init_putbits(data);
	data->cpos=0;
	while(1)//(!unpackable)
	{
		fetch=data->treesize;
//		if(multivolume_option)
//			fetch=check_multivolume(fetch);
		if((fetch=fetch_uncomp(data->tree, fetch, data))==0)
			break;
		fill_fpbuf(data);
		m=0;
		tptr=data->tree;
		r_dx=(tWORD)*(tptr++);
		r_cx=(fp_max&0xFF00)|(hash_bits&0xFF);
		r_dx<<=(hash_bits&0xFF);
		r_dx^=(tWORD)*(tptr++);
		r_dx&=(r_cx|0xFF);
		for(m=0; m<fetch; m++)
		{
			r_dx<<=(r_cx&0xFF);
			r_dx^=(tBYTE)*(tptr++);
			r_dx&=(r_cx|0xFF);
			t=data->fpbuf[r_dx];
			data->fpbuf[r_dx]=m;
			data->dtree[m]=t;
		}
		m=0;
		i=fetch;
		while(i>0)
		{
			if((r_ax=upd_tree(data, m))>i)
				r_ax=data->tc_passes=i;
			if(r_ax<THRESHOLD)
			{
				putbits(data, 9, (tBYTE)data->tree[m]);
				i--;
				m++;
			}
			else
			{
				i-=data->tc_passes;
				m+=data->tc_passes;
				r_ax=data->tc_passes-2;
				enc4_pass1(data, r_ax);
				enc4_pass2(data, data->dicpos);
			}
		}
	}
	shutdown_putbits(data);
	ArjFree(data, data->fpbuf);
	ArjFree(data, data->ftree);
	ArjFree(data, data->tree);
	data->tree = NULL;
}


tVOID flush_compdata(ArjPackData* data)
{
	if(data->out_bytes > 0)
	{
		data->dwCompressed += (tDWORD)data->out_bytes;
//		if(data->dwCompressed > data->origsize /*&& (!garble_enabled||!file_packing)*/)
//			unpackable = 1;
//		else
		{
//			if(!no_file_activity)
			{
//				if(garble_enabled)
//					garble_encode_stub(data->out_buffer, data->out_bytes);
//				if(file_packing)
				{
					file_write(data->out_buffer, data->out_bytes, data->hOutputIo);
				}
//				else
//				{
//					_memmove(packblock_ptr, (tCHAR *)data->out_buffer, data->out_bytes);
//					packblock_ptr += data->out_bytes;
//				}
				data->out_avail = PUTBIT_SIZE;
			}
		}
		data->out_bytes=0;
	}
}

/*

tUINT check_multivolume(tUINT increment)
{
	return 0;
tDWORD pvol;
tUINT arjsec_overhead;
tLONG remain;
tUINT  inc;
	
	if(!file_packing)
		return(increment);
	if(increment==0||volume_flag_set)
	{
		volume_flag_set=1;
		return(0);
	}
	pvol=0L;
	if(protfile_option)
		pvol=calc_protdata_size(volume_limit, protfile_option);
	arjsec_overhead=sign_with_arjsec?ARJSEC_SIG_MAXLEN+1:0;
	// Split this expression to work around High C bugs -- ASR 14/08/2001
	remain=volume_limit-_ftell(hOutputIo)-pvol-(tLONG)arjsec_overhead;
	stop_optimizer();
	remain-=(tLONG)data->out_bytes+(tLONG)data->cpos+(tLONG)ext_voldata;
	stop_optimizer();
	remain-=(tLONG)MULTIVOLUME_RESERVE+(tLONG)t_volume_offset;
	// Now decrement the buffer size until we fit the remainder 
	while((tLONG)data->bufsiz*2L>remain&&data->bufsiz>MIN_CRITICAL_BUFSIZ)
		data->bufsiz>>=1;
	if(data->bufsiz<MIN_CRITICAL_BUFSIZ)
		data->bufsiz=MIN_CRITICAL_BUFSIZ;
	if((tLONG)increment+1000L<remain&&(tLONG)increment*2L<remain)
		return(increment);
	inc=0;
	if((tLONG)increment<remain)
		inc=increment;
	else if(remain>0)
		inc=remain;
	if(inc<=0)
	{
		volume_flag_set=1;
		return(0);
	}
	if((tLONG)increment*2L>remain)
	{
		if(inc>1000)
			return((inc-inc%500)>>1);
		else if(inc>2000)
			return(inc-1000);
		else
			return((inc>512)?inc>>1:inc);
	}
}
*/

tVOID init_putbits(ArjPackData* data )
{
tQWORD fp;
	
	data->bitcount=0;
//	byte_buf=0;
	data->bitbuf=0;
	data->out_bytes=0;
	fp = 0;
//	if(!file_packing||no_file_activity)
//		fp=0L;
//	else
//	{
		///fp = _ftell(data->hOutputIo);

		//fp = _ftell(data->hInputIo);

//		if(fp > MAX_FILE_SIZE)
//			error(M_FILE_IS_TOO_LARGE);
//	}
	data->out_buffer = ArjAlloc(data, PUTBIT_SIZE );
	data->out_avail = PUTBIT_SIZE-((tDWORD)fp % PUTBIT_SIZE);
//	if(data->out_avail > PUTBIT_SIZE)
//		error(M_PUTBIT_SIZE_ERROR);
}


tVOID shutdown_putbits(ArjPackData* data)
{
	//if(!unpackable)
	{
		putbits(data, CHAR_BIT-1, 0);
		if(data->out_bytes!=0)
			flush_compdata(data);
	}
	ArjFree(data, data->out_buffer);
	data->out_bytes=0;
}

tDWORD file_write(tBYTE* out_buffer, tDWORD out_bytes, hSEQ_IO hIo)
{
tDWORD dwResult;
	CALL_SeqIO_Write(hIo, &dwResult, out_buffer, out_bytes);
	return dwResult;
}
__inline tQWORD _ftell(hSEQ_IO hIo)
{
tQWORD qwRet;
	CALL_SeqIO_GetSize(hIo, &qwRet, IO_SIZE_TYPE_EXPLICIT);
	return qwRet;
}

tPTR ArjAlloc(ArjPackData* data, tINT dwBytes)
{
tBYTE * lpData;
	
	CALL_Heap_Alloc(data->hHeap, (tPTR*)&lpData, dwBytes+16);
	if(lpData == NULL)
	{
		data->error = errNOT_ENOUGH_MEMORY;
	}
	return lpData;
}

tVOID ArjFree(ArjPackData* data, tPTR lpdata)
{
	if(lpdata == NULL)
	{
		return ;
	}
	CALL_Heap_Free(data->hHeap, lpdata);
}

tPTR __cdecl _memmove (tPTR dst, tPTR src, tUINT count)
{
void * ret = dst;

	if (dst <= src || (tCHAR *)dst >= ((tCHAR *)src + count)) 
	{
	//
	// Non-Overlapping Buffers
	// copy from lower addresses to higher addresses
	//
		memcpy(dst, src,count );
//		while (count--) 
//		{
//			*(tCHAR *)dst = *(tCHAR *)src;
//			dst = (tCHAR *)dst + 1;
//			src = (tCHAR *)src + 1;
//		}
	}
	else 
	{
	//
	// Overlapping Buffers
	// copy from higher addresses to lower addresses
	//
		dst = (tCHAR *)dst + count - 1;
		src = (tCHAR *)src + count - 1;

		while (count--) 
		{
			*(tCHAR *)dst = *(tCHAR *)src;
			dst = (tCHAR *)dst - 1;
			src = (tCHAR *)src - 1;
		}
	}
	return(ret);
}

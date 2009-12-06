#ifndef __STD_C_HASH_H__
#define __STD_C_HASH_H__

struct STD_C_HASH_ITER
{
	void				      *key;
	void				     *data;
	STD_C_HASH			     *hash;
	unsigned int	          hkey;
	struct STD_C_HASH_ITER   *next;
	struct STD_C_HASH_ITER   *prev;
	int					   foreach;
};

extern STD_C_HASH Std_C_Hash_New();

extern void		  Std_C_Hash_Del();

extern STD_C_HASH Std_C_Hash_Get();

#endif
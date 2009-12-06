#ifndef __STD_C_LIST_H__
#define __STD_C_LIST_H__

typedef struct STD_LIST STD_C_LIST;
typedef size_t SIZE_T;

struct STD_C_LIST_ITER 
{
    void*			            data;
    struct	STD_C_LIST*	 	    list;
    struct	STD_C_LIST_ITER*  __next;
    struct	STD_C_LIST_ITER*  __prev;
    int					   __foreach;
};

typedef unsigned int (*sd_list_func_t)(void* a_data, void* a_userdata);

extern STD_C_LIST* Std_C_List_New(SIZE_T capacity);

extern void        Std_C_List_Del(STD_C_LIST* this);


#endif
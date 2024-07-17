//Based on Listing 1 from  
/*
Y. Shoshitaishvili et al., 
"SOK: (State of) The Art of War: Offensive Techniques in Binary Analysis," 
2016 IEEE Symposium on Security and Privacy (SP), 
2016, pp. 138-157, 
doi: 10.1109/SP.2016.17.
*/

#include <string.h>

extern void symex_error(void);
extern void make_symbolic(void *, size_t);

#define MY_ASSERT(COND) \
	((COND) ? (void)0 : symex_error())

#define DATA_SIZE 0x100
#define BUF_SIZE 32


// This must be aligned on a page boundary in order to avoid extra forks
// for overlapping accesses.
char g_data[DATA_SIZE] __attribute__((aligned(DATA_SIZE)));/* = {0,'Z',0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0};*/

int recursive_loop(int i, int count, int magic){
    int is_equal = g_data[i]=='Z';
    int tmp_magic = magic + 1; 
    if(i<=0){
        return is_equal + count;
    }else{
        return is_equal + recursive_loop(i-1, count, tmp_magic);
    }
}

void *
memcpy (void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  MY_ASSERT(len <= BUF_SIZE);
  while (len--)
    *d++ = *s++;
  return dest;
}

int main() {
    char buf[BUF_SIZE];// = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int path = 0;
    int magic;   
    make_symbolic(&magic, sizeof(magic));
    //make_symbolic(&buf, sizeof(buf));
    //make_symbolic(&g_data, sizeof(g_data));
    

    //section1:
    //difficult check for fuzzing
    if(magic == 0x31337987){
        path = 1;
        memcpy(buf,g_data,31);
    }

    //section2:
    if(magic < 100 && magic % 15 == 2 && magic % 11 ==6){
        //only solution is 17
        path |=2;
        memcpy(buf,g_data,magic);
    }

    //section3:
    if(magic>0){
        path |=4;
        if(magic<0){
            path |=8;
            recursive_loop(6,0, magic);
        }
    }

    //most of the executiontime is spent here
    int count = recursive_loop(6,0, magic);

    //section4:
    if(count >=8 && count<=16){
        path |=16;
        memcpy(buf,g_data,count*20);
    }

    return path;
}






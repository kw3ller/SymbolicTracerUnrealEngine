// Based on Listing 1 from
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
char g_data[DATA_SIZE] __attribute__((aligned(DATA_SIZE))); /* = {0,'Z',0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,0};*/

int recursive_loop(int i, int count, int magic)
{
    int is_equal = g_data[i] == 'Z';
    int tmp_magic = magic + 1;
    if (i <= 0)
    {
        return is_equal + count;
    }
    else
    {
        return is_equal + recursive_loop(i - 1, count, tmp_magic);
    }
}

void *
memcpy(void *dest, const void *src, size_t len)
{
    char *d = dest;
    const char *s = src;
    MY_ASSERT(len <= BUF_SIZE);
    while (len--)
        *d++ = *s++;
    return dest;
}

int main()
{
    char buf[BUF_SIZE]; // = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int magic;
    int number;
    make_symbolic(&magic, sizeof(magic));
    make_symbolic(&number, sizeof(number));


    // difficult check for fuzzing
    if (magic == 0x31337987)
    {
        if (number > 10)
        {
            memcpy(buf, g_data, 31);
        }
        else if (number % 23 == 2)
        {
            recursive_loop(3, 0, 4);
        }
        else
        {
            memcpy(buf, g_data, 31);
        }
    }

  
    if (magic < 100)
    {

        if (magic % 15 == 2)
        {
            memcpy(buf, g_data, 15);

            if (magic % 11 == 6)
            {
                // only solution is 17
                memcpy(buf, g_data, magic);
            }

            if (magic == 32)
            {
                memcpy(buf, g_data, 21);
            }
        }

        memcpy(buf, g_data, 17);
    }

   
    if (magic > 0)
    {
        if (magic > 100)
        {
            recursive_loop(3, 0, 4);
        }

        if (magic < 100)
        {
            recursive_loop(3, 0, 2);
        }

        if (magic < 0)
        {
            recursive_loop(6, 0, magic);
        }
    }

    
    int count = recursive_loop(6, 0, magic);

    switch (number)
    {
    case 23:
    {
        if (count >= 8)
        {

            if (count <= 16)
            {
                return 3;
            }
            else
            {
                return 6;
            }
        }
        else if (count == 7)
        {
            return 12;
        }
        break;
    }
    default:
        return 78;
    }

    
    if (count > 0)
    {

        if (count <= 16)
        {
            memcpy(buf, g_data, count * 20);
        }
        else
        {
            memcpy(buf, g_data, 5 * 20);
        }

    }
    else if (count == 7)
    {
        return 12;
    }
    else if (count < 6)
    {
        return 26;
    }

    return number;
}



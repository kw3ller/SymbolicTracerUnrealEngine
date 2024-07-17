#include <string.h>

extern void symex_error(void);
extern void make_symbolic(void *, size_t);

int main()
{
    int x;

    make_symbolic(&x, sizeof(x));

    int p = 0;
    int q = 0;
    int r = 0;

    if (x > 10)
    {
        for (int i = 0; i < 20; i++)
        {
            p += 1;
        }
    }

    if (x < 10)
    {
        if (x == 0)
        {
            q = 1;

            if (x != 0)
            {
                q = 2;
            }
        }
        r = 2;
    }

    return 1;
}

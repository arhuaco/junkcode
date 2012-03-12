/*
 * sample smoothing proof-of-concept.
 * By Dale Schumacher
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define    W            5      /* smoothing window */

struct smooth {
    int        data[W];        /* data samples */
    int        sort[W];        /* sorted data */
    int        pos;            /* circular data buffer position */
    int        avg;            /* 2x moving average */
};

void
init_smooth(struct smooth* p)
{
    int i;

    for (i = 0; i < W; ++i) {
        p->data[i] = 0;
        p->sort[i] = 0;
    }
    p->pos = 0;
    p->avg = 0;
}

#define    get_smooth(p)        (((p)->avg) >> 1)
#define    get_median(p)        ((p)->sort[W/2])

void
put_smooth(struct smooth* p, int value)
{
    p->data[p->pos] = value;
    if (p->pos <= 0) {
        p->pos = W;
    }
    --p->pos;
}

void
sort_smooth(struct smooth* p)
{
    int i;
    int j;
    int k;
    int n;

    i = p->pos;
    for (k = 0; k < W; ++k) {
        /* get sample from ring buffer */
        if (i <= 0) {
            i = W;
        }
        --i;
        n = p->data[i];
        /* insert into sorted list */
        for (j = 0; j < k; ++j) {
            if (p->sort[j] >= n) {
                int m = k;
                while (j < m) {
                    p->sort[m] = p->sort[m - 1];
                    m -= 1;
                }
                break;
            }
        }
        p->sort[j] = n;
    }
    /* done sorting, pick median & add to average */
    p->avg = get_median(p) + get_smooth(p);
}

void
dump_smooth(struct smooth* p)
{
    int i;

    printf("data=[");
    i = p->pos;
    while(1) {
        ++i;
        if (i >= W) {
            i = 0;
        }
        printf("%d", p->data[i]);
        if (i == p->pos) {
            printf("] ");
            break;
        }
        printf(",");
    }
    printf("sort=[");
    for (i = 0; i < W; ++i) {
        printf("%s%d", (i ? "," : ""), p->sort[i]);
    }
    printf("] ");
    printf("median=%d smooth=%d ", get_median(p), get_smooth(p));
    printf("avg(x2)=%d\n", p->avg);
}

/**
SAMPLE DATA

1213744740.484058:    511    589      1
1213744740.489078:    513    588      1
1213744740.494059:    511    588      1
1213744740.499058:    509    588      1
1213744740.504078:    485    591      1  <===
1213744740.509058:    509    591      1
1213744740.514079:    506    586      1
1213744740.519078:    511    589      1
1213744740.524078:    507    587      1
1213744740.529079:    511    586      1
1213744740.534059:    511    590      1
1213744740.539058:    509    590      1
1213744740.544078:    420    589      1  <===
1213744740.549079:    509    591      1
1213744740.554078:    501    589      1
1213744740.559078:    511    588      1
1213744740.564079:    490    589      1  <===
1213744740.569058:    511    589      1
1213744740.574078:    512    588      1
1213744740.579078:    511    589      1
1213744740.584078:    513    588      1
1213744740.589078:    512    583      1
1213744740.594079:    510    589      1
1213744740.599098:    514    589      1
1213744740.604078:    510    588      1
1213744740.609058:    509    588      1
1213744740.614058:    514    588      1
1213744740.619078:    511    589      1
1213744740.624059:    517    589      1
1213744740.629057:    505    589      1
1213744740.634058:    511    589      1
1213744740.639089:    523    589      1  <===
1213744740.644116:    508    590      1
**/

static int sample_data[] = {
    100,
    101,
    102,
    103,
    104,
    105,
    511, /* start */
    513,
    511,
    509,
    485, /* <=== */
    509,
    506,
    511,
    507,
    511,
    511,
    509,
    420, /* <=== */
    509,
    501,
    511,
    490, /* <=== */
    511,
    512,
    511,
    513,
    512,
    510,
    514,
    510,
    509,
    514,
    511,
    517,
    505,
    511,
    523, /* <=== */
    508
};

void
test_smoothing()
{
    int i;
    int n;
    struct smooth s;
    struct smooth* p = &s;

    assert(p != NULL);
    init_smooth(p);
    assert(get_smooth(p) == 0);
    dump_smooth(p);

    n = sizeof(sample_data) / sizeof(int);
    for (i = 0; i < n; ++i) {
        put_smooth(p, sample_data[i]);
        sort_smooth(p);
        dump_smooth(p);
    }
}

int
main(int argc, char** argv)
{
    test_smoothing();
    return (exit(EXIT_SUCCESS), 0);
}

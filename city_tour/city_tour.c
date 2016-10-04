/*
Problem

During summer time, old cities in Europe are swarming with tourists who roam the
streets and visit points of interest.

Many old cities were built organically and not according to some architecture
plan, but, strangely, their growth exhibits a similar pattern: the cities
started from three points of interest, with each pair being connected by a
bidirectional street; then, gradually, new points of interest were added. Any
new point of interest was connected by two new bidirectional streets to two
different previous points of interest which were already directly connected by a
street.  A tourist visiting such a city would like to do a tour visiting as many
points of interest as possible. The tour can start at any point of interest and
must end at the same point of interest. The tour may visit each street at most
once and each point of interest at most once (with the exception of the first
point of interest which is visited exactly twice).

You are given the description of how the city grew. Find the largest number of
different points of interest a single tour can visit in this city.

Input

The first line of the input file contains the number of cases, T. T test cases
follow.

Each case begins with the integer N - the total number of points of interest in
the city. Points are denoted with numbers from 1 to N; numbers 1, 2, and 3
denote the three original points when the city started, while numbers 4, ..., N
denote the other points in the order they were added to the city.

The next N-3 lines each contain a pair of space-separated integers A, B,
indicating that the corresponding point of interest was connected by streets to
points A and B. First of these lines corresponds to point number 4, second to
point number 5, etc.

Output

For each test case, output one line containing "Case #x: y", where x is the case
number (starting from 1) and y is the largest number of points of interest a
tour can visit in this city.

Limits

1 ≤ T ≤ 50.

Small dataset

4 ≤ N ≤ 15.

Large dataset

4 ≤ N ≤ 1000.

Sample

---------------
Input   Output

2       Case #1: 4
5       Case #2: 6
1 2
2 1
6
1 2
1 4
4 5

*/

#include <stdlib.h>
#include <stdio.h>

#define ASSERT(x) do { if (!(x)) { fprintf(stderr,"assertion failed: %s\n", #x); exit(1); } } while (0)

/* #define DEBUG(...) fprintf(stderr, __VA_ARGS__) */
#define DEBUG(...) (void)0

typedef struct poi    poi;
typedef struct street street;
typedef struct end    end;

struct poi {
    int  visited; /* is visited = 1, or not = 0 */
    int  num;
    end* ends;    /* link list of connected street ends */
};

struct end {
    end*    next;
    poi*    poi;
    street* street; /* which street this belongs to */
};

struct street {
    int visited;
    end ends[2];
};

static street* street_new(poi* p0, poi* p1)
{
    street* me;
    ASSERT(me = (street*)calloc(1, sizeof(street)));

    me->ends[0].poi    = p0;
    me->ends[0].street = me;
    me->ends[0].next   = p0->ends;
    p0->ends           = &me->ends[0];

    me->ends[1].poi    = p1;
    me->ends[1].street = me;
    me->ends[1].next   = p1->ends;
    p1->ends           = &me->ends[1];

    return me;
}

static void street_delete(street* me)
{
    end** lfp;

    for (lfp = &me->ends[0].poi->ends;
         (*lfp)->street != me;
         lfp = &(*lfp)->next);

    *lfp = (*lfp)->next;

    for (lfp = &me->ends[1].poi->ends;
         (*lfp)->street != me;
         lfp = &(*lfp)->next);

    *lfp = (*lfp)->next;

    free(me);
}

static poi* pois;


static int dfs(poi* me, poi* start, int depth)
{
    int max = 0;

    if (me == start) {
        return 1;
    }

    if (me->visited) {
        return 0;
    }

    if (!start) {
        start = me;
    }

    {int n; for (n=0; n<depth; n++) DEBUG(" ");}
    DEBUG("visiting %ld\n", me-pois);

    me->visited = 1;
    {
        end* end;
        int  length = 0;

        for (end = me->ends; end; end = end->next) {
            ASSERT(end == &end->street->ends[0] ||
                   end == &end->street->ends[1]);

            if (end->street->visited) {
                continue;
            } else {

                poi* poi;

                end->street->visited = 1;

                if (&end->street->ends[0] == end) {
                    poi = end->street->ends[1].poi;
                    ASSERT(end->street->ends[0].poi == me);
                } else {
                    poi = end->street->ends[0].poi;
                    ASSERT(end->street->ends[1].poi == me);
                }
                length = dfs(poi, start, depth+1);
                if (length > max) {
                    max = length;
                }
                end->street->visited = 0;
            }
        }
    }
    me->visited = 0;

    return me == start ? max : max ? max + 1 : 0;
}

int main(void)
{
   int  num_cases;
   int  i;

   ASSERT(1 == fscanf(stdin, "%d", &num_cases));

   for (i = 1; i <= num_cases; i++) {
       int num_pois;

       ASSERT(1 == fscanf(stdin, "%d", &num_pois));

       ASSERT(num_pois >= 3);

//       if (3 == num_pois || 4 == num_pois) {
//           fprintf(stdout, "Case #%d: %d\n", i, num_pois);
//           continue;
//       }

       if (!(pois = (poi*)calloc(num_pois, sizeof(poi)))) {
           return 1;
       }

       street_new(&pois[0], &pois[1]);
       street_new(&pois[0], &pois[2]);
       street_new(&pois[2], &pois[1]);

       {
           int j;
           int max = 0;

           for (j = 3; j < num_pois; j++) {
               int n1, n2;
               ASSERT(2 == fscanf(stdin, "%d %d", &n1, &n2));

               street_new(&pois[j], &pois[n1-1]);
               street_new(&pois[j], &pois[n2-1]);
           }
           DEBUG("%d pois\n", num_pois);

           for (j = 0; j < num_pois; j++) {
               int length = dfs(pois+j, 0, 0);
               DEBUG("from %d got %d\n", j, length);
               if (length > max) {
                   max = length;
               }
           }
           fprintf(stdout, "Case #%d: %d\n", i, max);

           for (j = 0; j < num_pois; j++) {
               while (pois[j].ends) {
                   street_delete(pois[j].ends->street);
               }
           }
           free(pois);
           pois = 0;
       }
   }
   return 0;
}

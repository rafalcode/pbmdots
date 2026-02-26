/* This came from my old matread.c program
 * the idea here is to read coordinates froma text file
 * where each line has a coordinate (usually x and y) */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define GBUF 8
#define WBUF 8
#define TOPLINES 2

typedef unsigned char boole;

typedef struct /* px_t, pixel type */
{
    int x, y;
    int sz, buf;
} px_t;

typedef struct /* wseq_t */
{
    size_t *wln;
    size_t wsbuf;
    size_t quan;
    size_t lbuf; /* a buffer for the number of lines */
    size_t numl; /* number of lines, i.e. rows */
    size_t *wpla; /* words per line array: the number of words on each line */
} wseq_t;

wseq_t *create_wseq_t(size_t initsz)
{
    wseq_t *words=malloc(sizeof(wseq_t));
    words->wsbuf = initsz;
    words->quan = initsz;
    words->wln=calloc(words->wsbuf, sizeof(size_t));
    words->lbuf=WBUF;
    words->numl=0;
    words->wpla=calloc(words->lbuf, sizeof(size_t));
    return words;
}

void free_wseq(wseq_t *wa)
{
    free(wa->wln);
    free(wa->wpla);
    free(wa);
}

px_t *processinpf(char *fname, size_t *nr, size_t *nc)
{
    /* In order to make no assumptions, the file is treated as lines containing the same amount of words each,
     * except for lines starting with #, which are ignored (i.e. comments). These words are checked to make sure they contain only floating number-type
     * characters [0123456789+-.] only, one string variable is continually written over and copied into a growing floating point array each time */

    /* declarations */
    FILE *fp=fopen(fname,"r");
    int i;
    size_t couc /*count chars per line */, couw=0 /* count words */, oldcouw = 0;
    size_t coum=0, couw2=0;
    int c;
    boole inword=0;
    wseq_t *wa=create_wseq_t(GBUF);
    size_t bwbuf=WBUF;
    char *bufword=calloc(bwbuf, sizeof(char)); /* this is the string we'll keep overwriting. */

    px_t *px=malloc(GBUF*sizeof(px_t));
    px->sz=0;
    px->buf=GBUF;

    while( (c=fgetc(fp)) != EOF) {
        /*  take care of  */
        if( (c== '\n') | (c == ' ')) {
            if( inword==1) { /* we end a word */
                wa->wln[couw]=couc;
                bufword[couc++]='\0';
                bufword = realloc(bufword, couc*sizeof(char)); /* normalize */
                if(wa->numl == TOPLINES-1) {
                    // beware couw has already counted hte first word, so it will be 1
                    if(couw==1)
                        *nc=atoi(bufword);
                    else if(couw==2)
                        *nr=atoi(bufword);
                } else if(wa->numl>=TOPLINES) {
                    if(atoi(bufword) ==1) {
                        if(coum == px->buf-1) {
                            px->buf += GBUF;
                            px=realloc(px, px->buf*sizeof(px_t));
                        }
                        px[coum].x=(couw2-3)/(*nc);
                        px[coum].y=(couw2-3)%(*nc);
                        coum++;
                    }
                }
                couc=0;
                couw++;
                couw2++;
            }
            if(c=='#') {
                while( (c=fgetc(fp)) != '\n') ;
                continue;
            } else if(c=='\n') {
                if(wa->numl == wa->lbuf-1) {
                    wa->lbuf += WBUF;
                    wa->wpla=realloc(wa->wpla, wa->lbuf*sizeof(size_t));
                    memset(wa->wpla+(wa->lbuf-WBUF), 0, WBUF*sizeof(size_t));
                }
                wa->wpla[wa->numl] = couw-oldcouw;
                oldcouw=couw;
                wa->numl++;
            }
            inword=0;
        } else if( (inword==0) && ( (c == 0x50) | (c == 0x2B) | (c == 0x2D) | (c == 0x2E) | ((c >= 0x30) && (c <= 0x39))) ) { /* deal with first character of new word, + and - also allowed */
            if(couw == wa->wsbuf-1) {
                wa->wsbuf += GBUF;
                wa->wln=realloc(wa->wln, wa->wsbuf*sizeof(size_t));
                for(i=wa->wsbuf-GBUF;i<wa->wsbuf;++i)
                    wa->wln[i]=0;
            }
            couc=0;
            bwbuf=WBUF;
            bufword=realloc(bufword, bwbuf*sizeof(char)); /* don't bother with memset, it's not necessary */
            bufword[couc++]=c; /* no need to check here, it's the first character */
            inword=1;
        } else if( (c == 0x2E) | (c == 0x50) | ((c >= 0x30) && (c <= 0x39)) ) {
            if(couc == bwbuf-1) { /* the -1 so that we can always add and extra (say 0) when we want */
                bwbuf += WBUF;
                bufword = realloc(bufword, bwbuf*sizeof(char));
            }
            bufword[couc++]=c;
        } else {
            printf("Error. Non-numeric+P character detected. This program is only for reading P1 pbm files\n"); 
            free_wseq(wa);
            exit(EXIT_FAILURE);
        }

    } /* end of big for statement */
    fclose(fp);
    free(bufword);

    /* normalization stage */
    wa->quan=couw;
    px->sz=coum;
    wa->wln = realloc(wa->wln, wa->quan*sizeof(size_t)); /* normalize */
    px = realloc(px, px->sz*sizeof(px_t)); /* normalize */
    wa->wpla= realloc(wa->wpla, wa->numl*sizeof(size_t));

    free_wseq(wa);

    return px;
}

int main(int argc, char *argv[])
{
    /* argument accounting */
    if(argc!=2) {
        printf("Error. Pls supply argument (name of text file).\n");
        exit(EXIT_FAILURE);
    }

    int i,j;
    size_t nr, nc;
    px_t *px=processinpf(argv[1], &nr, &nc);

    // alternative printing methods:
    // for(i=0;i<px->sz;++i)
    //     printf("(%i,%i) ", px[i].x, px[i].y);
    // printf("\n"); 
    //
    // x on one row csv'd
    // for(j=0;j<px->sz;++j)
    //     printf((j!=px->sz-1)?"%i,":"%i\n", px[j].x);
    // y on one row csv'd
    // for(j=0;j<px->sz;++j)
    //     printf((j!=px->sz-1)?"%i,":"%i\n", px[j].y);
    for(j=0;j<px->sz;++j)
        printf("%i,%i\n", px[j].x, px[j].y);

    // for(j=0;j<n;++j) {
    //     prev = mat[j]; // first row-number each time
    //     cou=1;
    //     for(i=1;i<m;++i) {
    //         if(prev==mat[i*n+j]) {
    //             cou++;
    //             if(i==(m-1))
    //                 printf("%ix%i ", prev, cou);
    //         } else {
    //             if(cou==1)
    //                 printf("%i ", prev);
    //             else
    //                 printf("%ix%i ", prev, cou);
    //             cou=1;
    //             prev=mat[i*n+j];
    //         }
    //     }
    //      printf("\n"); 
    // }

    free(px);

    return 0;
}

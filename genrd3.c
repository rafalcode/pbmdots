/* This came from my old matread.c program
 * the idea here is to read coordinates froma text file
 * where each line has a coordinate (usually x and y) */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define GBUF 8
#define WBUF 8
#define TOPLINES 2
#define SZBX 2

//from manual inspection
// r7
// #define BLANKBLOCKX 57
// #define BLANKBLOCKY 28
//r3
#define BLANKBLOCKX 59
#define BLANKBLOCKY 44

#define CONDREALLOC(x, b, c, a, t); \
    if((x)>=((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
    }

typedef unsigned char boole;

typedef struct /* px_c */
{
    int x,y;
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

typedef struct /* av_c */
{
    int vbf, vsz;
    px_t *p;
} av_c;

av_c *crea_avc(int vbf)
{
    av_c *avc=malloc(sizeof(av_c));
    avc->vbf=vbf;
    avc->p=malloc(avc->vbf*sizeof(px_t));
    avc->vsz=0;
    return avc;
}

void condrea_avc(av_c *avc)
{
    /* somewhat trivial, but idea is that, as avc is a container, it can be re-alloced inside a function */
    CONDREALLOC(avc->vsz, avc->vbf, GBUF, avc->p, px_t);
    return;
}

void norm_avc(av_c *avc)
{
    /* somewhat trivial, but idea is that, as avc is a container, it can be re-alloced inside a function */
    avc->p=realloc(avc->p, avc->vsz*sizeof(px_t));
    return;
}

void free_avc(av_c *avc)
{
    free(avc->p);
    free(avc);
    return;
}

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

int *processinpf(char *fname, int *m, int *n)
{
    /* In order to make no assumptions, the file is treated as lines containing the same amount of words each,
     * except for lines starting with #, which are ignored (i.e. comments). These words are checked to make sure they contain only floating number-type
     * characters [0123456789+-.] only, one string variable is continually written over and copied into a growing floating point array each time */

    /* declarations */
    FILE *fp=fopen(fname,"r");
    int i;
    size_t couc /*count chars per line */, couw=0 /* count words */, oldcouw = 0;
    size_t coum=0;
    int c;
    boole inword=0;
    wseq_t *wa=create_wseq_t(GBUF);
    size_t bwbuf=WBUF;
    char *bufword=calloc(bwbuf, sizeof(char)); /* this is the string we'll keep overwriting. */
    size_t nc, nr;

    int *mat=malloc(GBUF*sizeof(int));

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
                        nc=atoi(bufword);
                    else if(couw==2)
                        nr=atoi(bufword);
                } else if(wa->numl>=TOPLINES) {
                    mat[coum]=atoi(bufword);
                    coum++;
                }
                couc=0;
                couw++;
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
                mat=realloc(mat, wa->wsbuf*sizeof(float));
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
    wa->wln = realloc(wa->wln, wa->quan*sizeof(size_t)); /* normalize */
    mat = realloc(mat, wa->quan*sizeof(int)); /* normalize */
    wa->wpla= realloc(wa->wpla, wa->numl*sizeof(size_t));

    // *m= wa->numl -TOPLINES;
    *m=nr;
    *n=nc; 
    free_wseq(wa);

    return mat;
}

void postpromat(int *mat, int n, int blx, int bly) /* manual inspection allows to blank out a big square: topleft */
{
    int i, j;
    for(i=0;i<bly;++i) 
        for(j=0;j<blx;++j) 
            mat[n*i+j]=0;
}

void plainprt(int *mat, int m, int n)
{
    int i, j;
    printf("Matrix is %i rows by %i columns and is as follows:\n", m, n); 
    // plain print:
    for(i=0;i<m;++i) {
        for(j=0;j<n;++j) 
            printf("%i", mat[i*n+j]);
        printf("\n"); 
    }
}

void prtoc(unsigned char *oc, int m, int n)
{
    int i, j;
    unsigned char tc;
    for(i=0;i<m-SZBX+1;++i) {
        for(j=0;j<n-SZBX+1;++j) {
            tc = (oc[(n-SZBX+1)*i+j] ==0)? ' ' : oc[(n-SZBX+1)*i+j];
            printf((tc==' ')? "%c" : "%X", tc);
        }
        printf("\n"); 
    }
}

void emuim(av_c *avc, int m, int n) /* emulate the image with avec */
{
    int i, j;
    unsigned char *mat2=calloc(m*n, sizeof(unsigned char));
    for(i=0;i<avc->vsz;++i)
        mat2[n*avc->p[i].y + avc->p[i].x]=1; 

    for(i=0;i<m;++i) {
        for(j=0;j<n;++j) 
            printf((mat2[i*n+j]==0)? " ": "1");;
        printf("\n"); 
    }
    free(mat2);
}

void twos(unsigned char *oc, int m, int n, av_c *avc)
{
    int i, j;
    // identifying the isolated clums of 1's and assigning an index.
    // to make code simple it's useful to have a number that always appears. 2 does. so does 1, but, 2 is easier.
    // it ise unique except in 1011, we will skip that.
    // then index assignment need only vary for 2 of the 8 types.
    // perhaps I should write up why there are only 8 types. see procedure.txt
    int tcou=0;
    for(i=0;i<m-SZBX+1;++i)
        for(j=0;j<n-SZBX+1;++j) {
            if(oc[(n-SZBX+1)*i+j]==2) {
                tcou++;
                if(oc[(n-SZBX+1)*i+j-1]==0x0B) // 1011(B) before it, 2 appears twice in this one (otherwise it is unique) - do not count
                    continue;
                if(oc[(n-SZBX+1)*(i-1)+j-1]==0x07) {
                    // 0111, we chose bottom left 1 so two directly below
                    avc->p[avc->vsz].x=j;
                    avc->p[avc->vsz++].y=i+2;
                } else if(oc[(n-SZBX+1)*(i-1)+j-1]==0x0E) {
                    avc->p[avc->vsz].x=j-1;
                    avc->p[avc->vsz++].y=i; // because it's 0111
                } else {
                    // default choose index directly below.
                    avc->p[avc->vsz].x=j;
                    avc->p[avc->vsz++].y=i+1; // because it's 0111
                }
            }
        }
    // printf("#twos:%i\n", tcou); 
}

void twosr(unsigned char *oc, int m, int n, av_c *avc) // reverse the y coord so that it increase going up
{
    int i, j;
    // identifying the isolated clums of 1's and assigning an index.
    // to make code simple it's useful to have a number that always appears. 2 does. so does 1, but, 2 is easier.
    // it ise unique except in 1011, we will skip that.
    // then index assignment need only vary for 2 of the 8 types.
    // perhaps I should write up why there are only 8 types. see procedure.txt
    int tcou=0;
    for(i=0;i<m-SZBX+1;++i)
        for(j=0;j<n-SZBX+1;++j) {
            if(oc[(n-SZBX+1)*i+j]==2) {
                tcou++;
                if(oc[(n-SZBX+1)*i+j-1]==0x0B) // 1011(B) before it, 2 appears twice in this one (otherwise it is unique) - do not count
                    continue;
                // watch the +1 and -1's these are due the zero indexing to one-indexing change.
                if(oc[(n-SZBX+1)*(i-1)+j-1]==0x07) {
                    // 0111, we chose bottom left 1 so two directly below
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-2-1;
                } else if(oc[(n-SZBX+1)*(i-1)+j-1]==0x0E) {
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-1; // because it's 0111
                } else {
                    // default choose index directly below.
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-1; // because it's 0111
                }
            }
        }
    // printf("#twos:%i\n", tcou); 
}

void twosr2(unsigned char *oc, int m, int n, av_c *avc) // this is for dealing with bigger blocks
{
    int i, j;
    // identifying the isolated clums of 1's and assigning an index.
    // to make code simple it's useful to have a number that always appears. 2 does. so does 1, but, 2 is easier.
    // it ise unique except in 1011, we will skip that.
    // then index assignment need only vary for 2 of the 8 types.
    // perhaps I should write up why there are only 8 types. see procedure.txt
    int tcou=0;
    int to8; // how many steps to the "8" directly below "2"
    for(i=0;i<m-SZBX+1;++i)
        for(j=0;j<n-SZBX+1;++j) {
            if(oc[(n-SZBX+1)*i+j]==2) {
                tcou++;
                // get the 2to8"
                to8=0;
                while(oc[(n-SZBX+1)*(i+(to8++))+j] != 8) ; // go down, and find the "8". there's nearly always an 8 som e rows under "2"
                printf("to8=%i\n", to8); 
                if(oc[(n-SZBX+1)*i+j-1]==0x0B) // 1011(B) before it, 2 appears twice in this one (otherwise it is unique) - do not count
                    continue;
                // watch the +1 and -1's these are due the zero indexing to one-indexing change.
                if(oc[(n-SZBX+1)*(i-1)+j-1]==0x07) {
                    // 0111, we chose bottom left 1 so two directly below
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-2-1;
                } else if(oc[(n-SZBX+1)*(i-1)+j-1]==0x0E) {
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-1; // because it's 0111
                } else {
                    // default choose index directly below.
                    avc->p[avc->vsz].x=j+1;
                    avc->p[avc->vsz++].y=m-i-1; // because it's 0111
                }
            }
        }
    // printf("#twos:%i\n", tcou); 
}

void ones(unsigned char *oc, int m, int n, av_c *avc)
{
    int i, j;
    int tcou=0;
    for(i=0;i<m-SZBX+1;++i) {
        for(j=0;j<n-SZBX+1;++j) {
            if(oc[(n-SZBX+1)*i+j]==2) {
                printf("oc-ij:%X ocijm1:%X\n", oc[(n-SZBX+1)*i+j], oc[(n-SZBX+1)*(i+1)+j-1]);
                tcou++;
            }
        }
    }
    printf("#ones:%i\n", tcou); 
}

int main(int argc, char *argv[])
{
    /* argument accounting */
    if(argc!=2) {
        printf("Error. Pls supply argument (name of text file).\n");
        exit(EXIT_FAILURE);
    }

    int i, j, m, n;
    int *mat=processinpf(argv[1], &m, &n);

    postpromat(mat, n, BLANKBLOCKX, BLANKBLOCKY);
    // plainprt(mat, m, n);

    unsigned char t;

    av_c *avc=crea_avc(GBUF);
    unsigned char *oc = calloc((m-SZBX+1)*(n-SZBX+1), sizeof(unsigned char));

    // Set 2x2 neighbourhood for each pixel.
    // last column and last row cannot have points.
    for(i=0;i<m-SZBX+1;++i)
        for(j=0;j<n-SZBX+1;++j) {
            t=0x00;
            t=(0x01&mat[i*n+j])<<3;
            t|=(0x01&mat[i*n+j+1])<<2;
            t|=(0x01&mat[(i+1)*n+j])<<1;
            t|=0x01&mat[(i+1)*n+j+1];
            oc[(n-SZBX+1)*i+j] =t;
        }
    // prtoc(oc, m, n);
    twosr2(oc, m, n, avc); // populates avc

    // print out points for easy loading onto R
    printf("# width=%i height=%i\n", n, m);
    for(i=0;i<avc->vsz;++i)
        printf("%i,%i\n", avc->p[i].x, avc->p[i].y); 

    free(mat);
    free(oc);
    free_avc(avc);

    return 0;
}

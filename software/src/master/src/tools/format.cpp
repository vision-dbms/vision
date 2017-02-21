/*------------------------------------------------------
 *   COMPILE USING FOLLOWING OPTIONS:
 *   HP 9000:   cc format.c -Wc,-Ni500 -o format
 *    Apollo:   cc format.c -O -o format
 *------------------------------------------------------
*/

/*-----------------------
 * Compilation Notes:
 *  HP:  increase internal compiler table size (default = 300)
 *       due to increased number of options in CODEWORD decode(s)
 *
 *  Apollo:  executable created for machine type where compilation was run
 *           - to create DN10000 executable from DN4000, eg, use options:
 *             cc format.c -O -A cpu,a88k -o format
 *-----------------------
 *  Other:
 *  Input file need not end with '\n' for loop to end
 *-----------------------
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Vp.h"

#define	ErrorExitValue	1
#define NormalExitValue	0

#define    ZERO     0

#define    YES      1
#define    NO       0

#define    ON       1
#define    OFF      0

#define    INt      0
#define    CHR      1
#define    STR      2
#define    DBL      3
#define    EXP      4

#define    LENGTH  20000   /* allocated length of _line[] */
#define    MAXSPC      1000   /* general "max space allocated" value */
#define    NSEC       31   /* sections 0-30 allocated */
#define    PAGE      200   /* max # lines/page */
 
#define    NL         56   /* default value: # lines printed/std page */
#define    TOP         5   /* default value: # lines for top margin */
#define    WIDTH      80   /* default value: # chars/line/std page */


#define    FREELINES  _lmax - _lcount    /* free space left on page */


typedef enum {Block, BlockTop, Concat, ContLabel, Control, FFoff, Footer,
              Header, HeaderNL, MultiTrailer, NewPg, Page, PgCnt, PgFormat,
              PgLine, Report, SecWidth, Skip, Top, Trailer, NotKnown,
              WideReport, Width} CODEWORD;


           /********   STRUCTURE DEFINITIONS   *********/

struct section {   /* section profile */
    int  n_lines;    /* # lines in section */
    int  max_len;    /* max string length of section */
    char **ptr;      /* (pter to pter) first line of section */
} TXT, SECTION[NSEC];

struct collection {  /* _tmp[] profile (collected sections) */
    int  max_sec;    /* largest section wrt lines */
    int  sum_len;    /* sum of max str lengths */
    int  lines;      /* total # lines in _tmp[] */
    int  ctr;        /* counter decr each time section saved */
    char **pter;     /* pts to beginning of _tmp[] */
} TMP;



        /*********   GLOBAL VARIABLE DEFINITIONS   *********/

int    _blockTop = OFF;     /* (input var) user can turn it ON */
int    _contLabel = OFF;    /* (input var) user can print label (ON) */
int    _formFeed = 0;       /* (input var) user can turn OFF formfeed */
int    _hdrNL = ON;         /* (input var) user can turn OFF printing NL after header */
int    _multiTrailer = OFF; /* (input var) user can turn multiTrailer ON */
int    _pgCnt = ON;         /* (input var) user can turn pg counting OFF */
int    _pgCntOrig = ON;     /* save original pg cnt setting */
int    _pgLen = NL;         /* (input var) no. lines printed/page */
int    _pgLine = 2;         /* (input var) location of pg line (DEFAULT = 2) */
int    _secWidth = 0;       /* (input var) section width set by user */
int    _skip = 0;           /* (input var) no. lines printed before skip line */
int    _topMarg = TOP;      /* (input var) no. lines for top margin */
int    _wideReport = OFF;   /* (input var) user can turn wideReport ON */
int    _width = WIDTH;      /* (input var) max no. chars printed/line/page */

int    _haveTrailer = NO;   /* YES if collected trailer text */
int    _inReport = NO;      /* YES if within a report */
int    _lcount = 0;         /* line counter (max value = _lmax) */
int    _lmax = 0;           /* _topMarg + _pgLen */
int    _nlFooter = 0;       /* no. lines per footer */
int    _nlHeader = 0;       /* no. lines per header */
int    _nlTrailer = 0;      /* no. lines per trailer */
int    _pg = 0;             /* page counter */
int    _rpt = 0;            /* report counter for error messages */
int    _skipCnt = 0;        /* skipline counter */
int    _symbol = '#';       /* token indication position of pg no. in pgline */
int    _widePgFmt = NO;     /* flags if PGFORMAT option set by WIDEREPORT spec */
int    _widePgLine = NO;    /* flags if PGLINE option set by WIDEREPORT spec */
int    _wroteHeader = NO;   /* flags if header was written */


char   _line[LENGTH];       /* holds input string */
char   _option[15];         /* option codeword: 14 chars + \0 */

char  errorI[] = "*** ERROR: TOO MANY LINES ***";
char  Null[]   = "";           /* null string used for adding blank lines */
char  Pg[]     = "  Page %d";  /* default format string for pg line */
char  _type[]  = { 'd',     /* decimal integer */
                   'c',     /* character */
                   's',     /* string */
                   'f',     /* double - decimal */
                   'e' };   /* double - exponential */



char   *_footer[PAGE];      /* lineptr array storing footer */
char   *_header[PAGE];      /* lineptr array storing header */
char   *_trailer[PAGE];     /* lineptr array storing trailer */
char   *_pgFormat[TOP];     /* lineptr array of format strings for pg line */
char   *_tmp[MAXSPC];          /* lineptr array of collected sections */
char   *_pf = Pg;           /* pter to pg line string (default set) */
char   *_rawp = ZERO;       /* pter to unconverted pg line string */
char  **_plabel = ZERO;     /* ptr to ptr to label string */


          /************  UTILITY DEFINITIONS   *************/
static int S_LEN () {
  size_t sLine = 0;
  if (fgets (_line, LENGTH, stdin)) {
    sLine = strlen (_line);
    while (sLine > 0 && ('\n' == _line[sLine - 1] || '\r' == _line[sLine - 1]))
      _line[--sLine - 1] = '\0';
  }
  return (int)sLine;
}

static void abortMain()
{
   fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                  "Problem with memory allocation",
                  "... PROCESS ABORTED\n\n");
   exit(ErrorExitValue);         /* aborts main routine */  
}


static CODEWORD decode (       /* decodes option associated with string */
    char *s                 /* returns corresponding codeword */
)
{  /** NOTE: change similar codewords (BLOCK/BLOCKTOP: 1st 5 chars SAME for BOTH */
   /** OR: ensure strncmp decreases from highest to lowest # chars (current fix) **/
   return
      (strncmp (s, "BLOCKTOP",     8) == 0) ? BlockTop :
      (strncmp (s, "BLOCK",        5) == 0) ? Block :
      (strncmp (s, "CONCAT",       6) == 0) ? Concat :
      (strncmp (s, "CONTINUELABEL",13) == 0) ? ContLabel :
      (strncmp (s, "CTRL",         4) == 0) ? Control :
      (strncmp (s, "FFOFF",        5) == 0) ? FFoff:
      (strncmp (s, "FF_OFF",       6) == 0) ? FFoff:
      (strncmp (s, "FOOTER",       6) == 0) ? Footer :
      (strncmp (s, "HEADERNL",     8) == 0) ? HeaderNL :
      (strncmp (s, "HEADER",       6) == 0) ? Header :
      (strncmp (s, "MULTITRAIL",  10) == 0) ? MultiTrailer :
      (strncmp (s, "NEWPAGE",      7) == 0) ? NewPg :
      (strncmp (s, "PAGELEN",      7) == 0) ? Page :
      (strncmp (s, "PGCNT",        5) == 0) ? PgCnt :
      (strncmp (s, "PGLINE",       6) == 0) ? PgLine :
      (strncmp (s, "PGFORMAT",     8) == 0) ? PgFormat :
      (strncmp (s, "REPORT",       6) == 0) ? Report :
      (strncmp (s, "SECWIDTH",     8) == 0) ? SecWidth :
      (strncmp (s, "SKIPLINE",     8) == 0) ? Skip :
      (strncmp (s, "TOPMARGIN",    9) == 0) ? Top :
      (strncmp (s, "TRAILER",      7) == 0) ? Trailer :
      (strncmp (s, "WIDEREPORT",  10) == 0) ? WideReport :
      (strncmp (s, "WIDTH",        5) == 0) ? Width :
      NotKnown;
}


static int dump(   /* fill v[] with maxl lines from _tmp[]; return maxl */
    char *v[]
)
{
   int  i, maxl, nl = TMP.lines;
    
   if (FREELINES < nl)
      maxl = FREELINES;
   else
      maxl = nl;
   for (i=0; i < maxl; i++)
      *v++ = *TMP.pter++;
   return (maxl);
}

static void formFeed()           /* user may specify when to use ff */
{
   if (_formFeed != ZERO && _lcount == _formFeed)
      ;    /* do not add formfeed to file */
   else
      printf("\f");
}


static void freeLines(     /* free allocated memory: nl lines of lineptr array v*/
    char *v[],
    int  nl
)
{
   while (--nl >=0) {
      if (v[nl] != ZERO) {    /* NOTE: error if try to free a ZERO pter */
         free(v[nl]);
         v[nl] = ZERO; 
      }
   }
}


static void initPtrArray(       /* initialize lineptr array elements */
    char *v[],
    int  size
)
{
   int  i;

   for (i=0; i < size; ++i)
      v[i] = ZERO;
}


static char *insertStr(
    char *s,        /* string which requires an insertion */
    int  slen,      /* length of string s, excluding terminating '\0' char */
    char *x,        /* ptr to location within s where insertion belongs */
    int  conv_arg  /* arg specifies what conversion type should be inserted */
)
{
    int   i;
    char *t;
    char *p;  /* ptr to newly converted line */
    
    if ((p = t = (char*)malloc((slen+2)*sizeof(char))) == ZERO)
       abortMain();
    else {
       p = t;  
       for (i=slen; i > 0 && (*t++ = *s++) != '\0'; --i) {
          if (s == x) {   /* found location of token; insert the following: */
             *t++ = '%';
             *t++ = _type[conv_arg];
             s++;
          }
       }
    }
    return (p);   /* return address of converted line t */
}


static int convertLine(
    char *v[],    /* lineptr array containing line needing conversion */
    int  nl,      /* total no. lines in v[] */
    int  token,   /* symbol in string indicating location of insertion */
    int  type     /* conversion specification (eg. INt, CHR, STR)*/
)
{
    char *p;          /* ptr to token position in string */
    char *s;          /* ptr to newly converted line */
    int  index = 0;   /* default offset value */
    int  cnt = nl, len;

    while (cnt > 0 && (p = strrchr(v[index], token)) == ZERO) {
       --cnt;
       ++index;
    }
    if (p != ZERO && (len = strlen(v[index])) > 0 ) {  /* got line with token */
       if ((_rawp = (char*)malloc((len+1)*sizeof(char))) == ZERO)
          abortMain();
       else 
          strcpy(_rawp, v[index]);   /* copy unconverted line to "raw" ptr */
       s = insertStr(v[index], len, p, type);   /* get new converted line */
       free(v[index]);   /* free space associated w/ orig. line */
       v[index] = s;     /* assign ptr to reference new line */
       return(index);    /* conversion successful */
    }
    else     /* lineptr array has no line containing token */
       return(-1);       /* conversion unsuccessful */
}


static void processTrailer()
{
   int tmp = _topMarg + _pgLen - _nlTrailer ;

   if (_nlTrailer) {    /* check if have trailer */
      if (_lcount < tmp) { /* if current line count can accommodate trailer */
         _lmax = tmp;          /* set new _lmax */
         _haveTrailer = YES;   /* set trailer flag */
      }
   }
   else {
      _haveTrailer = NO;
      _lmax = _topMarg + _pgLen;  /* recalc _lmax */
   }
}


static int restring(   /* strings sections together into out[] */
    int  maxl,  /* # lines in longest section */
    int  ns,    /* # sections to be processed */
    char *out[],/* returns # lines processed into out[] */
    int  tab    /* spacing between sections */
)
{
   int  a, b, i, j, newpad, offset, pad;
   char *s;
    
   for (i=0; i < maxl; i++) {
      if ((s = (char*)malloc((_width+1)*sizeof(char))) == ZERO)
         abortMain();
      else {
         for (j=1, offset=pad=0; j <= ns; j++) {
            if (SECTION[j].n_lines > 0) {        /* section printable */
               a = pad;
               b = SECTION[j].max_len;
               sprintf(s+offset, "%*s%-*s", a, "", b, *SECTION[j].ptr++);
               --SECTION[j].n_lines;
               offset += (a + b);
               if (pad == 0)
                  pad = tab;
            }
            else {              /* end of section */
               if (SECTION[j].max_len > 0) {  /* section existed or _secWidth set */
                  newpad = (SECTION[j].max_len + tab);
                  sprintf(s+offset, "%*s", newpad, "");  /* pad w/ blanks */
                  offset += newpad;
               }
               else {      /* section was empty and _secWidth not set */
                  fprintf(stderr,"R%d: missing Section %d  p.%d\n", _rpt, j, _pg);
                  fprintf(stderr,"     Check @@@SECTION line for errors\n\n");
               }
            }
         }
      }
      *out++ = s;     /* fill lineptr array */
   }
   return (maxl);     /* return no. lines in *out */
}

static int saveLines(   /* save lines in v[] (lineptr array) */
    char *v[],                             /* return signal: 1 or ZERO */
    int  maxstrlen,
    int  maxlines
)
{
   int   err = 1, flag, i, len, max = 0;
   char *s;
   char **t = v;           /* record start of array; **t (ptr to a ptr) */
    
   while ((len = S_LEN()) >= 0 && (flag = strncmp(_line,"@@@",3)) != 0) {
      if (len > maxstrlen) {
         if (err == 1) {
            fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                           "Input line too long",
                           "    Formatter will truncate line with ***");
            --err;
         }
         len = maxstrlen;
         for (i=3; i > 0; i--)
            _line[len-i] = '*';
        _line[len] = '\0';
      }
      if ((s = (char*)malloc((len+1)*sizeof(char))) == ZERO)
         abortMain();
      else if ((v-t) == maxlines) {     /* section too big! */
         fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                        "Section has too many lines",
                        "    Check @@@ option line locations");
         if ((len = strlen(errorI)) > max )
            max = len;
         free(s);    /* MUST FREE space malloc'd above before malloc'ing again! */
         s = (char*)malloc((len+1)*sizeof(char));   /* use new len value */
         strcpy(s, errorI);
         *--v = s;        /* reset last line as errorI line */
         TXT.n_lines = maxlines;
         TXT.max_len = max;
         TXT.ptr = t;
         return(1);       /* signal: struct memb values assigned */
      }
      else {
         strcpy(s, _line);
         *v++ = s;        /* fill linepter array */
         if (len > max)
            max = len;   /* record longest line length */
      }
   }
   if (flag == 0) {
      if (v == t)          /* both pt to same location */
         return (ZERO);
      else {               /* assign values to struct members */ 
         TXT.n_lines = (v - t);
         TXT.max_len = max;
         TXT.ptr = t;
         return (1);      /* signal: struct memb values assigned */
      }
   }
   return ZERO;
}


static void writeTopMargin()
{
   int i;

   if (_pgCnt == OFF || _pgLine > _lmax) {  /* print top margin only */
      for (i=0; i < _topMarg; ++i)
         printf("\n");
      _lcount = _topMarg;
   }
   else {  /* page counting is ON */
      for (i=1; i <= _topMarg; ++i) {   /* REM: default _pgLine = 2 */
         if (_pgLine == i) {
            printf(_pf, _pg);
            printf("\n");
         }
         else
            printf("\n");
      }
      _lcount = _topMarg; 
      if (_pgLine > _topMarg) {  /* default: pg line printed after top margin */
        printf(_pf, _pg);
        printf("\n");
        ++_lcount;
      }
   }
}


static void writeLines(      /* print nlines from lineptr array */
    char *v[],                /* update global line counter */
    int  nlines
)
{
   int  i = nlines;

   while (--i >= 0)
      printf ("%s\n", *v++);
   _lcount += nlines;
}


static void writeHeader()   /* print header; add _hdrNL newlines */
{                /* update global line counter */
   int i;
   
   writeLines(_header, _nlHeader);
   for (i=0; i < _hdrNL; i++) {   /* _hdrNL default = 1 */
      printf("\n");
      ++_lcount;
   }
}


static void beginPg()        /* print top margin, pg line (if specified), header */
{                /* process trailer if exists (recalc _lmax) */
   if (_pgCnt == ON)
      ++_pg;
   writeTopMargin();
   if (_nlHeader) {
      writeHeader();      /* _lcount updated by function */
      _wroteHeader = YES;  /* flags that header was written */
   }
   processTrailer();
}

/***** ONLY USED BY FOOTER OPTION .. soon to be phased out *****/
static void endPg()           /* complete bottom margin and go to new page */
{
   int  j;

   if (_pgLine > _lmax) {    /* user set pg line within bottom margin */
      if (_lcount < _lmax) {     /* add necessary blank lines */
         for (j=_lcount; j < _lmax; ++j)
            printf("\n");
      }
      for (j=_lmax+1; j < _pgLine; ++j)
          printf("\n");
      printf(_pf, _pg);         /* print pg line */
   }
   formFeed();     /* go to new page */
   _lcount = 0;     /* clear line counter */
}


static void writeFooter()   /* print footer and go to new page */
{
   printf("\n\n");       /* skip 2 lines before printing footer */
   _lcount += 2;
   writeLines(_footer, _nlFooter);
   freeLines(_footer, _nlFooter);
   _nlFooter = 0;
   endPg();         /* go to next page */
}


static void writeTrailer()   /* print trailer, flush buffer if multiTrailer OFF */
{
   writeLines(_trailer, _nlTrailer);
   if (_multiTrailer == 0) {  /* trailer only appears on single page */
      freeLines(_trailer, _nlTrailer);
      _nlTrailer = 0;
   }
}

static void finishPg()         /* finish processing current page */
{
   int j;
   
   if (_lcount < _lmax) {     /* add blank lines if necessary */
      for (; _lcount < _lmax; _lcount++)
         printf("\n");
   }
   if (_haveTrailer)  /* if have trailer, print it */
     writeTrailer();
   if (_pgLine > _lcount) {    /* user set pg line within bottom margin */
      for (j=_lcount+1; j < _pgLine; ++j)
          printf("\n");
      printf(_pf, _pg);   /* print pg line */
   }
   formFeed();     /* go to new page */
   _lcount = 0;    /* clear line counter */
}


static int collect(      /* collects n sections into _tmp[] */
    int  nsec        /* returns signal:  1 or ZERO */
)
{
   int  i, num;
   int  count = nsec, length = 0, maxlines = 0, nl = 0;
   char **t = _tmp;
    
        /* initialize structure members: 0-30 */
        /* 0 is empty place holder; users number sections 1-30 */
   for (i=0; i < NSEC; i++) {   
      SECTION[i].n_lines = 0;
      SECTION[i].max_len = _secWidth;  /* default: _secWidth = 0 */
   }
                           /* collect sections */
   while (S_LEN() >= 0 && strncmp(_line,"@@@SEC",6) == 0) {
      sscanf(_line, "%*[@@@SECTION] %d\n", &num);
      if (saveLines(_tmp+nl, _width, MAXSPC) != ZERO) {
         SECTION[num].n_lines = TXT.n_lines;
         SECTION[num].max_len = TXT.max_len; 
         SECTION[num].ptr = TXT.ptr;
         if (maxlines < TXT.n_lines)
            maxlines = TXT.n_lines;       /* find longest section */
         length += SECTION[num].max_len;   /* add max line lengths */
         nl += TXT.n_lines;                /* calc next free _tmp[] index */
         --count;                          /* saved section - record it */
      }
      else {       /* SECTION EMPTY */
          fprintf(stderr,"R%d: Section %d is empty!  p.%d\n", _rpt, num, _pg);
          fprintf(stderr,"    Check option line for possible errors\n\n");
      }
   }
   if (nl == 0)    /* nothing collected */
      return (ZERO);
   else {                        
      TMP.max_sec = maxlines;    /* assign values to struct members */
      TMP.sum_len = length;
      TMP.lines = nl;
      TMP.ctr = count;
      TMP.pter = t;
      return (1);                 /* signal assignment successful */
   }
}


int concat(   /* concatenates & reprocesses sections into output[] */
    int  nsec,                /* returns # lines in output[] */
    char *output[]
)
{
   int  i, l, last = 0,   /* explicit init required for apollo */
        len, maxl, maxlines, nl, ns, num, tab;

   if (collect(nsec) == ZERO)     /* no sections collected */
      return (ZERO); 
   else {
      len = TMP.sum_len;
      maxlines = TMP.max_sec;
   }
   if (strcmp(_line, "@@@CONCAT 0") == 0) {
      if (TMP.ctr != 0) {  /* problem with section collection */
         ns = nsec;            /* record expected # sections (input) */
         nsec -= TMP.ctr;      /* calc actual # sections collected */
         fprintf(stderr,"R%d: Formatter expected %d sections, but collected %d !\n", _rpt, ns, nsec);
         fprintf(stderr,"     Check @@@CONCAT line for errors  p.%d\n\n", _pg);
         if (_secWidth) {       /* process expected # sections */
            nsec = ns;
            while (TMP.ctr--) 
                len += _secWidth;   /* calc expected total length */
         }
      }
      if (nsec == 1) {           /* process single section */
         for (i=1; i < NSEC; i++) {    /* find that section */
            if ((l = SECTION[i].n_lines) > 0) {
               nl = l;
               num = i;
            }
         }
         for (i=0; i < nl; i++) 
            *output++ = *SECTION[num].ptr++;    
         return (nl);
      }
      if (len >= _width) {  /* sections can't fit on page */
         fprintf(stderr,"R%d: Sections can't fit across the page\n", _rpt);
         fprintf(stderr,"    Length = %d but page width = %d\n", len, _width);
         fprintf(stderr,"    Formatter will adjust text this time\n\n");
         last = nsec--;     /* remove last section */
         if ((l = SECTION[last].max_len) > 0)
            len -= l;       /* calc new length */
         else {   /* sections aren't numbered sequentially - dump output */
            fprintf(stderr,"R%d: Sections aren't numbered (1,2,3..)\n", _rpt);
            fprintf(stderr,"    Can't concatenate; will print as much text as possible\n\n");
            return (dump(output));
         }
      }          /* sections can fit on page */
      if (nsec > 1 && (tab=(_width-len)/(nsec-1)) >= 1)  {
         nl = restring(maxlines, nsec, output, tab);
         if (last > 0 && (maxl = SECTION[last].n_lines) > 0) {
            output[nl] = ZERO;   /* remember: skipped line is ZERO pter */
            output += (nl+1);    /* move to next free position */
            for (i=0; i < maxl; i++)
               *output++ = *SECTION[last].ptr++;
            nl += (maxl+1);
         }
         return (nl);
      }
      else {
         fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                        "Formatter can't adjust sections to fit across page",
                        "   Will print as much section text as possible");
         return (dump(output));
      } 
   }
   else {
      fprintf(stderr,"R%d: p.%d - Sections must end with @@@CONCAT 0 instead of: %s\n", _rpt, _pg, _line);
      fprintf(stderr,"    Formatter will print unprocessed text in space remaining on page\n\n");
      return (dump(output));
   }
}


static int fillSkipBlock(     /* creates buffer 'buf' by padding array 'v' */
    char *v[],            /* with blank lines based on skipline value */
    int  last,            /* keeps 'last' lines together (no blanks) */
    char *buf[]           /* returns no. lines in buffer */
)
{
   int  z;
   char **t = buf;  /* mark beginning of buffer */

   z = TXT.n_lines - last;   /* strip away 'last' lines */

   while (z--) {  /* pad buffer with blank lines */
      *buf++ = *v++;
      ++_skipCnt;
      if (_skipCnt == _skip) { 
         _skipCnt = 0;  /* reset skipline counter */
         *buf++ = Null;  /* add blank line */
      }
   }
   while (last--)   /* fill buffer with 'last' lines */
      *buf++ = *v++;

   return(buf - t);    /* return no. lines in buffer */
}


static void printLabel()     /* print label - first line of block array */
{
   if (_plabel == ZERO) {
      fprintf(stderr, "R%d: %s\n%s\n\n", _rpt,
                      "BLOCK Label string is empty",
                      "   Formatter will use default");
      printf("...  (cont)\n");  /* default label */
      ++_lcount;
   }
   else {
      printf("%s  (cont)\n", *_plabel);
      ++_lcount;
   }
}

static void printBlock(    /* print nlines of array 'v' as a block */
    char *v[],             /* keep 'last' lines together on a page */
    int  nlines,
    int  last
)
{
   int  count, i = 0, nl;

   nl = nlines - last;   /* strip away 'last' lines of text */
   
   if (_lcount == 0) {    /* check if starting new page */
      beginPg();
      if (_blockTop && _contLabel)  /* part of block already written */
         printLabel();
   }
   
   if (nlines <= FREELINES)     /* can fit ALL text */
      writeLines(v, nlines);
   else if (nl <= FREELINES) {  /* can fit only nl lines */
      writeLines(v, nl);
      finishPg();                  /* go to new page */
      beginPg();
      if (_contLabel)
         printLabel();
      writeLines(v+nl, last);   /* print 'last' lines */
   }
   else {   /* print out as much per page as possible */
      count = FREELINES;
      while (nl >= count) {
         writeLines(v+i, count);
         nl -= count;       /* calc no. lines left to print out */
         i += count;        /* calc new offset position */
         finishPg();           /* go to new page */
         beginPg();
         if (_contLabel)
            printLabel();
         count = FREELINES; /* number of free lines on current pg */
      }
      if ((count = nl+last) <= FREELINES)
         writeLines(v+i, count);
      else {
         writeLines(v+i, nl);  /*** Test the case  nl=0  ***/
         i += nl;           /* calc new offset position */
         finishPg();
         beginPg();
         if (_contLabel)
            printLabel();
         writeLines(v+i, last);
      }
   }
}

static void printSamePg( /* print nlines of block array v on same page*/
    char *v[],
    int  nlines
)
{
   if (_lcount == 0)     /* check if starting new page */
      beginPg();

   if (nlines <= FREELINES) {   /* sections CAN FIT on current page */
      writeLines(v, nlines);
      if (_lcount == _lmax)    /* no more space left on page */
         finishPg();
   }
   else {        /* sections CAN'T FIT on current page */
      finishPg();
      beginPg();
      writeLines(v, nlines);
   }
}


static void process(   /* carries out routine associated with option */
    char *opt,
    int  val
)
{
   int  c, i = 0, nl, nsec;
   char *array[PAGE];

   switch(decode(opt)) {
   case Block:
        if (val == 1) {      /* save text; print it on same page */
           if (saveLines(array, _width, _pgLen) != ZERO) {
              printSamePg(array, TXT.n_lines);
              freeLines(array, TXT.n_lines);
           }
           else
              fprintf(stderr,"R%d: Block is empty - p.%d\n%s\n\n", _rpt, _pg,
                             "Check location of @@@BLOCK line");
        }
        else if (val > 1) {    /* keep val lines together on the same pg ("blocked") */
           if (saveLines(array, _width, PAGE) != ZERO) {
              if (_contLabel)
                 _plabel = array; /* assign ptr to ptr to first line (label) */
              if (_skip) {    /* add blank lines to array; save in _tmp */
                 nl = fillSkipBlock(array, val, _tmp);
                 if (_blockTop) {
                    printSamePg(_tmp, val); /* "block" TOP val lines */
                    printBlock(_tmp+val, nl - val, val); /* "block" LAST val lines */
                 }
                 else
                    printBlock(_tmp, nl, val);
                 initPtrArray(_tmp, nl);   /* reinitialize _tmp */
              }
              else {      /* print array as a block */
                 if (_blockTop) {
                    printSamePg(array, val); /* "block" TOP val lines */
                    printBlock(array+val, TXT.n_lines - val, val); /* "block" LAST val lines */
                 }
                 else
                    printBlock(array, TXT.n_lines, val);
                 freeLines(array, TXT.n_lines);
              }
           }
           else {
              fprintf(stderr,"R%d: Block is empty - p.%d\n%s\n\n", _rpt, _pg,
                             "Check position of @@@BLOCK line");
              _plabel = ZERO;  /* clear label ptr */
           }
        }
        else if (val == 0)      /* found end of block line */
           fprintf(stderr,"R%d: @@@BLOCK 0 in wrong place - p.%d\n\n", _rpt, _pg);
   break;
   case BlockTop:
        if (val == 1)
           _blockTop = ON;
        else if (val == 0)
           _blockTop = OFF;
        break;
   case Concat:
        if (val > 0) {          /* signal concat routine */
           nsec = val;          /* record no. sections to be processed */
           if ( (c=getchar()) != '@') {
              ungetc(c, stdin);
              if (saveLines(array, _width, _pgLen) != ZERO)
                 i = TXT.n_lines;      /* no. lines saved used as offset */
           }
           else
              ungetc(c, stdin);
           if ((nl = concat(nsec, array+i)) != ZERO) {
              nl += i;
              printSamePg(array, nl);
              freeLines(array, nl);
              freeLines(_tmp, TMP.lines);
           }
           else {
              fprintf(stderr,"R%d: Sections to be concatenated are empty!\n", _rpt);
              fprintf(stderr,"     Check position of @@@SECTION line  p.%d\n\n", _pg);
           }
        }
        else if (val == 0)       /* found end of sections line */
           fprintf(stderr,"R%d: @@@CONCAT 0  in wrong place!  p.%d\n\n", _rpt, _pg);
        break;
   case ContLabel:
        if (val == 1)
           _contLabel = ON;
        else if (val == 0)
           _contLabel = OFF;
        break;
   case Control:
        while ((S_LEN()) >= 0 && (strncmp(_line, "@@@", 3)) !=0)
            printf("%s", _line);
        break;
   case FFoff:
        if (val > 0)       /* user specified last line to print w/out ff */
           _formFeed = val;
        else if (val == 0)
           _formFeed = 0;  /* user turned option off .. formatter adds ff */
        break;
   case Footer:
        if (val == 1) {
           if (saveLines(_footer, _width, _pgLen/2) != ZERO)
             _nlFooter = TXT.n_lines;
           else
             _nlFooter = 0;
        }
        else if (val > 1) {     /* signal concat routine */
           nsec = val;          /* record # sections to be processed */
           if ( (c=getchar()) != '@') {
              ungetc(c, stdin);
              if (saveLines(_footer, _width, _pgLen/2) != ZERO)
                 _nlFooter = TXT.n_lines;
           }
           else
              ungetc(c, stdin);
           if ((nl = concat(nsec, _footer + _nlFooter)) != ZERO) {
              nl += _nlFooter;
              if (saveLines(_footer + nl, _width, _pgLen/2) != ZERO)
                 _nlFooter = TXT.n_lines + nl;
              else
                 _nlFooter = nl;
           }
           else
              _nlFooter = 0;
        }
        else if (val == 0)       /* found end of footer line */
           fprintf(stderr,"R%d: @@@FOOTER 0  in wrong place!\n\n", _rpt);
        break;
   case Header:
        if (val == 1) {
           if (saveLines(_header, _width, _pgLen/2) != ZERO) 
              _nlHeader = TXT.n_lines;
           else {
              _nlHeader = 0;
              _wroteHeader = NO;  /* flag must correspond to empty header */
           }
        }
        else if (val > 1) {      /* signal concat routine */
           nsec = val;           /* record # sections to be processed */
           if ( (c=getchar()) != '@') {
              ungetc(c, stdin);
              if (saveLines(_header, _width,  _pgLen/2) != ZERO)
                 _nlHeader = TXT.n_lines;
           }
              ungetc(c, stdin);
           if ((nl = concat(nsec, _header + _nlHeader)) != ZERO) {
              nl += _nlHeader;
              if (saveLines(_header + nl, _width, _pgLen/2) != ZERO)
                 _nlHeader = TXT.n_lines + nl;
              else
                 _nlHeader = nl;
           }
           else {
              _nlHeader = 0;
              _wroteHeader = NO;
           }
        }
        else if (val == 0)      /* found end of header line */
           fprintf(stderr,"R%d: @@@HEADER 0  in wrong place!\n\n", _rpt);
        break;
   case HeaderNL:
        if (val >= 0)   /* allows use of N > 1 to indicate no. '\n' to print */
           _hdrNL = val;  /* default 1 = ON; 0 = OFF */
        else {
           _hdrNL = ON;  /* reset to default */
           fprintf(stderr,"R%d: %s\n%s\n\n", _rpt, 
                          "@@@HEADERNL option value incorrect", 
                          "Formatter will use default (1)");
        }
        break;
   case MultiTrailer:
        if (val == 1)
           _multiTrailer = ON;
        else if (val == 0)
           _multiTrailer = OFF;
        break;
   case NewPg:
        finishPg(); 
        break;
   case Page:
        _pgLen = val;
        break;
   case PgCnt:
        if (val == 1) { 
           _pgCnt = ON;         /* turn page counting ON */
           _pg = 0;             /* reset page counter */
        }
        else if (val == 0)
           _pgCnt = OFF;        /* turn page counting OFF */
        break;
   case PgFormat:
        if (val == 1) {        /* collect format for pg line */
           if (saveLines(_pgFormat, _width, TOP) == ZERO || _pgFormat[0] == ZERO) {  /* got nothing! */
              fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                             "@@@PGFORMAT missing input line",
                             "    Formatter will use default setting");  /* _pf = Pg  still valid */
           }
           else {   /* got something */
              if (TXT.n_lines > 1) {  /* got more than 1 line! */
                 fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                                "@@@PGFORMAT contains too many input lines",
                                "    Formatter will use the 1st line containing # char");
              }
              if ((i = convertLine(_pgFormat, TXT.n_lines, _symbol, INt)) >= 0)
                 _pf = _pgFormat[i];
              else
                 fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,
                                "@@@PGFORMAT input line missing # char",
                                "    Formatter will use default setting");  /* _pf = Pg  still valid */
           }
        }
        else if (val == 0)   /* found command line in wrong place */
           fprintf(stderr,"R%d: @@@PGFORMAT 0  in wrong place!\n\n", _rpt);
        break;
   case PgLine:
        if (val >= 0)
           _pgLine = val;       /* get location of pg line */
        else
           fprintf(stderr,"R%d: @@@PGLINE value incorrect \n\n", _rpt);
               /* formatter will use default (_pgLine = ZERO) */
        break;
   case Report:
        if (val == 1) {          /* beginning of report */
           _inReport = YES;
           ++_rpt;               /* increment report counter */
           _wroteHeader = NO;
           _haveTrailer = NO;
           _pg = 0;              /* reset counters */
           _lcount = 0;
           if (_lmax == 0)  
              _lmax = _topMarg + _pgLen;
        }
        else if (val == 0) {    /*** End of Report ***/
           if (_lcount > 0)  /* if current page has text */
              finishPg();    /* clean up and go to new page */

           _inReport = NO;   /* clear inputs/reset defaults */
           _hdrNL = ON;
           _blockTop = OFF;
           _contLabel = OFF;
           _plabel = ZERO;   /* clear ptr to Label string */
           freeLines(_header, _nlHeader);
           freeLines(_trailer, _nlTrailer);
           _nlHeader = 0;
           _nlFooter = 0;
           _nlTrailer = 0;
           _lmax = 0;
           _secWidth = 0;
           _pf = Pg;         /* assign pter to default string Pg[] */

           if (_wideReport == ON) {       /* current report was "wide" */
              printf("\n@@@WIDEREPORT 0\n");  /* mark end of wide report */
              _wideReport = OFF;              /* turn OFF wide report option */
              _pgCnt = _pgCntOrig;        /* reset original value (wide report turns it off) */
           }
        }
        break;
   case SecWidth:
        if (val > 0)
           _secWidth = val;       /* set section width */
        else if (val == 0)
           _secWidth = 0;         /* cancel specified section width */
        break;
   case Skip:
        if (val > 0)     
           _skip = val;
        else if (val == 0)       /* cancel skip line option */
           _skip = _skipCnt = 0;  /* reset skipline counter */
        break;
   case Top:
        _topMarg = val;
        break;
   case Trailer:
        if (val == 1) {
           if (saveLines(_trailer, _width, _pgLen/2) != ZERO)
             _nlTrailer = TXT.n_lines;
           else
             _nlTrailer = 0;
        }
        else if (val > 1) {     /* signal concat routine */
           nsec = val;          /* record # sections to be processed */
           if ( (c=getchar()) != '@') {
              ungetc(c, stdin);
              if (saveLines(_trailer, _width, _pgLen/2) != ZERO)
                 _nlTrailer = TXT.n_lines;
           }
           else
              ungetc(c, stdin);
           if ((nl = concat(nsec, _trailer + _nlTrailer)) != ZERO) {
              nl += _nlTrailer;
              if (saveLines(_trailer + nl, _width, _pgLen/2) != ZERO)
                 _nlTrailer = TXT.n_lines + nl;
              else
                 _nlTrailer = nl;
           }
           else
              _nlTrailer = 0;
        }
        else if (val == 0)       /* found end of trailer line */
           fprintf(stderr,"R%d: @@@TRAILER 0  in wrong place!\n\n", _rpt);
        break;
   case NotKnown:     
        fprintf(stderr,"R%d: Unknown codeword used: %s\n", _rpt, _option);
        fprintf(stderr,"     Formatter will disregard it and continue\n\n");
        break;       
   case WideReport:
        _wideReport = val;
        if (_wideReport == ON) {
            printf("@@@WIDEREPORT 1\n"); /* mark beg. of wide report in output */
            while ((S_LEN()) >= 0 && (strncmp(_line, "@@@WIDE", 7)) !=0) {
                printf("%s\n", _line);              /* print the option line */
                if (strncmp(_line, "@@@PGFORMAT", 11) == 0)  /* and check if */
                   _widePgFmt = YES;                         /* special case */
                if (strncmp(_line, "@@@PGLINE", 9) == 0)
                   _widePgLine = YES;
            }
            _pgCntOrig = _pgCnt;   /* preserve original setting */
            _pgCnt = OFF;          /* and turn counting off for wide report */
            /****  Deal with special case ****/
            if (_widePgLine == NO)  /* use formatter's pgline location */
               printf("@@@PGLINE %d\n", _pgLine);
            if (_widePgFmt == NO && _rawp != ZERO)  /* use formatter's pgformat line */
               printf("%s\n%s\n%s\n", "@@@PGFORMAT 1", _rawp, "@@@PGFORMAT 0" );
        }
        break;
   case Width:
        _width = val;            /* used in concat routine */
        break;
   default:
        break;
   }
}


/************************************************************************
 **************               MAIN   ROUTINE               **************
 ************************************************************************/

int main (int, char*[]) {
   int  freespace, space, value;

    /*   INITIALIZE ALL LINE POINTER ARRAYS   */

   initPtrArray(_footer, PAGE);
   initPtrArray(_header, PAGE);
   initPtrArray(_trailer, PAGE);
   initPtrArray(_pgFormat, TOP);
   initPtrArray(_tmp, MAXSPC);

    /*   BEGIN FORMATTING INPUT FILE   */ 

   while (S_LEN() >= 0) {

               /* outside of report */
      if (_inReport == NO) {          
         if (strcmp(_line, "") == 0)
            continue;               /* skip blank line */
         else if (strncmp(_line,"#",1) == 0)
            continue;               /* skip VISION comment */
         else if (strncmp(_line,"@@@",3) == 0) {  
            sscanf(_line, "%*[@@@]%s %d\n", _option, &value); 
            process(_option, value);
         }
      }
               /* within a report */
      else if (_inReport == YES) {    
         if (strncmp(_line,"#",1) == 0)
            continue;                     /* skip VISION comment */
         else if (strncmp(_line,"@@@",3) == 0) { 
            sscanf(_line, "%*[@@@]%s %d\n", _option, &value);
            process(_option, value);
         }
         if (_nlFooter) {             /* have footer */
            space = _nlHeader + _nlFooter;
            if (space >= _pgLen) {
               fprintf(stderr,"R%d: %s\n%s\n%s\n\n", _rpt,
                              "Footer can't fit on same page as header",
                              "   Formatter will compensate this time",
                              "   Check @@@PAGELEN value; try adjusting it to accommodate text");
               endPg();
               writeTopMargin();
               writeFooter();
            }
            else {
               freespace = _lmax - (_nlFooter+2);
               if (_lcount <= freespace)  
                  writeFooter();           /* advances to new page */
               else {      
                  endPg();
                  beginPg();
                  writeFooter();           /* advances to new page */
               }
            }
         }
         else if (_lcount == 0 && _inReport == YES)   /* new page */
            beginPg();

         if (_lcount < _lmax && _inReport == YES) {
           /** Needed to correctly process first page of report **/
            if (_wroteHeader == NO && _nlHeader != 0) {
               writeHeader();    /* _lcount updated by routine */
              _wroteHeader = YES;
            }
            if (_haveTrailer == NO && _nlTrailer != 0)
               processTrailer();    /* recalc _lmax */
           /** ------------------------------------------------ **/

            if (strncmp(_line,"@@@",3) != 0 && strncmp(_line,"#",1) != 0) {
               printf("%s\n", _line);     /* print only text lines */
               ++_lcount;
               if (_skip > 0)            /* have skip line option */
                  ++_skipCnt;
               else                      /* cancel skip line option */
                  _skipCnt = 0;
 
               if (_skipCnt == _skip && _skip != 0) {
                  _skipCnt = 0;               /* reset skipline counter */
                  printf("\n");
                  if (_lcount < _lmax)
                     ++_lcount;
               }
               if (_lcount == _lmax)
                  finishPg();         /* go to new page */
            }
         }
         else if (_lcount == _lmax && _inReport == YES)  /* clear _lcount */
            finishPg();
      }
   }
   return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  format.c 1 replace /users/lhk/projects/format
  870327 17:20:36 lhk Create formatter

 ************************************************************************/
/************************************************************************
  format.c 2 replace /users/lhk/format
  880226 14:11:47 lhk lmax calculated for EACH report

 ************************************************************************/
/************************************************************************
  format.c 3 replace /users/lhk/format
  880929 15:35:12 lhk Added more options; improved memory management

 ************************************************************************/
/************************************************************************
  format.c 4 replace /users/lhk/format
  881118 16:15:29 lhk Bug fix: NEW header appears on correct page

 ************************************************************************/

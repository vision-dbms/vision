/*----------------------------------------------------------
 *   COMPILE USING FOLLOWING OPTIONS:
 *   HP 9000:   cc paginate.c -Wc,-Ni400 -o paginate
 *    Apollo:   cc paginate.c -O -o paginate
 *----------------------------------------------------------
*/

/*-----------------------
 * Compilation Notes:
 *  HP:  increase internal compiler table size (default = 300)
 *       due to increased number of options in CODEWORD decode(s)
 *
 *  Apollo:  executable created for machine type where compilation was run
 *           - to create DN10000 executable from DN4000, eg, use options:
 *             cc paginate.c -O -A cpu,a88k -o paginate
 *-----------------------
 *  Other:
 *  Input file need not end with '\n' for loop to end
 *-----------------------
*/


#include "Vk.h"

#define    ZERO     0

#define    YES      1
#define    NO       0

#define    ON       1
#define    OFF      0

#define    INT      0
#define    CHR      1
#define    STR      2
#define    DBL      3
#define    EXP      4

#define    LENGTH  20000   /* allocated length of _line[] */
#define    LINES      10   /* max no. lines collected in _pgFormat[] */
#define    MAXSPC      1000   /* generic "max space allocated" value */
#define    PG        200   /* max no. lines/page */
#define    TABS       30   /* max no. tabstops per wide report page */

#define    NL         56   /* default value: no. lines printed/std page */
#define    WIDTH      80   /* default value: no. chars/line/std page */




typedef enum {Horizontal, LeftLabel, PgCntSeq, PgFormat, PgLine, SecWidth,
              Tabs, Title, Unknown, Vertical, WideReport, Width} CODEWORD;




           /********   STRUCTURE DEFINITIONS   *********/

struct section {  /* profile of section of saved text */
    int  n_lines;   /* no. lines in section */
    int  max_len;   /* max string length of section */
    int  sub_pg;    /* no. sub-pages in section */
    char **ptr;     /* (pter to pter) first line of section */
} TXT,       /* used for _pgFormat[] */
  PAGE[PG];  /* used for sections of _buf[] */

struct collection {  /* _buf[] profile (collected slices) */
    int  max_sp;     /* max no. of subpages in buffer */
    int  lines;      /* total no. lines in buffer */
    int  ctr;        /* incr count each time slice processed */
    char **ptr;     /* pts to beginning of _buf[] (not sure if needed) */
} BUF;



        /*********   GLOBAL VARIABLE DEFINITIONS   *********/


int    _lCol = 0;              /* (input) no. chars in left label */
int    _title = 0;             /* (input) no. lines per title */
int    _pgLine = 2;            /* (input) DEFAULT location of pg line */
int    _secWidth = 0;          /* (input) default tabstop: _width - _lCol */
int    _sequentialPgCnt = ON;  /* (input) sequential pg counting enabled */
int    _tabs = 0;              /* (input) no. tabstops per wide page */
int    _verticalSort = OFF;    /* (input) user can turn vertical sort ON */
int    _width = WIDTH;         /* (input) max no. chars printed/line/page */


int    _inReport = NO;      /* YES if within a report */


int    _pg = 0;             /* page counter */
int    _rpt = 0;            /* report counter for error messages */

int    _lcnt = 0;           /* line counter for current "slice" */
int    _maxlen = 0;         /* max line length in current "slice" */
int    _slice = 0;          /* current "slice" number (index into PAGE[]) */

int    _symbol = '#';       /* token indicating position of pg no. in pgline */

char   _line[LENGTH];       /* holds input string */
char   _option[15];         /* option codeword: 14 chars + \0 */
char   _temp[WIDTH];        /* temporary char array */

int    _tabStop[TABS];     /* holds user-supplied tab stops */

char  errorI[] = "*** ERROR: TOO MANY LINES ***";
char  Null[]   = "";           /* null string used for adding blank lines */
char  Pg[]     = "  Page %d";  /* default format string for pg line */
char  _type[]  = { 'd',     /* decimal integer */
                   'c',     /* character */
                   's',     /* string */
                   'f',     /* double - decimal */
                   'e' };   /* double - exponential */



char   *_pgFormat[LINES];   /* lineptr array of format strings for pg line */
char   *_buf[MAXSPC];          /* lineptr array of collected "slices" */
char   *_pf = Pg;           /* pter to pg line string (default set) */

char  **_bufp = _buf;     /*  ptr to ptr (used as offset into _buf[])   */


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
                  "... PROCESS ABORTED ...");
   exit(ErrorExitValue);  /* abort main() */
}

static CODEWORD decode(       /* decodes option associated with string */
    char *s                 /* returns corresponding codeword */
)
{
   return
      (strncmp (s, "HORIZONTAL",  10) == 0) ? Horizontal :
      (strncmp (s, "LLABEL",       6) == 0) ? LeftLabel :
      (strncmp (s, "PGCNTSEQ",     8) == 0) ? PgCntSeq :
      (strncmp (s, "PGFORMAT",     8) == 0) ? PgFormat :
      (strncmp (s, "PGLINE",       6) == 0) ? PgLine :
      (strncmp (s, "SECWIDTH",     8) == 0) ? SecWidth :
      (strncmp (s, "TABS",         4) == 0) ? Tabs :
      (strncmp (s, "TITLE",        5) == 0) ? Title :
      (strncmp (s, "VERTICAL",     8) == 0) ? Vertical :
      (strncmp (s, "WIDEREPORT",  10) == 0) ? WideReport :
      (strncmp (s, "WIDTH",        5) == 0) ? Width :
      Unknown;
}


static char *insertStr(
    char *s,         /* string which requires an insertion */
    int  slen,       /* length of string s, excluding terminating '\0' char */
    char *x,         /* ptr to location within s where insertion belongs */
    int  conv_arg    /* arg specifies what conversion type should be inserted */
)
{
    int   i;
    char *t;
    char *p;  /* ptr to newly converted line */
    
    if ((t = (char*)malloc((slen+2)*sizeof(char))) == ZERO)
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
       return (p);   /* return address of converted line t */
    }
    return s;
}



static int convertLine(
    char *v[],    /* lineptr array containing line needing conversion */
    int  nl,      /* total no. lines in v[] */
    int  token,   /* symbol in string indicating location of insertion */
    int  type     /* conversion specification (eg. INT, CHR, STR)*/
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
       s = insertStr(v[index], len, p, type);   /* get new converted line */
       free(v[index]);   /* free space associated w/ orig. line */
       v[index] = s;     /* assign ptr to reference new line */
       return(index);    /* conversion successful */
    }
    else     /* lineptr array has no line containing token */
       return(-1);       /* conversion unsuccessful */
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


static void initStructBUF()             /* initialize BUF structure */
{
    BUF.max_sp = 0;
    BUF.lines = 0;
    BUF.ctr = 0;
    BUF.ptr = _buf;       /* not used anywhere since _buf is global */
}


static int addLine(   /* add str (length len) to lineptr array at v[idx] */
    char *s,                      /* return 1 if line added; 0 if error */
    int len,
    char *v[],
    int idx
)
{
    char *t;

    if (idx >= MAXSPC)     /* exceeds bounds of _buf[]  */
       return (0);

    if (v[idx] != ZERO)   /* clean up if current idx isn't null ptr */
       free(v[idx]);

    if ((t = (char*)malloc((len+1)*sizeof(char))) == ZERO)
       abortMain();

    v[idx] = strcpy(t, s);   /* add copied string t to lineptr array */
    return (1);
    
}



static int collectDigits(   /** convert input chars to digits; store in dig[] **/
    int dig[],                       /** return number of digits collected **/
    int offset
)
{
    int   i = 0, j = offset;
    int   c;

    while ((c = getc(stdin)) != '\0') {
       if (isdigit(c))   /* got digit char */
          _temp[i++] = c;
       else if (c == ' ') {
          if (i) {  /* separator between digits (not a leading space) */
             _temp[i] = '\0';
             dig[j++] = atoi(_temp);
             i = 0;
          }
       }
    }  /* end while() */   /* should I clear _temp? (initialize to '\0') */
    
    /* get last char of current line */
    if ((c = getc(stdin)) != '\n') {  /* unexpected error */
       fprintf(stderr, "R%d: %s\n%s\n\n", _rpt, 
                 "Cannot recover from error in collectDigit()", 
                 "... PROCESS ABORTED ..." );
       exit(ErrorExitValue); /* aborts main .. maybe too severe?? */
    }
    else
       return (j - offset);
}

static int printTitle(   /* print _title lines of PAGE[num]; return lcnt */
    int num
)
{
    int lcnt;  /* number of lines printed */
    char **bp = PAGE[num].ptr;  /* initialize ptr to top of page */

    for (lcnt=0; lcnt < _title; lcnt++) {  /** print TITLE lines **/
       if (_pgLine == lcnt + 1) {
          printf(_pf, _pg);
          printf("\n");
          bp++;
       }
       else
          printf("%-.*s\n", _width, *bp++);
    }
    return (lcnt);
}

static void printSubPage(
    int num,        /* indicates which PAGE[] structure to process */
    int cnt,        /* line counter; points to next line to print (after title) */
    int tab,        /* subpage width */
    int offset      /* indicates position in string where subpage begins */
)
{
    int len;
    char **bp;

    for (bp = PAGE[num].ptr + _title; cnt < PAGE[num].n_lines; cnt++) {
       len = strlen(*bp);
       printf("%-*.*s", _lCol, _lCol, *bp);
       if (_lCol >= len) {
          printf("\n");  /* line contained fewer chars than left label */
          bp++;
       }
       else if (len - _lCol < tab)  /* chars extend beyond _lCol but not full tab amount */
          printf("%s\n", *bp++ + _lCol);  /* assume string is Label (no data) */
       else if (offset > len) {     /* beyond current line length */
          printf("\n");
          bp++;
       }
       else
          printf("%-.*s\n", tab, *bp++ + offset);

    }
    printf("\f");   /* finish current subpage */

}


static void printPage(    /* print PAGE[num]   .. for HORIZONTAL case */
    int  num
)
{
   int   i, j = 0, lcnt, tab;
   int   offset = _lCol;
   int   sp = PAGE[num].sub_pg;

   if (_tabs == 0)   /* constant tabstop */
      tab = _secWidth;
   else
      tab = _tabStop[j];   /* j=0 at this point */
      
   if (_sequentialPgCnt == OFF)
      _pg++;    /* done only once per "slice" */

   for (i=0; i < sp; i++) {  /**  for EACH subpage  **/
      if (_sequentialPgCnt == ON)
         _pg++;
      lcnt = printTitle(num);
      printSubPage(num, lcnt, tab, offset);
      offset += tab;
      if (_tabs) { /* have TABS set */
         if (j < _tabs)
            tab = _tabStop[++j];   /* get next tabstop */
         else
            tab = _width - _lCol;  /* set to default for remaining subpages */
      }
   }
}

static void printBuffer()
{
    int idx, j = 0, lcnt, tab;
    int offset = _lCol;

   if (_tabs == 0)   /* constant tabstop */
      tab = _secWidth;
   else
      tab = _tabStop[j];   /* j=0 at this point */

      /**  For each PAGE, process current subpage **/
   for (; j < BUF.max_sp; j++) {  /* subpage */
      for (idx = 0; idx < BUF.ctr; idx++) {
         _pg++;
         lcnt = printTitle(idx);
         printSubPage(idx, lcnt, tab, offset);
      }
      offset += tab;
      if (j < _tabs)
         tab = _tabStop[++j];   /* get next tabstop */
      else
         tab = _width - _lCol;  /* set to default for remaining subpages */
   }

}


static char **processPage(  /* assign PAGE struct members */
    int num,                               /* return next ptr into _buf[] */
    int cnt,
    char **pp,
    int max
)
{
    int  sp, t;
    
    PAGE[num].n_lines = cnt;
    PAGE[num].max_len = max;
    PAGE[num].ptr = pp;

    /* calculate number of sub-pages in current wide page ("slice") */

    if (_tabs == 0) {  /* use _secWidth as constant tabstop */
       if (_secWidth == 0)  /* calc new constant tabstop */
          _secWidth = _width - _lCol;
       sp = (max - _lCol)/(_secWidth);
       if (sp <= 0)
          sp = 1;
       else if (((max - _lCol) % _secWidth) > 0 )
          sp++;
    }
    else {  /* use _tabStop[] to calc sub-pages */
       if ((max = max - _lCol) >= 0) {
          sp = 1;
          for (t=0; t < _tabs; t++) {
             max = max - _tabStop[t];
             if (max > 0)
                sp++;
          }
          if (max > _width) { /* have more than 1 additional sub-page */
             sp++;
             fprintf(stderr, "R%d: %s\n%s\n\n", _rpt, 
                      "Have more sub-pages than TABS value indicates", 
                      "Paginator will truncate data beyond last expected page");
            /*NOTE: pr page routine should only print max _width chars
                    if on last subpage (automatically truncating possibly
                    longer lines, as explained in msg) */
          }
       }
       else {   /* _lCol > max; error in user input */
          fprintf(stderr, "R%d: %s\n%s\n\n", _rpt, 
                     "LLABEL input incorrect; exceeds max line length of report", 
                     "Paginator will assume default value to continue");
          _lCol = 0;
          _tabs = 0;
          sp = 1;
          if (_secWidth == 0)
              _secWidth = _width;
       }
    }
    PAGE[num].sub_pg = sp;

    if (pp - _buf + cnt >= MAXSPC)  /* next ptr exceeds bounds of _buf[] */
       return (ZERO);
    else
       return (pp + cnt);  /* next offset into _buf[] */

}

static int saveLines(   /* saves lines delimited by @@@ in v[] */
    char *v[],                            /* return signal: 1 or ZERO */
    int  maxstrlen, 
    int  maxlines
)
{
   int   err = 1, flag, i, len, max = 0;
   char *s;
   char **t = v;           /* record start of array */
    
   while ((len = S_LEN()) >= 0 && (flag = strncmp(_line,"@@@",3)) != 0) {
      if (len > maxstrlen) {
         if (err == 1) {
            fprintf(stderr,"R%d: %s\n%s\n\n", _rpt, 
                           "Input line too long",
                           "Paginator will truncate line with ***");
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
         fprintf(stderr,"R%d: Section has too many lines\n\n", _rpt);
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
      if (v == t)    /* both pt to same location */
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


static void bufferOverflow()   /* NOTE:   if pass _slice as arg to fnc, then it may not
                      need to be global .. but there should be a global
                      sliceCnt var (unless BUF maintains it since only it
                      needs to use it) */
{
   fprintf(stderr, "R%d: %s\n%s\n\n", _rpt, 
             "Report length exceeds bounds of _buf[]", 
             "... Dumping output and ABORTING ...");
   if (_verticalSort == ON)
      printBuffer();
   else
      printPage(_slice);

   exit(ErrorExitValue);   /* abort main() */
}


static void process(   /* carries out routine associated with option */
    char *opt,
    int  val
)
{
   int  cnt, idx;

   switch(decode(opt)) {

   case Horizontal:
        if (val > 0)                 /* turn horizontal sort ON (default) */
           _verticalSort = OFF;      /* by turning vertical sort OFF */
        else
           _verticalSort = ON;      /* redundant .. same as @@@VERTICAL 1 */
        break;
   case LeftLabel:
        if (val > 0)       /* default set to 0; user shouldn't need to do so */
           _lCol = val;
        break;
   case PgCntSeq:
        if (val > 0)         /* expect user input: val = 1 (default) */
           _sequentialPgCnt = ON;  /* no need to test if >1 unless it's a separate case */
        else
           _sequentialPgCnt = OFF; /* expect user input: val = 0 */
        break;
   case PgFormat:   /** needs work **/
        if (val == 1) {        /* collect format for pg line */
           if (saveLines(_pgFormat, _width, LINES) == ZERO || _pgFormat[0] == ZERO) {  /* got nothing! */
              fprintf(stderr,"R%d: %s\n%s\n\n", _rpt,          /* _pf = Pg  still valid */
                             "@@@PGFORMAT missing input line",
                             "Paginator will use default setting");
           }
           else {   /* got something */

              if ((cnt = convertLine(_pgFormat, TXT.n_lines, _symbol, INT)) >= 0)
                 _pf = _pgFormat[cnt];
              else {
                 fprintf(stderr,"R%d: @@@PGFORMAT input line missing # char\n", _rpt);
                 fprintf(stderr,"     Formatter will use default setting\n\n");  /* _pf = Pg  still valid */
              }
           }
        }
        else if (val == 0)   /* found command line in wrong place */
           fprintf(stderr,"R%d: @@@PGFORMAT 0  in wrong place!\n\n", _rpt);
        break;
   case PgLine:
        if (val >= 0)
           _pgLine = val;       /* get location of pg line */
        break;                  /* test elsewhere if within title */
   case SecWidth:
        if (val > 0)
           _secWidth = val;   /* width of text body per subpage */
        else {
           fprintf(stderr, "R%d: %s\n%s\n\n", _rpt,
                           "Invalid SECWIDTH value",
                           "Paginator will recover using defaults" );
           _secWidth = _width - _lCol;  /* not sure if needed here .. may not
                                           have _lCol yet .. I think I calc it
                                           somewhere in processPage routine */
        }
        break;
   case Tabs:   /***  needs work .. convert string & assign tabstops ***/
        if (val > 0) {
           _tabs = val;       /* set no. of tabs expected */
           idx = 0;    /* initialize index into _tabStop[] */ 
           if ((cnt = collectDigits(_tabStop, idx)) != _tabs) {
              fprintf(stderr, "R%d: %s%d%s%d\n%s\n\n", _rpt, 
                        "Expected ", _tabs, " tabs, but collected ", cnt,
                        "Will process report using default column width");
              _tabs = 0;
           }
       /*** "main" while() gets next line; expect "@@@TABS 0" .. no-op ***/
       /*** else, if another cmd line, will get processed as usual ***/
        }
        break;
   case Title:
        if (val > 0)
          _title = val;
        break;

   case Unknown:     
        fprintf(stderr,"R%d: %s%s\n%s\n\n", _rpt,
                 "Unknown codeword used: ", _option,
                 "Paginator will disregard it and continue");
        break;       
   case Vertical:
        if (val > 0)     
           _verticalSort = ON;
        else {        /* cancel vertical sort option */
           _verticalSort = OFF;
        }
        break;
   case WideReport:
        if (val == 1) {          /* beginning of wide report */
           _inReport = YES;
           ++_rpt;               /* increment report counter */
           _pg = 0;              /* reset counters */
           _lcnt = 0;
        }
        else if (val == 0) {    /* end of report; clear input vars */
           if (_verticalSort == ON)
              printBuffer();
           _inReport = NO;
           _title = 0;
           _secWidth = 0;
           _pf = Pg;            /* assign pter to default string Pg[] */
           freeLines(_buf, BUF.lines);  /* Re-init BUF >>after<< free current */
           initStructBUF();
           _bufp = _buf;        /* ptr reset to top of _buf[] */
           _lcnt = 0;
           _maxlen = 0;
           _slice = 0;        /** vars not cleared will be used in next report
                                  unless explicitly changed by user **/
        }
        break;
   case Width:
        _width = val;  /* page width after report split into subpages */
        break;   /* also width of title lines, which may be > section width */
   default:
        break;
   }
}


/************************************************************************
 **************               MAIN   ROUTINE               **************
 ************************************************************************/
           /*** NOTE:  _lcnt should probably be local to main **/
int main (int, char*[]) {
   int  c, len, value;
   char **nextp;

    /*   INITIALIZE ALL LINE POINTER ARRAYS   */

   initPtrArray(_pgFormat, LINES);
   initPtrArray(_buf, MAXSPC);

    /*   INITIALIZE BUFFER STRUCTURE   */

   initStructBUF();



    /*   BEGIN PAGINATING INPUT FILE   */ 

   while ((len = S_LEN()) >= 0) {

               /* outside of report */
      if (_inReport == NO) {          
         if (strncmp(_line,"@@@",3) == 0) {  
            sscanf(_line, "%*[@@@]%s %d\n", _option, &value); 
            process(_option, value);
         }  /* NOTE: option line may begin with formfeed */
         else if (strncmp(_line,"\f@@@",4) == 0) {
            sscanf(_line, "%*[\f@@@]%s %d\n", _option, &value); 
            process(_option, value);
            printf("\f");  /* add \f which got "lost" by option processing */
         }
         else   /* "push" _line through to stdout without processing */
            printf("%s\n", _line);   /* Rem: gets() removes '\n' from _line */
      }
               /* within a report */
      else if (_inReport == YES) {    
         if (strncmp(_line,"#",1) == 0)
                   continue;               /* skip VISION comment */
         else if (strncmp(_line,"@@@",3) == 0) { 
            sscanf(_line, "%*[@@@]%s %d\n", _option, &value);
            process(_option, value);
         }
         else if (strncmp(_line,"\f",1) == 0) {  /* defines current "slice" */
            if ((nextp = processPage(_slice, _lcnt, _bufp, _maxlen)) == ZERO)
               bufferOverflow(); /* exceeded bounds of _buf[]; abort main() */
            if (_verticalSort == OFF) {  /*** HORIZONTAL case ***/
               printPage(_slice);
               freeLines(_buf, _lcnt);  /* clear lines just processed */
               _lcnt = 0;     /* clear line count (just processed) */
               _bufp = _buf;  /* reinit buf ptr to top of _buf */
               _line[0] = ' ';  /* replace \f with space in current _line */
               len = strlen(_line);
               addLine(_line, len, _buf, _lcnt);  /* add it to input stream */
               _lcnt++;
            }
            else {  /*** VERTICAL case *** add current slice to BUF */
               if (BUF.max_sp < PAGE[_slice].sub_pg)
                  BUF.max_sp = PAGE[_slice].sub_pg;
               BUF.lines += _lcnt;
               BUF.ctr++;
               _bufp = nextp;
               _slice++;
               _lcnt = 0;
               _line[0] = ' ';  /* replace \f with space in current _line */
               len = strlen(_line);
               addLine(_line, len, _bufp, _lcnt);  /* add it to input stream */
               _lcnt++;
            }
         }
         else {
            if ((c = addLine(_line, len, _bufp, _lcnt))) {
               if (_maxlen < len)
                  _maxlen = len;
               _lcnt++;
            }
            else {   /* exceeded bounds of buffer */
               bufferOverflow();
            }
         }
      } /* end  _inReport = YES */
   }  /*  end while()  */

   return NormalExitValue;
}   /*  end main()  */

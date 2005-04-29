/*
    str_ops.c:

    Copyright (C) 2005 Istvan Varga, Matt J. Ingalls, John ffitch

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include "csdl.h"
#define CSOUND_STR_OPS_C    1
#include "str_ops.h"
#include <ctype.h>

#define STRSMAX 8

#define RNDINT(x)   ((long) ((double) (x) + ((double) (x) >= 0.0 ? 0.5 : -0.5)))

#ifndef HAVE_SNPRINTF
/* add any system that has snprintf() */
#if defined(HAVE_C99) || defined(HAVE_GCC3)
#define HAVE_SNPRINTF   1
#endif
#endif

/* strset by John ffitch */

static void str_set(ENVIRON *csound, int ndx, const char *s)
{
    if (csound->strsets == NULL) {
      csound->strsmax = STRSMAX;
      csound->strsets = (char **) csound->Calloc(csound, (csound->strsmax + 1)
                                                         * sizeof(char*));
    }
    if (ndx > (int) csound->strsmax) {
      int   i, newmax;
      /* assumes power of two STRSMAX */
      newmax = (ndx | (STRSMAX - 1)) + 1;
      csound->strsets = (char**) csound->ReAlloc(csound, csound->strsets,
                                                 (newmax + 1) * sizeof(char*));
      for (i = (csound->strsmax + 1); i <= newmax; i++)
        csound->strsets[i] = NULL;
      csound->strsmax = newmax;
    }
    if (ndx < 0)    /* -ve index */
      csound->Die(csound, Str("illegal strset index"));

    if (csound->strsets[ndx] != NULL) {
      if (strcmp(s, csound->strsets[ndx]) == 0)
        return;
      if (csound->oparms->msglevel & WARNMSG) {
        csound->Warning(csound, Str("strset index conflict at %d"), ndx);
        csound->Warning(csound, Str("previous value: '%s', replaced with '%s'"),
                                csound->strsets[ndx], s);
      }
      csound->Free(csound, csound->strsets[ndx]);
    }
    csound->strsets[ndx] = (char*) csound->Malloc(csound, strlen(s) + 1);
    strcpy(csound->strsets[ndx], s);
    if ((csound->oparms->msglevel & 7) == 7)
      csound->Message(csound, "Strsets[%d]: '%s'\n", ndx, s);
}

int strset_init(ENVIRON *csound, STRSET_OP *p)
{
    str_set(csound, (int) RNDINT(*p->indx), (char*) p->str);
    return OK;
}

/* for argdecode.c */

void strset_option(ENVIRON *csound, char *s)
{
    int indx = 0;

    if (!isdigit(*s))
      csound->Die(csound, Str("--strset: invalid format"));
    do {
      indx = (indx * 10) + (int) (*s++ - '0');
    } while (isdigit(*s));
    if (*s++ != '=')
      csound->Die(csound, Str("--strset: invalid format"));
    str_set(csound, indx, s);
}

int strget_init(ENVIRON *csound, STRGET_OP *p)
{
    int   indx;

    ((char*) p->r)[0] = '\0';
    if (*(p->indx) == SSTRCOD) {
      if (csound->currevent->strarg == NULL)
        return OK;
      if ((int) strlen(csound->currevent->strarg) >= csound->strVarMaxLen)
        return csound->InitError(csound, Str("strget: buffer overflow"));
      strcpy((char*) p->r, csound->currevent->strarg);
      return OK;
    }
    indx = (int) ((double) *(p->indx) + (*(p->indx) >= FL(0.0) ? 0.5 : -0.5));
    if (indx < 0 || indx > (int) csound->strsmax ||
        csound->strsets == NULL || csound->strsets[indx] == NULL)
      return OK;
    if ((int) strlen(csound->strsets[indx]) >= csound->strVarMaxLen)
      return csound->InitError(csound, Str("strget: buffer overflow"));
    strcpy((char*) p->r, csound->strsets[indx]);
    return OK;
}

/* strcpy */

int strcpy_opcode_init(ENVIRON *csound, STRCPY_OP *p)
{
    char  *newVal = (char*) p->str;

    if (p->r == p->str)
      return OK;
    if ((int) strlen(newVal) >= csound->strVarMaxLen)
      return csound->InitError(csound, Str("strcpy: buffer overflow"));
    strcpy((char*) p->r, newVal);
    return OK;
}

int strcpy_opcode_perf(ENVIRON *csound, STRCPY_OP *p)
{
    char  *newVal = (char*) p->str;

    if (p->r == p->str)
      return OK;
    if ((int) strlen(newVal) >= csound->strVarMaxLen)
      return csound->PerfError(csound, Str("strcpy: buffer overflow"));
    strcpy((char*) p->r, newVal);
    return OK;
}

/* strcat */

int strcat_opcode_init(ENVIRON *csound, STRCAT_OP *p)
{
    char  *newVal1 = (char*) p->str1;
    char  *newVal2 = (char*) p->str2;

    if ((int) (strlen(newVal1) + strlen(newVal2)) >= csound->strVarMaxLen)
      return csound->InitError(csound, Str("strcat: buffer overflow"));
    if (p->r != p->str2) {
      if (p->r != p->str1)
        strcpy((char*) p->r, newVal1);
      strcat((char*) p->r, newVal2);
      return OK;
    }
    if (newVal1[0] == '\0')
      return OK;
    memmove(newVal2 + strlen(newVal1), newVal2, strlen(newVal2) + 1);
    if (p->r != p->str1)
      memcpy(newVal2, newVal1, strlen(newVal1));
    return OK;
}

int strcat_opcode_perf(ENVIRON *csound, STRCAT_OP *p)
{
    char  *newVal1 = (char*) p->str1;
    char  *newVal2 = (char*) p->str2;

    if ((int) (strlen(newVal1) + strlen(newVal2)) >= csound->strVarMaxLen)
      return csound->PerfError(csound, Str("strcat: buffer overflow"));
    if (p->r != p->str2) {
      if (p->r != p->str1)
        strcpy((char*) p->r, newVal1);
      strcat((char*) p->r, newVal2);
      return OK;
    }
    if (newVal1[0] == '\0')
      return OK;
    memmove(newVal2 + strlen(newVal1), newVal2, strlen(newVal2) + 1);
    if (p->r != p->str1)
      memcpy(newVal2, newVal1, strlen(newVal1));
    return OK;
}

/* strcmp */

int strcmp_opcode(ENVIRON *csound, STRCAT_OP *p)
{
    int i;

    *(p->r) = FL(0.0);
    if (p->str1 == p->str2)
      return OK;
    i = strcmp((char*) p->str1, (char*) p->str2);
    if (i < 0)
      *(p->r) = FL(-1.0);
    else if (i > 0)
      *(p->r) = FL(1.0);
    return OK;
}

/* perform a sprintf-style format -- based on code by Matt J. Ingalls */

static int sprintf_opcode(ENVIRON *csound, SPRINTF_OP *p)
{
    int     len = 0, maxLen = csound->strVarMaxLen;
    int     numVals = (int) p->INOCOUNT - 1, strCode = p->XSTRCODE >> 1;
    char    strseg[2048], *outstring = (char*) p->r, *fmt = (char*) p->sfmt;
    MYFLT   **kvals = p->args, *pp = NULL;
    int     i = 0, j = 0, n;
    char    *segwaiting = NULL;
    int     maxChars;

    if (p->XINCODE) {
      sprintf(csound->errmsg, Str("sprintf: a-rate argument not allowed"));
      return -1;
    }
    if (numVals > 30) {
      csound->Die(csound, Str("sprintf: too many arguments"));
      return -1;
    }
    while (1) {
      if (i >= 2047) {
        sprintf(csound->errmsg, Str("sprintf: format string too long"));
        return -1;
      }
      if (*fmt != '%' && *fmt != '\0') {
        strseg[i++] = *fmt++;
        continue;
      }
      if (fmt[0] == '%' && fmt[1] == '%') {
        strseg[i++] = *fmt++;
        strseg[i++] = *fmt++;
        continue;
      }
      /* if already a segment waiting, then lets print it */
      if (segwaiting != NULL) {
        maxChars = maxLen - len;
        strseg[i] = '\0';
        if (numVals <= 0) {
          sprintf(csound->errmsg,
                  Str("sprintf: insufficient arguments for format"));
          return -1;
        }
        numVals--;
        if ((*segwaiting == 's' && !(strCode & 1)) ||
            (*segwaiting != 's' && (strCode & 1))) {
          sprintf(csound->errmsg,
                  Str("sprintf: argument type inconsistent with format"));
          return -1;
        }
        strCode >>= 1;
        pp = kvals[j++];
        switch (*segwaiting) {
        case 'd':
        case 'i':
        case 'o':
        case 'x':
        case 'X':
        case 'u':
        case 'c':
#ifdef HAVE_SNPRINTF
          n = snprintf(outstring, maxChars, strseg, (int) RNDINT(*pp));
#else
          n = sprintf(outstring, strseg, (int) RNDINT(*pp));
#endif
          break;
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
#ifdef HAVE_SNPRINTF
          n = snprintf(outstring, maxChars, strseg, (double) *pp);
#else
          n = sprintf(outstring, strseg, (double) *pp);
#endif
          break;
        case 's':
          if (pp == p->r) {
            sprintf(csound->errmsg, Str("sprintf: output argument may not be "
                                        "the same as any of the input args"));
            return -1;
          }
#ifdef HAVE_SNPRINTF
          n = snprintf(outstring, maxChars, strseg, (char*) pp);
#else
          n = sprintf(outstring, strseg, (char*) pp);
#endif
          break;
        default:
          sprintf(csound->errmsg, Str("sprintf: invalid format string"));
          return -1;
        }
        if (n < 0 || n >= maxChars) {
#ifdef HAVE_SNPRINTF
          /* safely detected excess string length */
          sprintf(csound->errmsg, Str("sprintf: buffer overflow"));
#else
          /* wrote past end of buffer - hope that did not already crash ! */
          csound->Die(csound, Str("sprintf: buffer overflow"));
#endif
          return -1;
        }
        outstring += n;
        len += n;
        i = 0;
      }
      if (*fmt == '\0')
        break;
      /* copy the '%' */
      strseg[i++] = *fmt++;
      /* find the format code */
      segwaiting = fmt;
      while (!isalpha(*segwaiting) && *segwaiting != '\0')
        segwaiting++;
    }
    if (numVals > 0) {
      sprintf(csound->errmsg, Str("sprintf: too many arguments for format"));
      return -1;
    }
    return 0;
}

int sprintf_opcode_init(ENVIRON *csound, SPRINTF_OP *p)
{
    if (sprintf_opcode(csound, p) != 0) {
      ((char*) p->r)[0] = '\0';
      return csound->InitError(csound, csound->errmsg);
    }
    return OK;
}

int sprintf_opcode_perf(ENVIRON *csound, SPRINTF_OP *p)
{
    if (sprintf_opcode(csound, p) != 0) {
      ((char*) p->r)[0] = '\0';
      return csound->PerfError(csound, csound->errmsg);
    }
    return OK;
}

int puts_opcode_init(ENVIRON *csound, PUTS_OP *p)
{
    p->noNewLine = (*p->no_newline == FL(0.0) ? 0 : 1);
    if (*p->ktrig > FL(0.0)) {
      if (!p->noNewLine)
        csound->Message(csound, "%s\n", (char*) p->str);
      else
        csound->Message(csound, "%s", (char*) p->str);
    }
    p->prv_ktrig = *p->ktrig;
    return OK;
}

int puts_opcode_perf(ENVIRON *csound, PUTS_OP *p)
{
    if (*p->ktrig != p->prv_ktrig && *p->ktrig > FL(0.0)) {
      p->prv_ktrig = *p->ktrig;
      if (!p->noNewLine)
        csound->Message(csound, "%s\n", (char*) p->str);
      else
        csound->Message(csound, "%s", (char*) p->str);
    }
    return OK;
}

static int strtod_opcode(ENVIRON *csound, STRSET_OP *p,
                         int (*err_func)(void*, const char*, ...))
{
    char    *s = NULL, *tmp;
    double  x;

    if (p->XSTRCODE)
      s = (char*) p->str;
    else {
      if (*p->str == SSTRCOD)
        s = csound->currevent->strarg;
      else {
        int ndx = (int) RNDINT(*p->str);
        if (ndx >= 0 && ndx <= (int) csound->strsmax && csound->strsets != NULL)
          s = csound->strsets[ndx];
      }
      if (s == NULL)
        return err_func(csound, Str("strtod: empty string"));
    }
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '\0')
      return err_func(csound, Str("strtod: empty string"));
    x = strtod(s, &tmp);
    if (*tmp != '\0')
      return err_func(csound, Str("strtod: invalid format"));
    *p->indx = (MYFLT) x;
    return OK;
}

int strtod_opcode_init(ENVIRON *csound, STRSET_OP *p)
{
    return strtod_opcode(csound, p, (int (*)(void*, const char*, ...))
                                    csound->InitError);
}

int strtod_opcode_perf(ENVIRON *csound, STRSET_OP *p)
{
    return strtod_opcode(csound, p, (int (*)(void*, const char*, ...))
                                    csound->PerfError);
}

static int strtol_opcode(ENVIRON *csound, STRSET_OP *p,
                         int (*err_func)(void*, const char*, ...))
{
    char  *s = NULL;
    int   sgn = 0, radix = 10;
    long  x = 0L;

    if (p->XSTRCODE)
      s = (char*) p->str;
    else {
      if (*p->str == SSTRCOD)
        s = csound->currevent->strarg;
      else {
        int ndx = (int) RNDINT(*p->str);
        if (ndx >= 0 && ndx <= (int) csound->strsmax && csound->strsets != NULL)
          s = csound->strsets[ndx];
      }
      if (s == NULL)
        return err_func(csound, Str("strtol: empty string"));
    }
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '\0')
      return err_func(csound, Str("strtol: empty string"));
    if (*s == '+') s++;
    else if (*s == '-') sgn++, s++;
    if (*s == '0') {
      if (s[1] == 'x' || s[1] == 'X')
        radix = 16, s += 2;
      else if (s[1] != '\0')
        radix = 8, s++;
      else {
        *p->indx = FL(0.0);
        return OK;
      }
    }
    if (*s == '\0')
      return err_func(csound, Str("strtol: invalid format"));
    switch (radix) {
      case 8:
        while (*s >= '0' && *s <= '7') x = (x * 8L) + (long) (*s++ - '0');
        break;
      case 10:
        while (*s >= '0' && *s <= '9') x = (x * 10L) + (long) (*s++ - '0');
        break;
      default:
        while (1) {
          if (*s >= '0' && *s <= '9')
            x = (x * 16L) + (long) (*s++ - '0');
          else if (*s >= 'A' && *s <= 'F')
            x = (x * 16L) + (long) (*s++ - 'A') + 10L;
          else if (*s >= 'a' && *s <= 'f')
            x = (x * 16L) + (long) (*s++ - 'a') + 10L;
          else
            break;
        }
    }
    if (*s != '\0')
      return err_func(csound, Str("strtol: invalid format"));
    if (sgn) x = -x;
    *p->indx = (MYFLT) x;
    return OK;
}

int strtol_opcode_init(ENVIRON *csound, STRSET_OP *p)
{
    return strtol_opcode(csound, p, (int (*)(void*, const char*, ...))
                                    csound->InitError);
}

int strtol_opcode_perf(ENVIRON *csound, STRSET_OP *p)
{
    return strtol_opcode(csound, p, (int (*)(void*, const char*, ...))
                                    csound->PerfError);
}


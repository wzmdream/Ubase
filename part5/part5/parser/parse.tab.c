/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	RW_CREATE	257
# define	RW_BUILD	258
# define	RW_REBUILD	259
# define	RW_DROP	260
# define	RW_DESTROY	261
# define	RW_PRINT	262
# define	RW_LOAD	263
# define	RW_HELP	264
# define	RW_QUIT	265
# define	RW_SELECT	266
# define	RW_INTO	267
# define	RW_WHERE	268
# define	RW_INSERT	269
# define	RW_DELETE	270
# define	RW_PRIMARY	271
# define	RW_NUMBUCKETS	272
# define	RW_ALL	273
# define	RW_FROM	274
# define	RW_SET	275
# define	RW_AS	276
# define	RW_TABLE	277
# define	RW_AND	278
# define	RW_OR	279
# define	RW_NOT	280
# define	RW_UPDATE	281
# define	RW_VALUES	282
# define	INT_TYPE	283
# define	REAL_TYPE	284
# define	CHAR_TYPE	285
# define	T_EQ	286
# define	T_LT	287
# define	T_LE	288
# define	T_GT	289
# define	T_GE	290
# define	T_NE	291
# define	T_EOF	292
# define	NOTOKEN	293
# define	T_INT	294
# define	T_REAL	295
# define	T_STRING	296
# define	T_QSTRING	297
# define	T_SHELL_CMD	298

#line 1 "parse.y"

//
// parser.y: yacc specification for simple SQL-like query language
//

#include <stdlib.h>
#include "heapfile.h"
#include "parse.h"

extern "C" int isatty(int);
extern int yylex();
extern int yywrap();
extern void reset_scanner();
extern void quit();

void yyerror(char *);

extern char *yytext;                    // tokens in string format
static NODE *parse_tree;                // root of parse tree
extern int yylineno;
typedef struct YYLTYPE
{
int first_line;
int first_column;
int last_line;
int last_column;
} YYLTYPE;
YYLTYPE Current;
# define YYLSP_NEEDED 1

#line 32 "parse.y"
#ifndef YYSTYPE
typedef union{
  int ival;
  float rval;
  char *sval;
  NODE *n;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		154
#define	YYFLAG		-32768
#define	YYNTBASE	51

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 298 ? yytranslate[x] : 88)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      46,    47,    50,     2,    48,     2,    49,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    42,
      46,    50,    52,    55,    57,    68,    72,    74,    76,    80,
      82,    84,    89,    95,   103,   107,   113,   119,   122,   130,
     134,   137,   140,   146,   148,   151,   153,   156,   158,   161,
     163,   165,   167,   171,   175,   179,   183,   185,   189,   193,
     195,   197,   201,   203,   207,   211,   213,   216,   219,   225,
     228,   230,   232,   234,   236,   238,   240,   242,   244,   246,
     248
};
static const short yyrhs[] =
{
      52,    45,     0,    44,     0,     1,     0,    38,     0,    53,
       0,    56,     0,    61,     0,    62,     0,    63,     0,    64,
       0,    65,     0,    66,     0,    67,     0,    68,     0,    69,
       0,    70,     0,    87,     0,    12,    78,    72,    20,    54,
      74,     0,    46,    54,    47,     0,    55,    48,    54,     0,
      55,     0,    86,    86,     0,    86,     0,    15,    13,    86,
      46,    57,    47,    28,    46,    59,    47,     0,    58,    48,
      57,     0,    58,     0,    86,     0,    60,    48,    59,     0,
      60,     0,    85,     0,    16,    20,    86,    74,     0,    27,
      86,    21,    80,    74,     0,     3,    23,    86,    46,    82,
      47,    71,     0,     7,    23,    86,     0,     4,    86,    46,
      86,    47,     0,     6,    86,    46,    86,    47,     0,     6,
      86,     0,     9,    23,    86,    20,    46,    43,    47,     0,
       8,    23,    86,     0,    10,    73,     0,    11,    45,     0,
      17,    86,    18,    32,    40,     0,    87,     0,    13,    86,
       0,    87,     0,    23,    86,     0,    87,     0,    14,    75,
       0,    87,     0,    76,     0,    77,     0,    79,    84,    85,
       0,    79,    84,    79,     0,    46,    78,    47,     0,    79,
      48,    78,     0,    79,     0,    86,    49,    86,     0,    86,
      49,    50,     0,    86,     0,    50,     0,    81,    48,    80,
       0,    81,     0,    86,    32,    85,     0,    83,    48,    82,
       0,    83,     0,    86,    29,     0,    86,    30,     0,    86,
      31,    46,    85,    47,     0,    86,    31,     0,    33,     0,
      34,     0,    35,     0,    36,     0,    32,     0,    37,     0,
      40,     0,    41,     0,    43,     0,    42,     0,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   132,   137,   145,   151,   158,   159,   160,   161,   162,
     163,   164,   168,   169,   170,   171,   172,   173,   180,   201,
     205,   209,   216,   220,   227,   236,   240,   247,   254,   258,
     265,   272,   279,   287,   294,   301,   317,   321,   328,   334,
     341,   348,   355,   359,   366,   370,   377,   381,   388,   392,
     399,   400,   404,   411,   418,   422,   426,   433,   437,   441,
     445,   452,   456,   463,   470,   474,   481,   485,   489,   493,
     500,   504,   508,   512,   516,   520,   527,   531,   535,   542,
     549
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "RW_CREATE", "RW_BUILD", "RW_REBUILD", 
  "RW_DROP", "RW_DESTROY", "RW_PRINT", "RW_LOAD", "RW_HELP", "RW_QUIT", 
  "RW_SELECT", "RW_INTO", "RW_WHERE", "RW_INSERT", "RW_DELETE", 
  "RW_PRIMARY", "RW_NUMBUCKETS", "RW_ALL", "RW_FROM", "RW_SET", "RW_AS", 
  "RW_TABLE", "RW_AND", "RW_OR", "RW_NOT", "RW_UPDATE", "RW_VALUES", 
  "INT_TYPE", "REAL_TYPE", "CHAR_TYPE", "T_EQ", "T_LT", "T_LE", "T_GT", 
  "T_GE", "T_NE", "T_EOF", "NOTOKEN", "T_INT", "T_REAL", "T_STRING", 
  "T_QSTRING", "T_SHELL_CMD", "';'", "'('", "')'", "','", "'.'", "'*'", 
  "start", "command", "query", "table_list", "table", "insert", 
  "attrib_list", "attrib", "value_list", "val", "delete", "update", 
  "create", "destroy", "build", "drop", "load", "print", "help", "quit", 
  "opt_primary_attr", "opt_into_relname", "opt_relname", "opt_where", 
  "qual", "selection", "join", "non_mt_qualattr_list", "qualattr", 
  "non_mt_attrval_list", "attrval", "non_mt_attrtype_list", "attrtype", 
  "op", "value", "string", "nothing", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    51,    51,    51,    51,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    53,    54,
      54,    54,    55,    55,    56,    57,    57,    58,    59,    59,
      60,    61,    62,    63,    64,    65,    66,    66,    67,    68,
      69,    70,    71,    71,    72,    72,    73,    73,    74,    74,
      75,    75,    76,    77,    78,    78,    78,    79,    79,    79,
      79,    80,    80,    81,    82,    82,    83,    83,    83,    83,
      84,    84,    84,    84,    84,    84,    85,    85,    85,    86,
      87
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     3,
       3,     1,     2,     1,    10,     3,     1,     1,     3,     1,
       1,     4,     5,     7,     3,     5,     5,     2,     7,     3,
       2,     2,     5,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       1,     3,     1,     3,     3,     1,     2,     2,     5,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     3,     0,     0,     0,     0,     0,     0,    80,     0,
       0,     0,     0,     0,     4,     2,     0,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,    79,     0,    37,     0,     0,     0,     0,    40,    47,
      41,     0,    60,    80,    56,    59,     0,     0,     0,     1,
       0,     0,     0,    34,    39,     0,    46,     0,     0,     0,
      45,     0,     0,     0,    80,     0,     0,     0,     0,     0,
      54,    44,     0,    55,    58,    57,     0,     0,    31,    49,
      80,    62,     0,     0,    65,     0,    35,    36,     0,     0,
      80,    21,    23,     0,    26,    27,    48,    50,    51,     0,
      32,     0,     0,    80,     0,    66,    67,    69,     0,     0,
      18,     0,    22,     0,     0,    74,    70,    71,    72,    73,
      75,     0,    61,    76,    77,    78,    63,     0,    33,    43,
      64,     0,    38,    19,    20,     0,    25,    53,    52,     0,
       0,     0,     0,    68,     0,    29,    30,     0,    24,     0,
      42,    28,     0,     0,     0
};

static const short yydefgoto[] =
{
     152,    16,    17,    90,    91,    18,    93,    94,   144,   145,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
     128,    59,    38,    78,    96,    97,    98,    43,    44,    80,
      81,    83,    84,   121,   146,    45,    79
};

static const short yypact[] =
{
      13,-32768,   -19,   -24,   -24,    16,    18,    30,    31,     2,
     -37,    48,    36,   -24,-32768,-32768,    23,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     -24,-32768,    25,    26,   -24,   -24,   -24,   -24,-32768,-32768,
  -32768,   -37,-32768,    57,    35,    38,   -24,   -24,    64,-32768,
      42,   -24,   -24,-32768,-32768,    73,-32768,    47,   -24,    75,
  -32768,   -37,    -5,    50,    83,   -24,   -24,    52,    53,    56,
  -32768,-32768,    -4,-32768,-32768,-32768,   -24,    10,-32768,-32768,
      83,    55,    72,    58,    59,    61,-32768,-32768,    66,    -4,
      83,    62,   -24,    65,    67,-32768,-32768,-32768,-32768,    43,
  -32768,   -24,    41,    96,   -24,-32768,-32768,    68,    69,    70,
  -32768,    -4,-32768,    90,   -24,-32768,-32768,-32768,-32768,-32768,
  -32768,    24,-32768,-32768,-32768,-32768,-32768,   -24,-32768,-32768,
  -32768,    41,-32768,-32768,-32768,    74,-32768,-32768,-32768,   101,
      76,    41,    89,-32768,    78,    79,-32768,    82,-32768,    41,
  -32768,-32768,   126,   128,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   -81,-32768,-32768,    15,-32768,   -18,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   -78,-32768,-32768,-32768,   -26,   -71,    29,
  -32768,    28,-32768,-32768,   -95,    -3,     3
};


#define	YYLAST		132


static const short yytable[] =
{
      32,    33,   100,    29,    30,    31,    99,   126,   109,    41,
      48,    39,   110,    42,     1,    57,     2,     3,    31,     4,
       5,     6,     7,     8,     9,    10,   138,    50,    11,    12,
     134,    53,    54,    55,    56,    73,   140,    31,    31,    34,
      13,    35,    89,    63,    64,    74,    60,    40,    67,    68,
     137,    14,    31,    36,    37,    71,    47,    15,   -80,    75,
      42,    46,    82,    85,   123,   124,    31,   125,    49,    92,
      58,    51,    52,    95,    42,   115,   116,   117,   118,   119,
     120,   123,   124,    61,   125,    65,    92,    62,    66,   112,
     105,   106,   107,    69,    70,    72,    76,    77,    82,    86,
      87,    85,    88,   101,   102,   103,   129,   104,    92,   108,
     111,    95,   113,   127,   131,   114,   132,   133,   135,   142,
     141,   147,   150,   143,   139,   148,   153,   149,   154,   136,
     122,   151,   130
};

static const short yycheck[] =
{
       3,     4,    80,     0,    23,    42,    77,   102,    89,    46,
      13,     8,    90,    50,     1,    41,     3,     4,    42,     6,
       7,     8,     9,    10,    11,    12,   121,    30,    15,    16,
     111,    34,    35,    36,    37,    61,   131,    42,    42,    23,
      27,    23,    46,    46,    47,    50,    43,    45,    51,    52,
     121,    38,    42,    23,    23,    58,    20,    44,    45,    62,
      50,    13,    65,    66,    40,    41,    42,    43,    45,    72,
      13,    46,    46,    76,    50,    32,    33,    34,    35,    36,
      37,    40,    41,    48,    43,    21,    89,    49,    46,    92,
      29,    30,    31,    20,    47,    20,    46,    14,   101,    47,
      47,   104,    46,    48,    32,    47,   103,    48,   111,    43,
      48,   114,    47,    17,    46,    48,    47,    47,    28,    18,
      46,    32,    40,    47,   127,    47,     0,    48,     0,   114,
     101,   149,   104
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 133 "parse.y"
{
		parse_tree = yyvsp[-1].n;
		YYACCEPT;
	;
    break;}
case 2:
#line 138 "parse.y"
{
	        if(!isatty(0))
		    puts(yyvsp[0].sval);
		(void)system(yyvsp[0].sval);
		parse_tree = NULL;
		YYACCEPT;
	;
    break;}
case 3:
#line 146 "parse.y"
{
		reset_scanner();
		parse_tree = NULL;
		YYACCEPT;
	;
    break;}
case 4:
#line 152 "parse.y"
{
		quit();
	;
    break;}
case 17:
#line 174 "parse.y"
{
		yyval.n = NULL;
	;
    break;}
case 18:
#line 182 "parse.y"
{
		NODE *where;
		NODE *qualattr_list = replace_alias_in_qualattr_list(yyvsp[-1].n, yyvsp[-4].n);
		if (qualattr_list == NULL) { // something wrong in qualattr_list
		  yyval.n = NULL;
		}
		else {
		  where = replace_alias_in_condition(yyvsp[-1].n, yyvsp[0].n);
		  if ((where == NULL) && (yyvsp[0].n != NULL)) {
		     yyval.n = NULL; //something wrong in where condition
		  }
		  else {
		    yyval.n = query_node(yyvsp[-3].sval, qualattr_list, where);
		  }
		}
	;
    break;}
case 19:
#line 202 "parse.y"
{
		yyval.n = yyvsp[-1].n;
	;
    break;}
case 20:
#line 206 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 21:
#line 210 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 22:
#line 217 "parse.y"
{
		yyval.n = alias_node(yyvsp[-1].sval, yyvsp[0].sval);
	;
    break;}
case 23:
#line 221 "parse.y"
{
		yyval.n = alias_node(yyvsp[0].sval, NULL);
	;
    break;}
case 24:
#line 228 "parse.y"
{
		NODE* tmp = merge_attr_value_list(yyvsp[-5].n, yyvsp[-1].n);
		if (tmp == NULL) yyval.n=NULL;
		else yyval.n = insert_node(yyvsp[-7].sval, tmp);
	;
    break;}
case 25:
#line 237 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 26:
#line 241 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 27:
#line 248 "parse.y"
{
		yyval.n = attrval_node(yyvsp[0].sval, NULL);
	;
    break;}
case 28:
#line 255 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 29:
#line 259 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 30:
#line 266 "parse.y"
{
		yyval.n = yyvsp[0].n;
	;
    break;}
case 31:
#line 273 "parse.y"
{ 
		yyval.n = delete_node(yyvsp[-1].sval, yyvsp[0].n);
	;
    break;}
case 32:
#line 280 "parse.y"
{
		yyval.n = update_node(yyvsp[-3].sval,yyvsp[-1].n,yyvsp[0].n);
	;
    break;}
case 33:
#line 288 "parse.y"
{
		yyval.n = create_node(yyvsp[-4].sval, yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 34:
#line 295 "parse.y"
{
		yyval.n = destroy_node(yyvsp[0].sval);
	;
    break;}
case 35:
#line 302 "parse.y"
{
		yyval.n = build_node(yyvsp[-3].sval, yyvsp[-1].sval, 0);
	;
    break;}
case 36:
#line 318 "parse.y"
{
		yyval.n = drop_node(yyvsp[-3].sval, yyvsp[-1].sval);
	;
    break;}
case 37:
#line 322 "parse.y"
{
		yyval.n = drop_node(yyvsp[0].sval, NULL);
	;
    break;}
case 38:
#line 329 "parse.y"
{
		yyval.n = load_node(yyvsp[-4].sval, yyvsp[-1].sval);
	;
    break;}
case 39:
#line 335 "parse.y"
{
		yyval.n = print_node(yyvsp[0].sval);
	;
    break;}
case 40:
#line 342 "parse.y"
{
		yyval.n = help_node(yyvsp[0].sval);
	;
    break;}
case 41:
#line 349 "parse.y"
{
		quit();
	;
    break;}
case 42:
#line 356 "parse.y"
{
		yyval.n = primattr_node(yyvsp[-3].sval, yyvsp[0].ival);
	;
    break;}
case 43:
#line 360 "parse.y"
{
		yyval.n = NULL;
	;
    break;}
case 44:
#line 367 "parse.y"
{
		yyval.sval = yyvsp[0].sval;
	;
    break;}
case 45:
#line 371 "parse.y"
{
		yyval.sval = NULL;
	;
    break;}
case 46:
#line 378 "parse.y"
{
		yyval.sval = yyvsp[0].sval;
	;
    break;}
case 47:
#line 382 "parse.y"
{
		yyval.sval = NULL;
	;
    break;}
case 48:
#line 389 "parse.y"
{
		yyval.n = yyvsp[0].n;
	;
    break;}
case 49:
#line 393 "parse.y"
{
		yyval.n = NULL;
	;
    break;}
case 52:
#line 405 "parse.y"
{
		yyval.n = select_node(yyvsp[-2].n, yyvsp[-1].ival, yyvsp[0].n);
	;
    break;}
case 53:
#line 412 "parse.y"
{
		yyval.n = join_node(yyvsp[-2].n, yyvsp[-1].ival, yyvsp[0].n);
	;
    break;}
case 54:
#line 419 "parse.y"
{
		yyval.n = yyvsp[-1].n;
	;
    break;}
case 55:
#line 423 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 56:
#line 427 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 57:
#line 434 "parse.y"
{
		yyval.n = qualattr_node(yyvsp[-2].sval, yyvsp[0].sval);
	;
    break;}
case 58:
#line 438 "parse.y"
{
		yyval.n = qualattr_node(yyvsp[-2].sval, "*");
	;
    break;}
case 59:
#line 442 "parse.y"
{
		yyval.n = qualattr_node(NULL, yyvsp[0].sval);
	;
    break;}
case 60:
#line 446 "parse.y"
{
                yyval.n = qualattr_node(NULL, "*");
        ;
    break;}
case 61:
#line 453 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 62:
#line 457 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 63:
#line 464 "parse.y"
{
		yyval.n = attrval_node(yyvsp[-2].sval, yyvsp[0].n);
	;
    break;}
case 64:
#line 471 "parse.y"
{
		yyval.n = prepend(yyvsp[-2].n, yyvsp[0].n);
	;
    break;}
case 65:
#line 475 "parse.y"
{
		yyval.n = list_node(yyvsp[0].n);
	;
    break;}
case 66:
#line 482 "parse.y"
{
		yyval.n = attrtype_node(yyvsp[-1].sval, 'i'-128);
	;
    break;}
case 67:
#line 486 "parse.y"
{
		yyval.n = attrtype_node(yyvsp[-1].sval, 'f'-128);
	;
    break;}
case 68:
#line 490 "parse.y"
{
	 	yyval.n = attrtype_node(yyvsp[-4].sval, yyvsp[-1].n->u.VALUE.u.ival);
	;
    break;}
case 69:
#line 494 "parse.y"
{
		yyval.n = attrtype_node(yyvsp[-1].sval, 2);
	;
    break;}
case 70:
#line 501 "parse.y"
{
		yyval.ival = LT;
	;
    break;}
case 71:
#line 505 "parse.y"
{
		yyval.ival = LTE;
	;
    break;}
case 72:
#line 509 "parse.y"
{
		yyval.ival = GT;
	;
    break;}
case 73:
#line 513 "parse.y"
{
		yyval.ival = GTE;
	;
    break;}
case 74:
#line 517 "parse.y"
{
		yyval.ival = EQ;
	;
    break;}
case 75:
#line 521 "parse.y"
{
		yyval.ival = NE;
	;
    break;}
case 76:
#line 528 "parse.y"
{
		yyval.n = int_node(yyvsp[0].ival);
	;
    break;}
case 77:
#line 532 "parse.y"
{
		yyval.n = float_node(yyvsp[0].rval);
	;
    break;}
case 78:
#line 536 "parse.y"
{
		yyval.n = string_node(yyvsp[0].sval);
	;
    break;}
case 79:
#line 543 "parse.y"
{
		yyval.sval = yyvsp[0].sval;
	;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 552 "parse.y"


void parse(void)
{
  extern void new_query();
  extern void interp(NODE *);

  for(;;){
    yylineno=1;
    // reset parser and scanner for a new query
    new_query();

    // print a prompt
    printf("%s", PROMPT);
    fflush(stdout);

    // if a query was successfully read, interpret it
    if(yyparse() == 0 && parse_tree != NULL)
      interp(parse_tree);
  }
}

void yyerror(char *s)
{
    printf("Error: %s encountered at line:%d col:%d to col:%d\n", s, yylineno,(Current).first_column,(Current).last_column);
//(Current).first_line
//(Current).first_column
//(Current).last_column
    //puts(s);
}

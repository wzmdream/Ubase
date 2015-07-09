#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

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
# define	RW_AS	275
# define	RW_TABLE	276
# define	RW_AND	277
# define	RW_OR	278
# define	RW_NOT	279
# define	RW_VALUES	280
# define	INT_TYPE	281
# define	REAL_TYPE	282
# define	CHAR_TYPE	283
# define	T_EQ	284
# define	T_LT	285
# define	T_LE	286
# define	T_GT	287
# define	T_GE	288
# define	T_NE	289
# define	T_EOF	290
# define	NOTOKEN	291
# define	T_INT	292
# define	T_REAL	293
# define	T_STRING	294
# define	T_QSTRING	295
# define	T_SHELL_CMD	296


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */

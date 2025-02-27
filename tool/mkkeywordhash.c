/*
** Compile and run this standalone program in order to generate code that
** implements a function that will translate alphabetic identifiers into
** parser token codes.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
** A header comment placed at the beginning of generated code.
*/
static const char zHdr[] = 
  "/***** This file contains automatically generated code ******\n"
  "**\n"
  "** The code in this file has been automatically generated by\n"
  "**\n"
  "**   sqlite/tool/mkkeywordhash.c\n"
  "**\n"
  "** The code in this file implements a function that determines whether\n"
  "** or not a given identifier is really an SQL keyword.  The same thing\n"
  "** might be implemented more directly using a hand-written hash table.\n"
  "** But by using this automatically generated code, the size of the code\n"
  "** is substantially reduced.  This is important for embedded applications\n"
  "** on platforms with limited memory.\n"
  "*/\n"
;

/*
** All the keywords of the SQL language are stored in a hash
** table composed of instances of the following structure.
*/
typedef struct Keyword Keyword;
struct Keyword {
  char *zName;         /* The keyword name */
  char *zTokenType;    /* Token value for this keyword */
  int mask;            /* Code this keyword if non-zero */
  int priority;        /* Put higher priorities earlier in the hash chain */
  int id;              /* Unique ID for this record */
  int hash;            /* Hash on the keyword */
  int offset;          /* Offset to start of name string */
  int len;             /* Length of this keyword, not counting final \000 */
  int prefix;          /* Number of characters in prefix */
  int longestSuffix;   /* Longest suffix that is a prefix on another word */
  int iNext;           /* Index in aKeywordTable[] of next with same hash */
  int substrId;        /* Id to another keyword this keyword is embedded in */
  int substrOffset;    /* Offset into substrId for start of this keyword */
  char zOrigName[20];  /* Original keyword name before processing */
};

/*
** Define masks used to determine which keywords are allowed
*/
#if defined(SQLITE_OMIT_ALTERTABLE) || defined(SQLITE_OMIT_VIRTUALTABLE)
#  define ALTER      0
#else
#  define ALTER      0x00000001
#endif
#define ALWAYS       0x00000002
#ifdef SQLITE_OMIT_ANALYZE
#  define ANALYZE    0
#else
#  define ANALYZE    0x00000004
#endif
#ifdef SQLITE_OMIT_ATTACH
#  define ATTACH     0
#else
#  define ATTACH     0x00000008
#endif
#ifdef SQLITE_OMIT_AUTOINCREMENT
#  define AUTOINCR   0
#else
#  define AUTOINCR   0x00000010
#endif
#ifdef SQLITE_OMIT_CAST
#  define CAST       0
#else
#  define CAST       0x00000020
#endif
#ifdef SQLITE_OMIT_COMPOUND_SELECT
#  define COMPOUND   0
#else
#  define COMPOUND   0x00000040
#endif
#ifdef SQLITE_OMIT_CONFLICT_CLAUSE
#  define CONFLICT   0
#else
#  define CONFLICT   0x00000080
#endif
#ifdef SQLITE_OMIT_EXPLAIN
#  define EXPLAIN    0
#else
#  define EXPLAIN    0x00000100
#endif
#ifdef SQLITE_OMIT_FOREIGN_KEY
#  define FKEY       0
#else
#  define FKEY       0x00000200
#endif
#ifdef SQLITE_OMIT_PRAGMA
#  define PRAGMA     0
#else
#  define PRAGMA     0x00000400
#endif
#ifdef SQLITE_OMIT_REINDEX
#  define REINDEX    0
#else
#  define REINDEX    0x00000800
#endif
#ifdef SQLITE_OMIT_SUBQUERY
#  define SUBQUERY   0
#else
#  define SUBQUERY   0x00001000
#endif
#ifdef SQLITE_OMIT_TRIGGER
#  define TRIGGER    0
#else
#  define TRIGGER    0x00002000
#endif
#if defined(SQLITE_OMIT_AUTOVACUUM) && \
    (defined(SQLITE_OMIT_VACUUM) || defined(SQLITE_OMIT_ATTACH))
#  define VACUUM     0
#else
#  define VACUUM     0x00004000
#endif
#ifdef SQLITE_OMIT_VIEW
#  define VIEW       0
#else
#  define VIEW       0x00008000
#endif
#ifdef SQLITE_OMIT_VIRTUALTABLE
#  define VTAB       0
#else
#  define VTAB       0x00010000
#endif
#ifdef SQLITE_OMIT_AUTOVACUUM
#  define AUTOVACUUM 0
#else
#  define AUTOVACUUM 0x00020000
#endif
#ifdef SQLITE_OMIT_CTE
#  define CTE        0
#else
#  define CTE        0x00040000
#endif
#ifdef SQLITE_OMIT_UPSERT
#  define UPSERT     0
#else
#  define UPSERT     0x00080000
#endif
#ifdef SQLITE_OMIT_WINDOWFUNC
#  define WINDOWFUNC 0
#else
#  define WINDOWFUNC 0x00100000
#endif
#ifdef SQLITE_OMIT_GENERATED_COLUMNS
#  define GENCOL     0
#else
#  define GENCOL     0x00200000
#endif
#ifdef SQLITE_OMIT_RETURNING
#  define RETURNING  0
#else
#  define RETURNING  0x00400000
#endif


/*
** These are the keywords
*/
static Keyword aKeywordTable[] = {
  { "ABORT",            "TK_ABORT",        CONFLICT|TRIGGER, 0      },
  { "ACTION",           "TK_ACTION",       FKEY,             0      },
  { "ADD",              "TK_ADD",          ALTER,            1      },
  { "AFTER",            "TK_AFTER",        TRIGGER,          0      },
  { "ALL",              "TK_ALL",          ALWAYS,           0      },
  { "ALTER",            "TK_ALTER",        ALTER,            0      },
  { "ALWAYS",           "TK_ALWAYS",       GENCOL,           0      },
  { "ANALYZE",          "TK_ANALYZE",      ANALYZE,          0      },
  { "AND",              "TK_AND",          ALWAYS,           10     },
  { "AS",               "TK_AS",           ALWAYS,           10     },
  { "ASC",              "TK_ASC",          ALWAYS,           0      },
  { "ATTACH",           "TK_ATTACH",       ATTACH,           1      },
  { "AUTOINCREMENT",    "TK_AUTOINCR",     AUTOINCR,         0      },
  { "BEFORE",           "TK_BEFORE",       TRIGGER,          0      },
  { "BEGIN",            "TK_BEGIN",        ALWAYS,           1      },
  { "BETWEEN",          "TK_BETWEEN",      ALWAYS,           5      },
  { "BY",               "TK_BY",           ALWAYS,           10     },
  { "CASCADE",          "TK_CASCADE",      FKEY,             1      },
  { "CASE",             "TK_CASE",         ALWAYS,           5      },
  { "CAST",             "TK_CAST",         CAST,             5      },
  { "CHECK",            "TK_CHECK",        ALWAYS,           1      },
  { "COLLATE",          "TK_COLLATE",      ALWAYS,           1      },
  { "COLUMN",           "TK_COLUMNKW",     ALTER,            1      },
  { "COMMIT",           "TK_COMMIT",       ALWAYS,           1      },
  { "CONFLICT",         "TK_CONFLICT",     CONFLICT,         0      },
  { "CONSTRAINT",       "TK_CONSTRAINT",   ALWAYS,           1      },
  { "CREATE",           "TK_CREATE",       ALWAYS,           2      },
  { "CROSS",            "TK_JOIN_KW",      ALWAYS,           3      },
  { "CURRENT",          "TK_CURRENT",      WINDOWFUNC,       1      },
  { "CURRENT_DATE",     "TK_CTIME_KW",     ALWAYS,           1      },
  { "CURRENT_TIME",     "TK_CTIME_KW",     ALWAYS,           1      },
  { "CURRENT_TIMESTAMP","TK_CTIME_KW",     ALWAYS,           1      },
  { "DATABASE",         "TK_DATABASE",     ATTACH,           0      },
  { "DEFAULT",          "TK_DEFAULT",      ALWAYS,           1      },
  { "DEFERRED",         "TK_DEFERRED",     ALWAYS,           1      },
  { "DEFERRABLE",       "TK_DEFERRABLE",   FKEY,             1      },
  { "DELETE",           "TK_DELETE",       ALWAYS,           10     },
  { "DESC",             "TK_DESC",         ALWAYS,           3      },
  { "DETACH",           "TK_DETACH",       ATTACH,           0      },
  { "DISTINCT",         "TK_DISTINCT",     ALWAYS,           5      },
  { "DO",               "TK_DO",           UPSERT,           2      },
  { "DROP",             "TK_DROP",         ALWAYS,           1      },
  { "END",              "TK_END",          ALWAYS,           1      },
  { "EACH",             "TK_EACH",         TRIGGER,          1      },
  { "ELSE",             "TK_ELSE",         ALWAYS,           2      },
  { "ESCAPE",           "TK_ESCAPE",       ALWAYS,           4      },
  { "EXCEPT",           "TK_EXCEPT",       COMPOUND,         4      },
  { "EXCLUSIVE",        "TK_EXCLUSIVE",    ALWAYS,           1      },
  { "EXCLUDE",          "TK_EXCLUDE",      WINDOWFUNC,       1      },
  { "EXISTS",           "TK_EXISTS",       ALWAYS,           4      },
  { "EXPLAIN",          "TK_EXPLAIN",      EXPLAIN,          1      },
  { "FAIL",             "TK_FAIL",         CONFLICT|TRIGGER, 1      },
  { "FILTER",           "TK_FILTER",       WINDOWFUNC,       4      },
  { "FIRST",            "TK_FIRST",        ALWAYS,           4      },
  { "FOLLOWING",        "TK_FOLLOWING",    WINDOWFUNC,       4      },
  { "FOR",              "TK_FOR",          TRIGGER,          2      },
  { "FOREIGN",          "TK_FOREIGN",      FKEY,             1      },
  { "FROM",             "TK_FROM",         ALWAYS,           10     },
  { "FULL",             "TK_JOIN_KW",      ALWAYS,           3      },
  { "FUNCTION",          "TK_FUNCTION",    ALWAYS,           1,     },
  { "GENERATED",        "TK_GENERATED",    ALWAYS,           1      },
  { "GLOB",             "TK_LIKE_KW",      ALWAYS,           3      },
  { "GROUP",            "TK_GROUP",        ALWAYS,           5      },
  { "GROUPS",           "TK_GROUPS",       WINDOWFUNC,       2      },
  { "HAVING",           "TK_HAVING",       ALWAYS,           5      },
  { "IF",               "TK_IF",           ALWAYS,           2      },
  { "IGNORE",           "TK_IGNORE",       CONFLICT|TRIGGER, 1      },
  { "IMMEDIATE",        "TK_IMMEDIATE",    ALWAYS,           1      },
  { "IN",               "TK_IN",           ALWAYS,           10     },
  { "INDEX",            "TK_INDEX",        ALWAYS,           1      },
  { "INDEXED",          "TK_INDEXED",      ALWAYS,           0      },
  { "INITIALLY",        "TK_INITIALLY",    FKEY,             1      },
  { "INNER",            "TK_JOIN_KW",      ALWAYS,           1      },
  { "INSERT",           "TK_INSERT",       ALWAYS,           10     },
  { "INSTEAD",          "TK_INSTEAD",      TRIGGER,          1      },
  { "INTERSECT",        "TK_INTERSECT",    COMPOUND,         5      },
  { "INTO",             "TK_INTO",         ALWAYS,           10     },
  { "IS",               "TK_IS",           ALWAYS,           5      },
  { "ISNULL",           "TK_ISNULL",       ALWAYS,           5      },
  { "JOIN",             "TK_JOIN",         ALWAYS,           5      },
  { "KEY",              "TK_KEY",          ALWAYS,           1      },
  { "LANGUAGE",         "TK_LANGUAGE",     ALWAYS,           0      },
  { "LAST",             "TK_LAST",         ALWAYS,           4      },
  { "LEFT",             "TK_JOIN_KW",      ALWAYS,           5      },
  { "LIKE",             "TK_LIKE_KW",      ALWAYS,           5      },
  { "LIMIT",            "TK_LIMIT",        ALWAYS,           3      },
  { "MATCH",            "TK_MATCH",        ALWAYS,           2      },
  { "MATERIALIZED",     "TK_MATERIALIZED", CTE,              12     },
  { "NATURAL",          "TK_JOIN_KW",      ALWAYS,           3      },
  { "NO",               "TK_NO",           FKEY|WINDOWFUNC,  2      },
  { "NOT",              "TK_NOT",          ALWAYS,           10     },
  { "NOTHING",          "TK_NOTHING",      UPSERT,           1      },
  { "NOTNULL",          "TK_NOTNULL",      ALWAYS,           3      },
  { "NULL",             "TK_NULL",         ALWAYS,           10     },
  { "NULLS",            "TK_NULLS",        ALWAYS,           3      },
  { "OF",               "TK_OF",           ALWAYS,           3      },
  { "OFFSET",           "TK_OFFSET",       ALWAYS,           1      },
  { "ON",               "TK_ON",           ALWAYS,           1      },
  { "OR",               "TK_OR",           ALWAYS,           9      },
  { "ORDER",            "TK_ORDER",        ALWAYS,           10     },
  { "OTHERS",           "TK_OTHERS",       WINDOWFUNC,       3      },
  { "OUTER",            "TK_JOIN_KW",      ALWAYS,           5      },
  { "OVER",             "TK_OVER",         WINDOWFUNC,       3      },
  { "PARTITION",        "TK_PARTITION",    WINDOWFUNC,       3      },
  { "PLAN",             "TK_PLAN",         EXPLAIN,          0      },
  { "PRAGMA",           "TK_PRAGMA",       PRAGMA,           0      },
  { "PRECEDING",        "TK_PRECEDING",    WINDOWFUNC,       3      },
  { "PRIMARY",          "TK_PRIMARY",      ALWAYS,           1      },
  { "QUERY",            "TK_QUERY",        EXPLAIN,          0      },
  { "RAISE",            "TK_RAISE",        TRIGGER,          1      },
  { "RANDOM",           "TK_RANDOM",       ALWAYS,           1      },
  { "RANGE",            "TK_RANGE",        WINDOWFUNC,       3      },
  { "RECURSIVE",        "TK_RECURSIVE",    CTE,              3      },
  { "REFERENCES",       "TK_REFERENCES",   FKEY,             1      },
  { "REGEXP",           "TK_LIKE_KW",      ALWAYS,           3      },
  { "REINDEX",          "TK_REINDEX",      REINDEX,          1      },
  { "RELEASE",          "TK_RELEASE",      ALWAYS,           1      },
  { "RENAME",           "TK_RENAME",       ALTER,            1      },
  { "REPLACE",          "TK_REPLACE",      CONFLICT,         10     },
  { "RESTRICT",         "TK_RESTRICT",     FKEY,             1      },
  { "RETURNING",        "TK_RETURNING",    RETURNING,        10     },
  { "RIGHT",            "TK_JOIN_KW",      ALWAYS,           0      },
  { "ROLLBACK",         "TK_ROLLBACK",     ALWAYS,           1      },
  { "ROW",              "TK_ROW",          TRIGGER,          1      },
  { "ROWS",             "TK_ROWS",         ALWAYS,           1      },
  { "SAVEPOINT",        "TK_SAVEPOINT",    ALWAYS,           1      },
  { "SELECT",           "TK_SELECT",       ALWAYS,           10     },
  { "SET",              "TK_SET",          ALWAYS,           10     },
  { "TABLE",            "TK_TABLE",        ALWAYS,           1      },
  { "TEMP",             "TK_TEMP",         ALWAYS,           1      },
  { "TEMPORARY",        "TK_TEMP",         ALWAYS,           1      },
  { "THEN",             "TK_THEN",         ALWAYS,           3      },
  { "TIES",             "TK_TIES",         WINDOWFUNC,       3      },
  { "TO",               "TK_TO",           ALWAYS,           3      },
  { "TRANSACTION",      "TK_TRANSACTION",  ALWAYS,           1      },
  { "TRIGGER",          "TK_TRIGGER",      TRIGGER,          1      },
  { "UNBOUNDED",        "TK_UNBOUNDED",    WINDOWFUNC,       3      },
  { "UNION",            "TK_UNION",        COMPOUND,         3      },
  { "UNIQUE",           "TK_UNIQUE",       ALWAYS,           1      },
  { "UPDATE",           "TK_UPDATE",       ALWAYS,           10     },
  { "USING",            "TK_USING",        ALWAYS,           8      },
  { "VACUUM",           "TK_VACUUM",       VACUUM,           1      },
  { "VALUES",           "TK_VALUES",       ALWAYS,           10     },
  { "VIEW",             "TK_VIEW",         VIEW,             1      },
  { "VIRTUAL",          "TK_VIRTUAL",      VTAB,             1      },
  { "WHEN",             "TK_WHEN",         ALWAYS,           1      },
  { "WHERE",            "TK_WHERE",        ALWAYS,           10     },
  { "WINDOW",           "TK_WINDOW",       WINDOWFUNC,       3      },
  { "WITH",             "TK_WITH",         CTE,              4      },
  { "WITHOUT",          "TK_WITHOUT",      ALWAYS,           1      },
};

/* Number of keywords */
static int nKeyword = (sizeof(aKeywordTable)/sizeof(aKeywordTable[0]));

/* Map all alphabetic characters into lower-case for hashing.  This is
** only valid for alphabetics.  In particular it does not work for '_'
** and so the hash cannot be on a keyword position that might be an '_'.
*/
#define charMap(X)   (0x20|(X))

/*
** Comparision function for two Keyword records
*/
static int keywordCompare1(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pA->len - pB->len;
  if( n==0 ){
    n = strcmp(pA->zName, pB->zName);
  }
  assert( n!=0 );
  return n;
}
static int keywordCompare2(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pB->longestSuffix - pA->longestSuffix;
  if( n==0 ){
    n = strcmp(pA->zName, pB->zName);
  }
  assert( n!=0 );
  return n;
}
static int keywordCompare3(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  int n = pA->offset - pB->offset;
  if( n==0 ) n = pB->id - pA->id;
  assert( n!=0 );
  return n;
}

/*
** Return a KeywordTable entry with the given id
*/
static Keyword *findById(int id){
  int i;
  for(i=0; i<nKeyword; i++){
    if( aKeywordTable[i].id==id ) break;
  }
  return &aKeywordTable[i];
}

/*
** If aKeyword[*pFrom-1].iNext has a higher priority that aKeyword[*pFrom-1]
** itself, then swap them.
*/
static void reorder(int *pFrom){
  int i = *pFrom - 1;
  int j;
  if( i<0 ) return;
  j = aKeywordTable[i].iNext;
  if( j==0 ) return;
  j--;
  if( aKeywordTable[i].priority >= aKeywordTable[j].priority ) return;
  aKeywordTable[i].iNext = aKeywordTable[j].iNext;
  aKeywordTable[j].iNext = i+1;
  *pFrom = j+1;
  reorder(&aKeywordTable[i].iNext);
}

/* Parameter to the hash function
*/
#define HASH_OP ^
#define HASH_CC '^'
#define HASH_C0 4
#define HASH_C1 3
#define HASH_C2 1

/*
** This routine does the work.  The generated code is printed on standard
** output.
*/
int main(int argc, char **argv){
  int i, j, k, h;
  int bestSize, bestCount;
  int count;
  int nChar;
  int totalLen = 0;
  int aKWHash[1000];  /* 1000 is much bigger than nKeyword */
  char zKWText[2000];

  /* Remove entries from the list of keywords that have mask==0 */
  for(i=j=0; i<nKeyword; i++){
    if( aKeywordTable[i].mask==0 ) continue;
    if( j<i ){
      aKeywordTable[j] = aKeywordTable[i];
    }
    j++;
  }
  nKeyword = j;

  /* Fill in the lengths of strings and hashes for all entries. */
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    p->len = (int)strlen(p->zName);
    assert( p->len<sizeof(p->zOrigName) );
    memcpy(p->zOrigName, p->zName, p->len+1);
    totalLen += p->len;
    p->hash = (charMap(p->zName[0])*HASH_C0) HASH_OP
              (charMap(p->zName[p->len-1])*HASH_C1) HASH_OP
              (p->len*HASH_C2);
    p->id = i+1;
  }

  /* Sort the table from shortest to longest keyword */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare1);

  /* Look for short keywords embedded in longer keywords */
  for(i=nKeyword-2; i>=0; i--){
    Keyword *p = &aKeywordTable[i];
    for(j=nKeyword-1; j>i && p->substrId==0; j--){
      Keyword *pOther = &aKeywordTable[j];
      if( pOther->substrId ) continue;
      if( pOther->len<=p->len ) continue;
      for(k=0; k<=pOther->len-p->len; k++){
        if( memcmp(p->zName, &pOther->zName[k], p->len)==0 ){
          p->substrId = pOther->id;
          p->substrOffset = k;
          break;
        }
      }
    }
  }

  /* Compute the longestSuffix value for every word */
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ) continue;
    for(j=0; j<nKeyword; j++){
      Keyword *pOther;
      if( j==i ) continue;
      pOther = &aKeywordTable[j];
      if( pOther->substrId ) continue;
      for(k=p->longestSuffix+1; k<p->len && k<pOther->len; k++){
        if( memcmp(&p->zName[p->len-k], pOther->zName, k)==0 ){
          p->longestSuffix = k;
        }
      }
    }
  }

  /* Sort the table into reverse order by length */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare2);

  /* Fill in the offset for all entries */
  nChar = 0;
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->offset>0 || p->substrId ) continue;
    p->offset = nChar;
    nChar += p->len;
    for(k=p->len-1; k>=1; k--){
      for(j=i+1; j<nKeyword; j++){
        Keyword *pOther = &aKeywordTable[j];
        if( pOther->offset>0 || pOther->substrId ) continue;
        if( pOther->len<=k ) continue;
        if( memcmp(&p->zName[p->len-k], pOther->zName, k)==0 ){
          p = pOther;
          p->offset = nChar - k;
          nChar = p->offset + p->len;
          p->zName += k;
          p->len -= k;
          p->prefix = k;
          j = i;
          k = p->len;
        }
      }
    }
  }
  for(i=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ){
      p->offset = findById(p->substrId)->offset + p->substrOffset;
    }
  }

  /* Sort the table by offset */
  qsort(aKeywordTable, nKeyword, sizeof(aKeywordTable[0]), keywordCompare3);

  /* Figure out how big to make the hash table in order to minimize the
  ** number of collisions */
  bestSize = nKeyword;
  bestCount = nKeyword*nKeyword;
  for(i=nKeyword/2; i<=2*nKeyword; i++){
    if( i<=0 ) continue;
    for(j=0; j<i; j++) aKWHash[j] = 0;
    for(j=0; j<nKeyword; j++){
      h = aKeywordTable[j].hash % i;
      aKWHash[h] *= 2;
      aKWHash[h]++;
    }
    for(j=count=0; j<i; j++) count += aKWHash[j];
    if( count<bestCount ){
      bestCount = count;
      bestSize = i;
    }
  }

  /* Compute the hash */
  for(i=0; i<bestSize; i++) aKWHash[i] = 0;
  for(i=0; i<nKeyword; i++){
    h = aKeywordTable[i].hash % bestSize;
    aKeywordTable[i].iNext = aKWHash[h];
    aKWHash[h] = i+1;
    reorder(&aKWHash[h]);
  }

  /* Begin generating code */
  printf("%s", zHdr);
  printf("/* Hash score: %d */\n", bestCount);
  printf("/* zKWText[] encodes %d bytes of keyword text in %d bytes */\n",
          totalLen + nKeyword, nChar+1 );
  for(i=j=k=0; i<nKeyword; i++){
    Keyword *p = &aKeywordTable[i];
    if( p->substrId ) continue;
    memcpy(&zKWText[k], p->zName, p->len);
    k += p->len;
    if( j+p->len>70 ){
      printf("%*s */\n", 74-j, "");
      j = 0;
    }
    if( j==0 ){
      printf("/*   ");
      j = 8;
    }
    printf("%s", p->zName);
    j += p->len;
  }
  if( j>0 ){
    printf("%*s */\n", 74-j, "");
  }
  printf("static const char zKWText[%d] = {\n", nChar);
  zKWText[nChar] = 0;
  for(i=j=0; i<k; i++){
    if( j==0 ){
      printf("  ");
    }
    if( zKWText[i]==0 ){
      printf("0");
    }else{
      printf("'%c',", zKWText[i]);
    }
    j += 4;
    if( j>68 ){
      printf("\n");
      j = 0;
    }
  }
  if( j>0 ) printf("\n");
  printf("};\n");

  printf("/* aKWHash[i] is the hash value for the i-th keyword */\n");
  printf("static const unsigned char aKWHash[%d] = {\n", bestSize);
  for(i=j=0; i<bestSize; i++){
    if( j==0 ) printf("  ");
    printf(" %3d,", aKWHash[i]);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s};\n", j==0 ? "" : "\n");    

  printf("/* aKWNext[] forms the hash collision chain.  If aKWHash[i]==0\n");
  printf("** then the i-th keyword has no more hash collisions.  Otherwise,\n");
  printf("** the next keyword with the same hash is aKWHash[i]-1. */\n");
  printf("static const unsigned char aKWNext[%d] = {0,\n", nKeyword+1);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("  ");
    printf(" %3d,", aKeywordTable[i].iNext);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s};\n", j==0 ? "" : "\n");    

  printf("/* aKWLen[i] is the length (in bytes) of the i-th keyword */\n");
  printf("static const unsigned char aKWLen[%d] = {0,\n", nKeyword+1);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("  ");
    printf(" %3d,", aKeywordTable[i].len+aKeywordTable[i].prefix);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s};\n", j==0 ? "" : "\n");    

  printf("/* aKWOffset[i] is the index into zKWText[] of the start of\n");
  printf("** the text for the i-th keyword. */\n");
  printf("static const unsigned short int aKWOffset[%d] = {0,\n", nKeyword+1);
  for(i=j=0; i<nKeyword; i++){
    if( j==0 ) printf("  ");
    printf(" %3d,", aKeywordTable[i].offset);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s};\n", j==0 ? "" : "\n");

  printf("/* aKWCode[i] is the parser symbol code for the i-th keyword */\n");
  printf("static const unsigned char aKWCode[%d] = {0,\n", nKeyword+1);
  for(i=j=0; i<nKeyword; i++){
    char *zToken = aKeywordTable[i].zTokenType;
    if( j==0 ) printf("  ");
    printf("%s,%*s", zToken, (int)(14-strlen(zToken)), "");
    j++;
    if( j>=5 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s};\n", j==0 ? "" : "\n");
  printf("/* Hash table decoded:\n");
  for(i=0; i<bestSize; i++){
    j = aKWHash[i];
    printf("** %3d:", i);
    while( j ){
      printf(" %s", aKeywordTable[j-1].zOrigName);
      j = aKeywordTable[j-1].iNext;
    }
    printf("\n");
  }
  printf("*/\n");
  printf("/* Check to see if z[0..n-1] is a keyword. If it is, write the\n");
  printf("** parser symbol code for that keyword into *pType.  Always\n");
  printf("** return the integer n (the length of the token). */\n");
  printf("static int keywordCode(const char *z, int n, int *pType){\n");
  printf("  int i, j;\n");
  printf("  const char *zKW;\n");
  printf("  assert( n>=2 );\n");
  printf("  i = ((charMap(z[0])*%d) %c", HASH_C0, HASH_CC);
  printf(" (charMap(z[n-1])*%d) %c", HASH_C1, HASH_CC);
  printf(" n*%d) %% %d;\n", HASH_C2, bestSize);
  printf("  for(i=(int)aKWHash[i]; i>0; i=aKWNext[i]){\n");
  printf("    if( aKWLen[i]!=n ) continue;\n");
  printf("    zKW = &zKWText[aKWOffset[i]];\n");
  printf("#ifdef SQLITE_ASCII\n");
  printf("    if( (z[0]&~0x20)!=zKW[0] ) continue;\n");
  printf("    if( (z[1]&~0x20)!=zKW[1] ) continue;\n");
  printf("    j = 2;\n");
  printf("    while( j<n && (z[j]&~0x20)==zKW[j] ){ j++; }\n");
  printf("#endif\n");
  printf("#ifdef SQLITE_EBCDIC\n");
  printf("    if( toupper(z[0])!=zKW[0] ) continue;\n");
  printf("    if( toupper(z[1])!=zKW[1] ) continue;\n");
  printf("    j = 2;\n");
  printf("    while( j<n && toupper(z[j])==zKW[j] ){ j++; }\n");
  printf("#endif\n");
  printf("    if( j<n ) continue;\n");
  for(i=0; i<nKeyword; i++){
    printf("    testcase( i==%d ); /* %s */\n",
           i+1, aKeywordTable[i].zOrigName);
  }
  printf("    *pType = aKWCode[i];\n");
  printf("    break;\n");
  printf("  }\n");
  printf("  return n;\n");
  printf("}\n");
  printf("int sqlite3KeywordCode(const unsigned char *z, int n){\n");
  printf("  int id = TK_ID;\n");
  printf("  if( n>=2 ) keywordCode((char*)z, n, &id);\n");
  printf("  return id;\n");
  printf("}\n");
  printf("#define SQLITE_N_KEYWORD %d\n", nKeyword);
  printf("int sqlite3_keyword_name(int i,const char **pzName,int *pnName){\n");
  printf("  if( i<0 || i>=SQLITE_N_KEYWORD ) return SQLITE_ERROR;\n");
  printf("  i++;\n");
  printf("  *pzName = zKWText + aKWOffset[i];\n");
  printf("  *pnName = aKWLen[i];\n");
  printf("  return SQLITE_OK;\n");
  printf("}\n");
  printf("int sqlite3_keyword_count(void){ return SQLITE_N_KEYWORD; }\n");
  printf("int sqlite3_keyword_check(const char *zName, int nName){\n");
  printf("  return TK_ID!=sqlite3KeywordCode((const u8*)zName, nName);\n");
  printf("}\n");

  return 0;
}

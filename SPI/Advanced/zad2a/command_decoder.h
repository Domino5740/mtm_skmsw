#define MAX_TOKEN_NR 2
#define MAX_KEYWORD_NR 4
#define MAX_KEYWORD_STRING_LTH 20

typedef enum TokenType {KEYWORD, NUMBER, STRING} TokenType;
typedef enum KeywordCode {ID, CALIB, GOTO, STEP} KeywordCode;

typedef union TokenValue {
    enum KeywordCode eKeyword;
    unsigned int uiNumber;
    char * pcString;
} TokenValue;

typedef struct Token {
    enum TokenType eType;
    union TokenValue uValue;
} Token;

typedef struct Keyword {
    enum KeywordCode eCode;
    char cString[MAX_KEYWORD_STRING_LTH + 1];
} Keyword;

void DecodeMsg(char *pcString);
unsigned char ucFindTokensInString(char *pcString);
enum Result eStringToKeyword (char pcStr[], enum KeywordCode *peKeywordCode);
void DecodeTokens(void);

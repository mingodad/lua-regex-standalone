#include "sqlite3.h"
#include "lua-regex.h"

static void sqlite3_regexp( sqlite3_context *context, int argc, sqlite3_value **argv ) {
    assert(argc == 2);
    LuaMatchState ms;
    const char *subject = (const char *)sqlite3_value_text(argv[1]);
    const char *pattern = (const char *)sqlite3_value_text(argv[0]);
    assert(argc == 2);
    if(str_find(&ms, subject, strlen(subject), pattern, strlen(pattern), 0, 0))
        sqlite3_result_int(context, 1);
    else
    {
        if(ms.error) sqlite3_result_error(context, ms.error, strlen(ms.error));
        else sqlite3_result_int(context, 0);
    }
}

static void sqlite3_regexp_gsub( sqlite3_context *context, int argc, sqlite3_value **argv ) {
	assert(argc == 4);
	char_buffer_st *result;
	const char *error_ptr;
	const char *subject = (const char *)sqlite3_value_text(argv[0]);
	const char *pattern = (const char *)sqlite3_value_text(argv[1]);
	const char *replacement = (const char *)sqlite3_value_text(argv[2]);
	int ntimes = sqlite3_value_int(argv[3]);
	if(ntimes < 0) ntimes = 0;

	if(!subject || !pattern || !replacement){
		sqlite3_result_null(context);
		return;
	}
	result = str_gsub(subject, strlen(subject), pattern, strlen(pattern),
					  replacement, ntimes, &error_ptr);
	if(result){
		sqlite3_result_text(context, result->buf, result->used, SQLITE_TRANSIENT);
		free(result);
	}
	else sqlite3_result_error(context, error_ptr, strlen(error_ptr));
}

int set_sqlite3_regexp_func(sqlite3 *db)
{
  int nErr = sqlite3_create_function(db,  "regexp", 2, SQLITE_UTF8, 0, sqlite3_regexp, 0, 0);
  nErr += sqlite3_create_function(db,  "match", 2, SQLITE_UTF8, 0, sqlite3_regexp, 0, 0);
	nErr += sqlite3_create_function(db,  "gsub", 4, SQLITE_UTF8, 0, sqlite3_regexp_gsub, 0, 0);
	return nErr ? SQLITE_ERROR : SQLITE_OK;
}

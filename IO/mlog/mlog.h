/*
* NAME: Copyright (c) 2020, Biren Patel
* DESC: Minimal logging library for C99+
* LISC: MIT License
*
*
*         Thread ID                       File   Func     Level
*            |                              |     |         |
*            |                              |     |         |
*            |                              |     |         |
*     123@4567890 Mon Jan 1 01:01:01 1990 mlog.c:main:123 TRACE hello, world!
*      |          \_____________________/              |        \___________/
*      |                     |                         |              |
*      |                     |                         |              |
*     PID                Datetime                     Line         Message
*
*
*/

#ifndef MLOG_H
#define MLOG_H

#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
* Compiler Checks
*******************************************************************************/

#ifndef __GNUC__
    #error "mlog.h requires GNU C Compiler"
#endif

#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 199901L
        #define MLOG_STD99_PLUS
    #endif
#endif

#ifndef MLOG_STD99_PLUS
    #error "mlog.h requires C99+"
#endif

/*******************************************************************************
* API
*******************************************************************************/

//mLog core methods
#define mLogFatal(msg, ...)
#define mLogError(msg, ...)
#define mLogWarn(msg, ...)
#define mLogInfo(msg, ...)
#define mLogDebug(msg, ...)
#define mLogTrace(msg, ...)

//defines for log and flush configurations
#define MLOG_NONE   0
#define MLOG_FATAL  1
#define MLOG_ERROR  2
#define MLOG_WARN   3
#define MLOG_INFO   4
#define MLOG_DEBUG  5
#define MLOG_TRACE  6

//defines for thread safety configurations
#define MLOG_THREAD_SAFE    0
#define MLOG_THREAD_UNSAFE  1

//mLog service start
//if name or mode is null, fp will be used as an already-open stream.
//if fp is null, name and mode will be used to open a new stream.
//if none are null, freopen(name, mode, fp) will be invoked.
// @ cbk : hook for any MLOG_ERROR_CODES that mLog methods may encouter.
int mLogOpen(const char *name, const char *mode, FILE *fp, void (*cbk) (int c));

//mLog service stop: if close is true, then the file stream will also be closed
int mLogClose(bool close);

//get verbal descriptions of MLOG_ERROR_CODES for logging, callbacks, etc.
const char *mLogLookupError(int error);

//all error codes that any API method can emit and send to the callback
enum MLOG_ERROR_CODES
{
    MLOG_SUCCESS            = 0,
    MLOG_BAD_INPUT          = 1,
    MLOG_WRITE_FAIL         = 2,
    MLOG_FPRINTF_FAIL       = 3,
    MLOG_VFPRINTF_FAIL      = 4,
    MLOG_FOPEN_FAIL         = 5,
    MLOG_FREOPEN_FAIL       = 6,
    MLOG_FCLOSE_FAIL        = 7,
    MLOG_FFLUSH_FAIL        = 8,
    MLOG_TIME_FAIL          = 9,
    MLOG_UNDEFINED_ERROR    = 10
};

/*******************************************************************************
* Configurations
*******************************************************************************/

//log levels greater than set value are reduced to zero-cost no-ops
#define MLOG_LOG_LEVEL MLOG_TRACE

//log levels greater than set value do not trigger immediate output buffer flush
#define MLOG_FLUSH_LEVEL MLOG_TRACE

//enable when multiple threads must concurrently access the logging facility
#define MLOG_THREAD_SAFETY MLOG_THREAD_UNSAFE

/*******************************************************************************
* Internals 
*******************************************************************************/

//@ fp : write target
//@ callback : hook for any internal errors/success that mLog may encouter
struct
{
    FILE *fp;
    void (*callback) (int error);
} mlog_obj;

//primary func for requesting log writes
//not to be used directly by the end-user (see mLog core methods)
__attribute__((__format__(__printf__, 5, 6)))
int mLogSend(
    int level,
    int ln,
    const char *fn,
    const char *fx,
    const char *msg,
    ...
);

//mLog core methods
//qualifying NOP defines are overwritten to wrap over mLogSend
#define MLOG_PARAMS __LINE__, __FILE__, __func__

#if MLOG_LOG_LEVEL >= MLOG_FATAL
#undef mLogFatal
#define mLogFatal(msg, ...) mLogSend(MLOG_FATAL, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#if MLOG_LOG_LEVEL >= MLOG_ERROR
#undef mLogError
#define mLogError(msg, ...) mLogSend(MLOG_ERROR, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#if MLOG_LOG_LEVEL >= MLOG_WARN
#undef mLogWarn
#define mLogWarn(msg, ...) mLogSend(MLOG_WARN, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#if MLOG_LOG_LEVEL >= MLOG_INFO
#undef mLogInfo
#define mLogInfo(msg, ...) mLogSend(MLOG_INFO, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#if MLOG_LOG_LEVEL >= MLOG_DEBUG
#undef mLogDebug
#define mLogDebug(msg, ...) mLogSend(MLOG_DEBUG, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#if MLOG_LOG_LEVEL >= MLOG_TRACE
#undef mLogTrace
#define mLogTrace(msg, ...) mLogSend(MLOG_TRACE, MLOG_PARAMS, msg, ##__VA_ARGS__)
#endif

#endif

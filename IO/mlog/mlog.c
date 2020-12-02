/*
* NAME: Copyright (c) 2020, Biren Patel
* DESC: Implementation for mLog API
* LISC: MIT License
*/

#include "mlog.h"

#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

/*******************************************************************************
Local tracing and debug logging to stderr. mLog itself needs some mechanism to
trace itself for debugging purposes so it falls back onto a simple preprocessor 
switch.
*/

#define LOCAL_TRACE 0

#if LOCAL_TRACE == 0
    #define TRACE(msg, ...) /* NOP */
#else
    #define TRACE(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)
#endif

/*******************************************************************************
Dummy callback for propogating mLog errors. Used as a NOP placeholder when user
doesn't provide a callback on mLogOpen. Allows for a bit tidier source.
*/

void mLogDummyCallback(int error __attribute__((unused)))
{
    return;
}

/******************************************************************************/

static const char *error_LU[MLOG_UNDEFINED_ERROR] =
{
    [MLOG_SUCCESS]      = "no error found",
    [MLOG_BAD_INPUT]    = "function arguments are invalid",
    [MLOG_WRITE_FAIL]   = "cannot write to file specified on mLogOpen",
    [MLOG_FOPEN_FAIL]   = "cannot open specified file using stdio fopen",
    [MLOG_FREOPEN_FAIL] = "cannot reopen specified file using stdio freopen",
    [MLOG_FCLOSE_FAIL]  = "cannot close specified file using stdio fclose",
    [MLOG_FFLUSH_FAIL]  = "cannot flush buffer to file specified on mLogOpen",
    [MLOG_TIME_FAIL]    = "cannot fetch current time"
};

/******************************************************************************/

static const char *level_LU[MLOG_TRACE + 1] = 
{
    [MLOG_NONE]     = "NONE" ,
    [MLOG_FATAL]    = "FATAL",
    [MLOG_ERROR]    = "ERROR",
    [MLOG_WARN]     = "WARN" ,
    [MLOG_INFO]     = "INFO" ,
    [MLOG_DEBUG]    = "DEBUG",
    [MLOG_TRACE]    = "TRACE"
};

/*******************************************************************************
mLogOpen: prepare the file stream linked to the global log var. For flexibility,
the file could be already open, or may need to be re-opened. The end user will
indicate this through a specific combination of NULL input pointers.
*/

#define OPEN_FMT "mlog service start: %s" /*ctime has second line feed*/

int mLogOpen(const char *name, const char *mode, FILE *fp, void (*cbk) (int c))
{
    TRACE("entering mLogOpen\n");
    
    TRACE("configuring calback\n");
    if (!cbk)
    {
        TRACE("using dummy callback\n");
        mlog_obj.callback = mLogDummyCallback;
    }
    else
    {
        mlog_obj.callback = cbk;
    }
    
    int error = MLOG_UNDEFINED_ERROR;
    
    //fp is available but name or mode is not
    if ((!name || !mode) && fp)
    {
        TRACE("fp available, name or mode is not\n");
        
        mlog_obj.fp = fp;
        
        TRACE("file pointer set with success\n");
    }
    //name and mode available but fp is not
    else if (name && mode && !fp)
    {
        TRACE("name and mode available, fp is not\n");
        mlog_obj.fp = fopen(name, mode);
        
        if (!mlog_obj.fp) 
        {
            mlog_obj.callback(MLOG_FOPEN_FAIL);
            return MLOG_FOPEN_FAIL;
        }
        
        TRACE("fopen success\n");
    }
    //name, mode, and fp are all available
    else if (name && mode && fp)
    {
        TRACE("name, mode, and fp all available\n");
        mlog_obj.fp = freopen(name, mode, fp);
        
        if (!mlog_obj.fp) 
        {
            TRACE("file pointer is null\n");
            mlog_obj.callback(MLOG_FREOPEN_FAIL);
            return MLOG_FREOPEN_FAIL;
        }
        
        TRACE("freopen success\n");
    }
    else
    {
        TRACE("exiting mLogOpen with bad input\n");
        return MLOG_BAD_INPUT;
    }
    
    TRACE("sending open time to log file\n");
    time_t now = time(NULL);
    
    if (now < 0) 
    {
        mlog_obj.callback(MLOG_TIME_FAIL);
        return MLOG_TIME_FAIL;
    }
    
    error = fprintf(mlog_obj.fp, OPEN_FMT, ctime(&now));
    
    if (error < 0) 
    {
        mlog_obj.callback(MLOG_FPRINTF_FAIL);
        return MLOG_FPRINTF_FAIL;
    }
    
    TRACE("exiting mLogOpen with success\n");
    
    mlog_obj.callback(MLOG_SUCCESS);
    
    return MLOG_SUCCESS;
}

/*******************************************************************************
mLogClose: resource cleanup and managment of mlog_obj structure and associated
file streams.
*/

#define CLOSE_FMT "mlog service stop: %s\n"

int mLogClose(bool close)
{
    TRACE("entering mLogClose\n");
    int error = MLOG_UNDEFINED_ERROR;
    
    TRACE("sending close time to log file\n");
    
    time_t now = time(NULL);
    
    if (now < 0) 
    {
        mlog_obj.callback(MLOG_TIME_FAIL);
        return MLOG_TIME_FAIL;
    }
    
    error = fprintf(mlog_obj.fp, CLOSE_FMT, ctime(&now));
    
    if (error < 0)
    {
        mlog_obj.callback(MLOG_FPRINTF_FAIL);
        return MLOG_FPRINTF_FAIL;
    }
    
    if (close)
    {
        TRACE("closing mlog_obj file pointer\n");
        if (fclose(mlog_obj.fp) == EOF) 
        {
            mlog_obj.callback(MLOG_FCLOSE_FAIL);
            return MLOG_FCLOSE_FAIL;
        }
    }
    
    TRACE("exiting mLogClose with success\n");
    
    mlog_obj.callback(MLOG_SUCCESS);
    return MLOG_SUCCESS;
}

/*******************************************************************************
mLogSend: The core of the mLog facility. Construct a message from the variadic
input and send to the stream designated on mLogOpen. Each log entry consists of
four main parts: 
*/

#define PID_FMT (long) getpid()
#define TID_TMD (uintptr_t) pthread_self()
#define ID_FMT "%ld@%" PRIXPTR " "

#define FMT "%.24s %s:%s:%d %s "

int mLogSend
(
    int level,
    int ln,
    const char *fn,
    const char *fx,
    const char *msg,
    ...
)
{
    TRACE("entering mLogSend at severity %d\n", level);
    
    va_list args;
    va_start(args, msg);
    
    int error = MLOG_UNDEFINED_ERROR;
    
    TRACE("fetching current time\n");
    time_t now = time(NULL);
    if (now < 0) 
    {
        mlog_obj.callback(MLOG_TIME_FAIL);
        return MLOG_TIME_FAIL;
    }
    
    TRACE("creating identifier\n");
    error = fprintf(mlog_obj.fp, ID_FMT, PID_FMT, TID_TMD);
    if (error < 0) 
    {
        mlog_obj.callback(MLOG_FPRINTF_FAIL);
        return MLOG_FPRINTF_FAIL;
    }
    
    TRACE("creating prefix\n");
    error = fprintf(mlog_obj.fp, FMT, ctime(&now), fn, fx, ln, level_LU[level]);
    if (error < 0) 
    {
        mlog_obj.callback(MLOG_FPRINTF_FAIL);
        return MLOG_FPRINTF_FAIL;
    }
    
    TRACE("creating suffix\n");
    error = vfprintf(mlog_obj.fp, msg, args);
    if (error < 0) 
    {
        mlog_obj.callback(MLOG_VFPRINTF_FAIL);
        return MLOG_VFPRINTF_FAIL;
    }
    
    TRACE("terminating line\n");
    error = fprintf(mlog_obj.fp, "\n");
    if (error < 0) 
    {
        mlog_obj.callback(MLOG_WRITE_FAIL);
        return MLOG_WRITE_FAIL;
    }
    
    if (level <= MLOG_FLUSH_LEVEL)
    {
        TRACE("flushing buffer\n");
        
        error = fflush(mlog_obj.fp);
        if (error == EOF) 
        {
            mlog_obj.callback(MLOG_FFLUSH_FAIL);
            return MLOG_FFLUSH_FAIL;
        }
        
        TRACE("flush successful\n");        
    }
    
    va_end(args);
    
    TRACE("exiting mLogSend with success\n");
    
    mlog_obj.callback(MLOG_SUCCESS);
    return MLOG_SUCCESS;
}

/******************************************************************************/

const char *mLogLookupError(int error)
{
    TRACE("entering error lookup\n");
    if (error >= MLOG_SUCCESS && error < MLOG_UNDEFINED_ERROR)
    {
        TRACE("found error in lookup\n");
        return error_LU[error];
    }
    
    TRACE("error does not exist\n");
    return "\0";
}

/*************************************************************************
    > File Name     : log.h
    > Author        : libingli
    > Created Time  : Mon 17 Aug 2015 10:28:18 PM CST
 ************************************************************************/
#ifndef _LOG_H_
#define _LOG_H_

enum log_level{
    LOG_EMERG,  //紧急情况，需要立即通知技术人员。
    LOG_ALERT,  //应该被立即改正的问题，如系统数据库被破坏，ISP连接丢失。
    LOG_CRIT,   //重要情况，如硬盘错误，备用连接丢失。
    LOG_ERR,    //错误，不是非常紧急，在一定时间内修复即可。
    LOG_WARN,   //警告信息，不是错误，比如系统磁盘使用了85%等。
    LOG_NOTICE, //不是错误情况，也不需要立即处理。
    LOG_INFO,   //情报信息，正常的系统消息，比如骚扰报告，带宽数据等，不需要处理。
    LOG_DEBUG,  //包含详细的开发情报的信息，通常只在调试一个程序时使用。
};

#define log_emerg(format, args...)     \
    log_output(LOG_EMERG, format, ##args)

#define log_err(format, args...)     \
    log_output(LOG_ERR, format, ##args)

#define log_warn(format, args...)     \
    log_output(LOG_WARN, format, ##args)

#define log_info(format, args...)    \
    log_output(LOG_INFO, format, ##args)

#define log_dbg(format, args...)     \
    log_output(LOG_DEBUG, format, ##args)

void log_output(int level, char *format, ...);

void log_init(void);
void log_finish(void);

#endif /* _LOG_H_ */


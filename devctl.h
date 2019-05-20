#ifndef __devctl_h
#define __devctl_h

/* The following prototypes are taken from sys/sys/bus.h */

/**
 * devctl hooks.  Typically one should use the devctl_notify
 * hook to send the message.  However, devctl_queue_data is also
 * included in case devctl_notify isn't sufficiently general.
 */
boolean_t devctl_process_running(void);
void devctl_notify_f(const char *__system, const char *__subsystem,
    const char *__type, const char *__data, int __flags);
void devctl_notify(const char *__system, const char *__subsystem,
    const char *__type, const char *__data);
void devctl_queue_data_f(char *__data, int __flags);
void devctl_queue_data(char *__data);
struct sbuf;
void devctl_safe_quote_sb(struct sbuf *__sb, const char *__src);

#endif

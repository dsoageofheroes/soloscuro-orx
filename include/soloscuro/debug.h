#ifndef SOLOSUCRO_DEBUG_H
#define SOLOSUCRO_DEBUG_H

#ifdef DEBUG
#define  debug(fmt, ...) sol_debug(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
#else
#       define debug(...)
#endif

#define info(fmt, ...) printf((fmt), ##__VA_ARGS__);
#define warn(fmt, ...) printf(("[%s:%d] %s: " fmt), __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);
#define error(fmt, ...) fprintf(stderr, ("[%s:%d] %s: " fmt), __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);
#define fatal(fmt, ...) fprintf(stderr, ("FATAL [%s:%d] %s: " fmt), __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);exit(1);

#endif

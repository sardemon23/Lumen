#ifndef MODULE_H
#define MODULE_H

typedef void (*module_entry_t)(void);

typedef struct Module {
    const char *name;
    module_entry_t start;
} Module;

#define MODULE_REGISTER(name, fn) \
    void fn(void); \
    __attribute__((section(".modules"))) Module __module_##fn = { name, fn }


extern const Module __start_modules[];
extern const Module __stop_modules[];

#endif

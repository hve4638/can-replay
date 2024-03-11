#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cargs.h"
#define NEXTARGS(CARGS) nextarg(CARGS, __FILE__, __LINE__)

struct optinfo;
struct funcinfo;

struct cargs {
    int index;
    int argc;
    const char** argv;

    OPTFUNC(argcall);
    struct optinfo* opt_first;
    struct optinfo* opt_last;
};

// linked-list 형식으로 이어집니다
// allocated는 구조체 제거시 keyword를 free해야 하는지 여부를 나타냅니다
// *주의* 한 cargs 내의 optinfo는 한번에 제거되어야 하며 일부를 제거하고 나머지를 재사용하면 안됩니다
// allocated가 true인 keyword 필드는 다른 optinfo의 keyword에서 사용하고 있을 수 있습니다
// ex가 0이라면, keyword는 첫 인덱스의 값만 사용해야 합니다
// ex가 1이라면, keyword를 문자열로 사용해야 합니다
struct optinfo {
    int ex;
    int allocated;
    int argc;
    void(*func)(int, const char**);
    char* keyword;
    struct optinfo* next;
};

static void callwithargs(struct cargs* cargs, int argc, void (*func)(int, const char**));
static void cargs_makeandadd_opt(struct cargs* cargs, char* keyword, int length, int allocated, int argc, OPTFUNC(func));

static const char* nextarg(struct cargs* cargs, const char* file, int line);
static void cargs_add_opt(struct cargs*, struct optinfo*);
static void optinfo_free_recursively(struct optinfo*);
static struct optinfo* findopt(struct cargs* cargs, const char keyword);
static struct optinfo* findopt_ex(struct cargs* cargs, const char* keyword);

static int startwith(const char* source, const char* substr) {
    int i = 0;
    while (1) {
        if (substr[i] == '\0') return 1;
        else if (source[i] == '\0') return 1;
        else if (source[i] != substr[i]) return 0;
        i++;
    }
}
static int cargs_eoa(struct cargs* cargs) {
    return (cargs->index >= cargs->argc);
}

struct cargs* cargs_init() {
    struct cargs* args = (struct cargs*)malloc(sizeof(struct cargs));
    args->opt_first = NULL;
    args->opt_last = NULL;
    args->argcall = NULL;

    return args;
}

void cargs_option(struct cargs* cargs, const char* keywords, int argc, OPTFUNC(func), int allow_duplicate) {
    int i = 0, count = 0;
    char *str = strdup(keywords);
    char *p = str;
    int allocated = 1;
    while(1) {
        if (str[i] == '\0') {
            cargs_makeandadd_opt(cargs, p, count, allocated, argc, func);
            break;
        }
        else if (str[i] == ':') {
            str[i] = '\0';
            cargs_makeandadd_opt(cargs, p, count, allocated, argc, func);
            p = str + i + 1;
            allocated = 0;
            count = -1;
        }
        i++; count++;
    }
}

void cargs_makeandadd_opt(struct cargs* cargs, char* keyword, int length, int allocated, int argc, OPTFUNC(func)) {
    int ex;
    
    if (length == 0) {
        fprintf(stderr, "warning : keyword error in cargs_option()\n");
    }
    else {
        ex = (length == 1) ? 0 : 1;

        struct optinfo* info = (struct optinfo*)malloc(sizeof(struct optinfo));
        info->keyword = keyword;
        info->ex = ex;
        info->allocated = allocated;
        info->argc = argc;
        info->func = func;
        info->next = NULL;
        
        cargs_add_opt(cargs, info);
    }
}

void cargs_add_opt(struct cargs* cargs, struct optinfo* info) {
    if (cargs->opt_first == NULL) {
        cargs->opt_first = info;
        cargs->opt_last = info;
    }
    else {
        cargs->opt_last->next = info;
        cargs->opt_last = info;
    }
}

void cargs_args(struct cargs* cargs, OPTFUNC(call)) {
    cargs->argcall = call;
}

void cargs_run(struct cargs* cargs, int argc, const char** argv) {
    int isopt, isoptex, isnormal;
    struct optinfo *opt;
    
    cargs->index = 1;
    cargs->argc = argc;
    cargs->argv = argv;
    OPTFUNC(call) = cargs->argcall;

    const char** normalarg = NULL;
    int normal_count = 0;
    int normal_cut = 0;

    const char* text;
    while (!cargs_eoa(cargs)) {
        text = NEXTARGS(cargs);
        isopt = startwith(text, "-");
        isoptex = startwith(text, "--");
        isnormal = 0;
        
        if (isopt || isoptex) {
            if (isoptex) {
                opt = findopt_ex(cargs, text+2);
            }
            else {
                opt = findopt(cargs, text[1]);
            }
            
            if (opt == NULL) {
                isnormal = 1;
            }
            else {
                if (normal_count > 0) normal_cut = 1;

                callwithargs(cargs, opt->argc, opt->func);
            }
        }
        else {
            isnormal = 1;
        }

        if (isnormal) {
            if (normal_cut) {
                fprintf(stderr, "Error : args parse fail\n");
                exit(-1);
            }
            if (normal_count == 0) {
                normalarg = cargs->argv + cargs->index-1;
            }
            normal_count++;
        }
    }

    if (call != NULL) {
        call(normal_count, normalarg);
    }
}

void cargs_close(struct cargs* cargs) {
    optinfo_free_recursively(cargs->opt_first);
}

void optinfo_free_recursively(struct optinfo* info) {
    struct optinfo* current;
    struct optinfo* next;

    current = info;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

const char* nextarg(struct cargs* cargs, const char* file, int line) {
    if (cargs->index < cargs->argc) {
        return cargs->argv[cargs->index++];
    }
    else {
        fprintf(stderr, "[%s:%d] fatal error : out of range\n", file, line);
        exit(-1);
    }
}

struct optinfo* findopt(struct cargs* cargs, const char keyword) {
    struct optinfo *info = cargs->opt_first;
    while (info != NULL) {
        if (!info->ex && info->keyword[0] == keyword) {
            return info;
        }
        info = info->next;
    }
    return NULL;
}

struct optinfo* findopt_ex(struct cargs* cargs, const char* keyword) {
    struct optinfo *info = cargs->opt_first;
    while (info != NULL) {
        if (info->ex && strcmp(info->keyword, keyword) == 0) {
            return info;
        }
        info = info->next;
    }
    return NULL;
}

void callwithargs(struct cargs* cargs, int argc, void (*func)(int, const char**)) {
    const char** p = cargs->argv + cargs->index;

    cargs->index += argc;
    if (cargs->index > cargs->argc) {
        fprintf(stderr, "[%s:%d] fatal error : out of range\n", __FILE__, __LINE__);
        exit(-1);
    }
    
    func(argc, p);
}
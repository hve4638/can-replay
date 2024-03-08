#ifndef CARGS_H
#define CARGS_H
#define OPTFUNC(F) void(*F)(int, const char**)
#define OPTFUNC_TYPE void*(int, const char**)
#include <stdio.h>

struct cargs;

// cargs 생성
struct cargs* cargs_init();

// 인자의 옵션 처리
// 옵션으로 처리할 키워드를 지정하고 함수를 지정합니다
// keywords : 옵션 키워드를 지정합니다
//      여러 옵션 키워드를 넣을 수 있으며 :로 구분합니다
//      한글자 키워드는 -, 두글자 키워드는 --로 시작합니다
//      keyword 예제 : "v:verbose" 는 -v, --verbose 옵션을 처리합니다
// argc : 옵션을 처리하는 함수에 넘겨주는 인자의 개수입니다
// func : 옵션을 처리하는 void(*)(int, const char**) 포맷의 함수 포인터입니다
//      함수내 첫번째 int형 매개변수는 argc 고정입니다
//      옵션 처리시 넘겨줄 충분한 인자가 없다면 에러가 발생합니다
// allow_duplicate : (미구현됨)
//      1이라면 옵션을 여러번 처리할 수 있습니다
//      0이라면 옵션이 여러번 처리되는 것을 허용하지 않습니다
void cargs_option(struct cargs* cargs, const char* keywords, int argc, OPTFUNC(call), int allow_duplicate);

// 옵션으로 처리되지 않는 인자(=남은 인자)를 받아오는 함수를 지정합니다
// 남은 인자는 연속되어야 합니다
// 남은 인자 사이 옵션이 처리되면 에러가 발생합니다
void cargs_args(struct cargs* cargs, OPTFUNC(call));

// 인자 처리 작업을 수행합니다
// 인자에 나타나는 옵션의 순서대로 옵션 처리 함수가 실행됩니다
// 모든 옵션 처리 함수가 끝난 다음 남은 인자를 처리하는 함수를 실행합니다
// 남은 인자가 0개라도 남은 인자를 처리하는 함수를 수행합니다
void cargs_run(struct cargs* cargs, int argc, const char** argv);

// cargs 정리
void cargs_close(struct cargs* cargs);

#endif
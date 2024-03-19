#include "color.h"

namespace color {
    const char* red = "\033[1;31m";
    const char* green = "\033[1;32m";
    const char* yellow = "\033[1;33m";
    const char* blue = "\033[34m";
    const char* magenta = "\033[35m";
    const char* cyan = "\033[36m";
    const char* white = "\033[37m";
    const char* reset = "\033[0m";

    namespace text {
        const char* red = "\033[1;31m";
        const char* green = "\033[1;32m";
        const char* yellow = "\033[1;33m";
        const char* blue = "\033[34m";
        const char* magenta = "\033[35m";
        const char* cyan = "\033[36m";
        const char* white = "\033[37m";
        const char* reset = "\033[0m";
    }
    
    namespace bg {
        const char* red = "\033[1;41m";
        const char* green = "\033[1;42m";
        const char* yellow = "\033[1;43m";
        const char* blue = "\033[44m";
        const char* magenta = "\033[45m";
        const char* cyan = "\033[46m";
        const char* white = "\033[47m";
    }
}
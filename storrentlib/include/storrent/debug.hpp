#pragma once

#if defined(TRACE)
    #include <iostream>
    #define TRACE_OUT(x) std::cout << x << std::endl
#else
    #define TRACE_OUT((void)(x))
#endif

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <cassert>

#ifdef DEBUG
#define Assert(cond, exp)                                                     \
    {                                                                         \
        assert (cond);                                                        \
    }
#else
#define Assert(cond, exp)                                                     \
    {                                                                         \
    }
#endif

#define AssertIndexRange(index, range)                                        \
    {                                                                         \
        Assert (index < range, "Index out of range!");                        \
    }

#endif
#ifndef _ERRORS_H
#define _ERRORS_H

#pragma once

#include <assert.h>

#define CHECK_NULL_RETURN(X, RET) if (!(X)) { return(RET); }
#define ASSERT_VALUE(X, VALUE) assert(X == VALUE);

#endif
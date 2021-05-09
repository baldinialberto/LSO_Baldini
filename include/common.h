#ifndef _COMMON_H
#define _COMMON_H

#pragma once

typedef char bool;

#ifdef DEBUG
#define DEBUG(X) X
#endif
#ifndef DEBUG
#define DEBUG(X)
#endif

#endif
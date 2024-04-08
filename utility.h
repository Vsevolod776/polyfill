/*
 * VsevolodWTF 2023
 */
#ifndef _UTILITY_H
#define _UTILITY_H

#include <type_traits>
#include <exception>

struct in_place_t {};
static constexpr in_place_t in_place;

struct nullopt_t {};
static constexpr nullopt_t nullopt;

struct unexpect_t {};
static constexpr unexpect_t unexpect;

#endif

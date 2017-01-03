hebimath - arbitrary precision arithmetic library
-------------------------------------------------------------------------------
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/294421b48f7241c1940f0c260633215e)](https://www.codacy.com/app/suigin/hebimath?utm_source=github.com&utm_medium=referral&utm_content=suiginsoft/hebimath&utm_campaign=badger)
[![Build Status](https://travis-ci.org/suiginsoft/hebimath.svg?branch=master)](https://travis-ci.org/suiginsoft/hebimath)
[![Coverity Scan](https://img.shields.io/coverity/scan/11311.svg)](https://scan.coverity.com/projects/suiginsoft-hebimath)
[![License](https://img.shields.io/packagist/l/doctrine/orm.svg)](https://github.com/suiginsoft/hebimath/raw/master/LICENSE)

What is Hebimath?
===============================================================================

**ヘビー 【Hebī】**

1. *Heavy. Big. Huge. Of great weight; hard to lift or carry: "A heavy load."*
2. *Extreme. Intense. Scary. Tough. "A heavy drinker."*
3. *Applying power; a final burst of energy.*

**ヘビ 【Hebi】**

1. *Snake. Serpent. Reptile. "Best watch out for that SICP snake!"*

In short, Hebimath is a library capable of performing bignum arithmetic in the
same vein as GMP, libtommath, TomsFastMath, MPI and friends.

Hebimath has the following design features:

- Written in C99 and assembly language for POSIX userspace platforms.
- Low-level maths kernel functions that operate on natural numbers, optimized
  for specific hardware platforms, as well as a generic version written in C.
  Supports both compile-time and runtime function multi-versioning, depending
  on your needs and requirements.
- High-level interface for large integers and rationals, built on top of the
  optimized maths kernels. Handles memory management and returns an error
  code when a memory allocation fails. Furthermore, it correctly handles
  overflows when computing the number of bytes for allocations, unlike some
  of the competitors.
- Ability to override the memory allocator for each number primitive.
- Capacity to override default memory allocator for each thread
- Can use fixed-size buffers instead of an allocator when initializing
  a number primitive, useful for memory allocations on the stack.
- Can specify separate memory allocators for result and temporary buffers.
- Smallest unit size for natural numbers and integers is 16 bytes. This
  larger unit or packet size aids in shrinking the amount of code bloat for
  unrolled kernel functions.
- Memory buffers are correctly aligned for use with SIMD instructions.

Hebimath adheres to these principles in its implementation:

- Be compact, concise and readable. Favor small code, but don't over-do it.
  In the low-level kernel functions, with the exception of the "generic"
  version, favor runtime speed above all else.
- Avoid unnecessary memory allocations, and delay allocations until they are
  absolutely required for output variables.
- Allocate all temporary work memory upfront for algorithms using sharp upper
  bounds on the input argument lengths. No allocations/deallocations at each
  recursive or iterative level.
- Avoid mutating output variables when errors occur, and document what may be
  left in an unspecified state.
- Load values from data structures into local variables if they are used across
  function call boundaries to avoid reloading them more than once. Useful on
  CPUs with lots of callee-saved registers.

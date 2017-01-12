hebimath - arbitrary precision arithmetic library
-------------------------------------------------------------------------------
[![Build Status](https://travis-ci.org/suiginsoft/hebimath.svg?branch=master)](https://travis-ci.org/suiginsoft/hebimath)
[![Coverity Scan](https://img.shields.io/coverity/scan/11311.svg)](https://scan.coverity.com/projects/suiginsoft-hebimath)
[![Quality Gate](https://sonarqube.com/api/badges/gate?key=hebimath)](https://sonarqube.com/dashboard/index/hebimath)
[![License](https://img.shields.io/packagist/l/doctrine/orm.svg)](https://github.com/suiginsoft/hebimath/raw/master/LICENSE)

Overview
===============================================================================

Hebimath is a library for working with big numbers. It's in the same class of
libraries as GNU MP, MPI, TomsFastMath, libzahl and others. As such, Hebimath
shares many features with these libraries, but has some unique ones as well.

Hebimath has the following design features:

- Written in C99 and assembly language for POSIX userspace platforms. Is forward
  compatible with C11.
- High-level interface for large integers, rationals and floating-point numbers.
- Low-level maths kernel functions that operate on natural numbers, optimized
  for common hardware platforms. Supports both compile-time and runtime
  function multi-versioning, depending on your needs and requirements.
- Ability to override the memory allocator for each high-level number primitive.
- Can override the default memory allocator for each thread.
- Can use fixed-size buffers instead of an allocator when initializing
  a primitive, useful when using statically allocated memory or fast ad-hoc
  memory allocations on the stack.
- Can setup a separate allocator for internal temporary/scratchpad allocations.
- Smallest unit size for natural numbers and integers is 16 bytes. This
  larger unit size aids in shrinking the amount of code bloat for unrolled
  kernel functions.
- Memory buffers are correctly aligned for use with SIMD instructions.

Hebimath adheres to these principles in its implementation:

- Be compact, concise and readable. Favor small code but do not over do it.
  In the low-level kernel functions favor runtime speed without sacrificing
  reliability & security best practices.
- Avoid unnecessary memory allocations and delay allocations until they are
  absolutely required for output variables.
- Allocate all temporary memory upfront for algorithms using sharp upper bounds
  on the input argument lengths. No allocations/deallocations at each recursive
  or iterative level.
- Avoid mutating output variables when errors occur and document what may be
  left in an unspecified state.
- Load values from data structures into local variables if they are used across
  function call boundaries to avoid reloading them more than once. Useful on
  CPUs with lots of callee-saved registers.

What's with the name?
===============================================================================

**ヘビ 【Hebi】**

1. *Snake. Serpent. Reptile. "Best watch out for that SICP snake!"*

**ヘビー 【Hebī】**

1. *Heavy. Big. Huge. Of great weight; hard to lift or carry: "A heavy load."*
2. *Extreme. Intense. Scary. Tough. "A heavy drinker."*
3. *Applying power; a final burst of energy.*

[![Hebimath](https://my.mixtape.moe/ovjwcq.png)](https://my.mixtape.moe/onybql.webm "Computing beyond your dreams. Bignums beyond your imagination.")

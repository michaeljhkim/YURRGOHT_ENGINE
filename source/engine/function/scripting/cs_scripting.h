#pragma once
/*
For loading C# code into C++ for modification:
https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting

For loading C# as a scripting language:
https://github.com/oleg-shilo/cs-script


WHY CHOOSE C#:
- Familiar language in the unlikely scenario that someone decides to use this engine
- Easier language to learn and manage for my friends
- It is fast (relative for garbage collecting)
- Uses C-based syntax
- Portable
- implemented for fun and as a personal challenge

WHY C# AS A SCRIPTING LANGUAGE:
- I was a big doom modder around 2020-2021, and I wanted to give a similar experience to people
- It should theoretically be most of the benefits of C#, with no auxillary compile tool required for users
- Trade off is longer startup time, but I think that's worthwhile, especially heavy computation SHOULD be done in C++

NOTES:
- Hopefully auxilliary C++ code will be available for use as well, perhaps interchangeably?
- For now, must focus on just doing C#
*/

// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// Standard headers
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Manually installed in the engine cmake file
#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include <dlfcn.h>
#include <limits.h>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#define string_compare strcmp

using string_t = std::basic_string<char_t>;


namespace Yurrgoht
{
    // Forward declarations
    bool load_hostfxr(const char_t *app);
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *assembly);
    int run_app_example();

/********************************************************************************************
 * Function used to load and activate .NET Core
 ********************************************************************************************/
    void *load_library(const char_t *);
    void *get_export(void *, const char *);
}

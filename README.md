# Beacon Object File Visual Studio Template

This repository contains the Beacon Object File Visual Studio (BOF-VS) template project.
You can read more about rationale and design decisions from this blog [post](https://www.cobaltstrike.com/blog/simplifying-bof-development).

## Quick Start Guide

To get started, use the instructions provided below.

### Prerequisites:

* An x64 Windows 10/11 development machine (without a security solution)
* Visual Studio Community/Pro/Enterprise 2022 (Desktop Development with C++ installed)

### Template Installation

Download the latest [release](https://github.com/Cobalt-Strike/bof-vs/releases/latest/download/bof-vs.zip),
and copy the `bof-vs.zip` archive under the 
`%USERPROFILE%\Documents\Visual Studio 2022\Templates\ProjectTemplates` folder.
The template is accessible through Visual Studio's new project dialog,
where you can locate it by searching with the keyword `BOF`. Be certain
to have `All languages` chosen as the language filter.

If Visual Studio does not recognize the template, then reset the project template cache by
deleting the following file: `%localappdata%\Microsoft\VisualStudio\<VS vesrion>\ProjectTemplatesCache_{<GUID>}\cache.bin`

### Debug Build

The `Debug` target builds your BOF to an executable, which allows
you to benefit from the convenience of debugging your BOF code directly within
Visual Studio's built-in debugger. This will enable you to work at the source
code level without running the BOF through a Beacon.

### Release Build

The Release target compiles a release object file of your BOF,
which is designed to be used directly with Cobalt Strike.

## Dynamic Function Resolution

The project template includes two macro definitions to facilitate Dynamic
Function Resolution (DFR) declarations. These macros provide a robust mechanism
for efficiently resolving Win32 API functions in BOFs and simplify the
development process significantly.

### DFR Macro

The `DFR` macro can automatically extract the function type and generate 
the required function declaration.

```cpp

DFR(KERNEL32, OpenProcess)
```

The above `DFR` macro statement expands to the following declaration.

```cpp

DECLSPEC_IMPORT decltype(OpenProcess) KERNEL32$OpenProcess;
```

A common practice is to map the `KERNEL32$OpenProcess` function to OpenProcess
using the following macro definition. This mapping enables you to call the
OpenProcess function directly, eliminating the need for the `KERNEL32$` prefix.

```cpp
#define OpenProcess KERNEL32$OpenProcess
```

#### Example Usage

```cpp
DFR(KERNEL32, OpenProcess)
#define OpenProcess KERNEL32$OpenProcess

void func1() { 
    OpenProcess(...); 
}

void func2() { 
    OpenProcess(...); 
}
```

### DFR_LOCAL Macro

The `DFR_LOCAL(module, function)` macro allows you to define a local function
pointer variable that directly references the `module$function` function. One
of the main advantages of using this macro compared to the `DFR` macro is 
the elimination of the need for the additional `OpenProcess` -> `KERNEL32$OpenProcess` 
mapping. This streamlines the code and makes it more concise. However, it's
important to note that the function pointer created with the `DFR_LOCAL` macro 
has a limited scope and can only be accessed within the function where it is defined.
Consequently, if you plan to use the required WINAPI functions in multiple 
functions throughout your BOF, you will need to define the function pointer 
using the DFR_LOCAL macro in each of those functions.

#### Example Usage

```cpp
void func1() { 
    DFR_LOCAL(KERNEL32, OpenProcess); 
    OpenProcess(...); 
} 

void func2() { 
    DFR_LOCAL(KERNEL32, OpenProcess); 
    OpenProcess(...); 
} 
```

## Mocked APIs

The template includes a mocked version of the Beacon API for Debug builds,
enabling BOF debugging without a running Beacon instance. When you select
either the Debug or UnitTest configuration, the mocked API is automatically
included into the project.

### Argument Packer

The BofData class implements an argument packer to replicate the argument
packing behavior of the bof_pack aggressor function. This enables us to
call BOF's entry point with custom arguments without Beacon. 

```cpp
bof::mock::BofData data; 
// the pack function takes one or more arguments 
data.pack<int, short, int, const char*>(6502, 80, 68010, "Hello World"); 

// alternatively, the << operator can be used to construct the arguments buffer 
data << 0xdeadface << L"Hello World"; 

// raw buffers can be added too 
const char buf[] = { 0x41, 0x42, 0x43, 0x44 }; 
data.addData(buf, sizeof(buf)); 

go(data.get(), data.size()); 
```

### Beacon API

The template also provides a mocked implementation of the Data Parser, Output,
and Format APIs. The mocked functions within the Output API print the output 
to the standard output, ensuring the results are visible. Moreover, all returned
output is stored for future examination and analysis. 

Furthermore, the Internal API functions (such as BeaconUseToken,
BeaconInjectProcess, etc.) are declared. However, it is important to note that
these functions lack the real implementation and only display an error message
on the standard error if called.

## Unit Tests

The project template offers an additional build target called `UnitTest`,
specifically designed to build BOFs with the GoogleTest framework. Furthermore,
the mock library provides a convenient `runMocked` function that handles
the argument packing, execution of the BOF's entry point, and capturing all
generated outputs.

Install the GoogleTest framework:

1. Right-click the project name in Solution Explorer.
2. Select `Manage NuGet Packages`.
3. Ensure that the `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-static` package is installed.

### Example Usage
```cpp
extern "C" { 
    #include "beacon.h" 

    void go(char* args, int len) { 
        datap parser; 
        BeaconDataParse(&parser, args, len); 
        int number = BeaconDataInt(&parser); 
        BeaconPrintf(CALLBACK_OUTPUT, "Hello: %i", number); 
    } 
} 

TEST(ExampleBofTest, TestCase1) { 
    std::vector<bof::output::OutputEntry> actual = 
        bof::runMocked<int>(go, 6502); 

    std::vector<bof::output::OutputEntry> expected = { 
        {CALLBACK_OUTPUT, "Hello: 6502"} 
    }; 

    ASSERT_EQ(expected, actual); 
}
```

## Sleepmask
In addition to supporting standard Beacon Object Files, the template also includes
functionality for developing Sleepmask BOFs. Beacon's Sleepmask can be used to apply
runtime masking to its PE sections and Heap allocations. Therefore, this template
creates a "mock Beacon" as part of the call to runMockedSleepMask() to replicate
the layout of Beacon in memory during debugging. This function also makes it possible
to apply malleable C2 settings to the "mock Beacon".


```c
// Mock up Beacon and run the sleep mask once
bof::runMockedSleepMask(sleep_mask);

// Mock up Beacon with the specific .stage C2 profile
bof::runMockedSleepMask(sleep_mask,
    {
        .allocator = bof::profile::Allocator::VirtualAlloc,
        .obfuscate = bof::profile::Obfuscate::False,
        .useRWX = bof::profile::UseRWX::True,
        .module = "",
    },
    {
        .sleepTimeMs = 5000,
        .runForever = false,
    }
);
```


## Multiple BOFs

The template supports multiple BOFs within separate files, enabling each .cpp
file to be compiled into an individual BOF. This approach eliminates the need
for multiple projects and allows grouping similar BOFs under one Visual Studio
project. When it comes to debugging, since each BOF is compiled into its own
debug executable, it is important to adjust the debug command accordingly.
This can be done through the project properties: `Configuration Properties
-> Debugging -> Command`. 

#include <Windows.h>
#include "base\helpers.h"

/**
 * For the debug build we want:
 *   a) Include the mock-up layer
 *   b) Undefine DECLSPEC_IMPORT since the mocked Beacon API
 *      is linked against the the debug build.
 */
#ifdef _DEBUG
#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT
#include "base\mock.h"
#endif

extern "C" {
#include "beacon.h"
#include "sleepmask.h"

    // Define the Dynamic Function Resolution declaration for the GetLastError function
    DFR(KERNEL32, GetLastError);
    // Map GetLastError to KERNEL32$GetLastError 
    #define GetLastError KERNEL32$GetLastError 

    void go(char* args, int len) {
        /**
         * Define the Dynamic Function Resolution declaration for the GetSystemDirectoryA function
         * This time we use the DFR_LOCAL macro which create a local function pointer variable that
         * points to GetSystemDirectoryA. Therefore, we do have to map GetSystemDirectoryA to
         * KERNEL32$GetSystemDirectoryA
         */
        DFR_LOCAL(KERNEL32, GetSystemDirectoryA);
        char path[MAX_PATH + 1];

        UINT bytesCopied = GetSystemDirectoryA(path, sizeof(path));
        if (bytesCopied == 0) {
            BeaconPrintf(CALLBACK_ERROR, "Error: %i", GetLastError());
        }
        else if (bytesCopied <= sizeof(path)) {
            BeaconPrintf(CALLBACK_OUTPUT, "System Directory: %s", path);
        }
    }

    /*
    void sleep_mask(PSLEEPMASK_INFO info, PFUNCTION_CALL funcCall) {
    }
    */
}

// Define a main function for the bebug build
#if defined(_DEBUG) && !defined(_GTEST)

int main(int argc, char* argv[]) {
    // Run BOF's entrypoint
    // To pack arguments for the bof use e.g.: bof::runMocked<int, short, const char*>(go, 6502, 42, "foobar");
    bof::runMocked<>(go);

    /* To test a sleepmask BOF, the following mockup executors can be used
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
    */

    return 0;
}

// Define unit tests
#elif defined(_GTEST)
#include <gtest\gtest.h>

TEST(BofTest, Test1) {
    std::vector<bof::output::OutputEntry> got =
        bof::runMocked<>(go);
    std::vector<bof::output::OutputEntry> expected = {
        {CALLBACK_OUTPUT, "System Directory: C:\\Windows\\system32"}
    };
    // It is possible to compare the OutputEntry vectors, like directly
    // ASSERT_EQ(expected, got);
    // However, in this case, we want to compare the output, ignoring the case.
    ASSERT_EQ(expected.size(), got.size());
    ASSERT_STRCASEEQ(expected[0].output.c_str(), got[0].output.c_str());
}
#endif
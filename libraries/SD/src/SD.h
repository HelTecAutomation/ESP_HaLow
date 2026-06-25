#pragma once
// Compatibility header.
//
// This core ships the SD library with its implementation header named
// "halow_SD.h". Standard Arduino SD sketches -- and the examples bundled with
// this very library (SD_Test, SD_time) -- use `#include <SD.h>` / `#include
// "SD.h"`, which otherwise fails to resolve here ("SD.h: No such file or
// directory"). This shim makes the standard include work by forwarding to the
// real implementation, so existing sketches and examples compile unchanged.
#include "halow_SD.h"

# core-utils

|Library|fmt (std:fmt in cpp20)|
|---|---|
|link|https://github.com/fmtlib/fmt|
|descr|{fmt} is an open-source formatting library providing a fast and safe alternative to C stdio and C++ iostreams.|

## Logging

#### EMLogger.h
```cpp
#pragma once

#include <logging/Logging.h>

LOG_COMPONENT_SETUP(em, em_logger);
```

#### main.cpp
```asm
#include <logging/Logging.h>
#include "EMLogger.h"

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level="info";
    logger::setup(logProps);


    em::log::info("em::info");
    em::log::warning("em::warn");

    return 0;
}
```

![](https://raw.githubusercontent.com/darvik80/core-utils/master/images/logging.png)
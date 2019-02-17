# PlatformIO howto

https://platformio.org/


List installed platforms

    platformio platform list


List available boards

    platformio boards


Run specific build

    platformio ci --board=megaatmega2560 --lib="." examples/HX711_full_example


Run specific environment

    platformio run --environment lopy4


Build all environments

    platformio run

"Make clean" for all environments

    platformio run -t clean


Dump specific build environment

    platformio run --environment lopy4 --target envdump

See slot `CPPDEFINES`.

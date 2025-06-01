## Building
### Dependencies 
* [Raylib](https://github.com/raysan5/raylib)
* [cwiid](https://github.com/sugol-a/cwiid)
    1. Clone the modified repo to allow build in 2025, python support is disabled as we didn't need it and didn't feel like fixing it.
    2. `aclocal`
    3. `autoconf`
    4. `./configure --without-python`
    5. `make`
    6. `sudo make install`
    This installs the cwiid libs to `/usr/local/lib`. You will need to set the `PKG_CONFIG_PATH` to have the libs. This can be done temporarily with `PKG_CONFIG_PATH=/usr/local/lib/pkgconfig make`
    To run the compiled game binary you will also need to pass `LD_PRELOAD=/usr/local/lib/libcwiid.so.1 ./bin/weeninja`. 
* bluez-libs
### Build
`make` in the root dir of the project. The binary will be in `./bin`
### Run
To use a wiimote pass `YES` when executing the binary. You will have 5 seconds to pair the wiimote otherwise the game will quit.
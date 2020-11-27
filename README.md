# `shuffle3-lean` - Improved 3 stage byte shuffler

Deterministically and reversably shuffle a file's bytes around.

## Shuffling
Shuffle a file in place

``` shell
$ shuffle3 -s file
```

## Unshuffling
Unshuffle a file in place

``` shell
$ shuffle3 -u file
```

## Other options
Run with `-h` for more options.

# Building
Run `make` to build the normal binary. It will output to `shuffle3-release`.

## Release target
The `release` (default) target uses the variables `RELEASE_CFLAGS`, `RELEASE_CXXFLAGS` and `RELEASE_LDFLAGS` to specify opitimisations, as well as the `OPT_FLAGS` variable. These can be set by you if you wish.

### Note
The default `OPT_FLAGS` contains the flag `-march=native`. This may be underisable for you, in which case set the variable or modify the makefile to remove it.

## Debug target
To build with debug information, run `make debug`. Extra debug flags can be provided with the `DEBUG_CFLAGS`, `DEBUG_CXXFLAGS` and `DEBUG_LDFLAGS` variables which have default values in the Makefile.

The build and unstripped binary will be `shuffle3-debug`.

## Notes
Before switching between `release` and `debug` targets, remember to run `make clean`.
To disable stripping of release build binaries, run with `make STRIP=: release`

# License
GPL'd with <3

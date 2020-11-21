# shuffle3 - 3 stage byte shuffler

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
Run with `--help` for more options.

# Building
Run `make` to build the normal binary. It will output to `shuffle3-release`.

## Release target
The `release` (default) target uses the variables `RELEASE_CFLAGS` and `RELEASE_LDFLAGS` to specify opitimisations. These can be set by you if you wish.

### Note
The default `RELEASE_CFLAGS` contains the flag `-march=native`. This may be underisable for you, in which case set the variable or modify the makefile to remove it.

## Debug target
To build with debug information, run `make debug`. Extra debug flags can be provided with the `DEBUG_CFLAGS` and `DEBUG_LDFLAGS` variables which have default values in the Makefile.

The build and unstripped binary will be `shuffle3-debug`.

## Notes
Before switching between `release` and `debug` targets, remember to run `make clean`.

# License
GPL'd with <3

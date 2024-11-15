# Quest on Saer Tor

Inspired by Rimworld, Dwarf Fortress, Songs of Syx, etc.

Tech stack:

* Ninja build runner
* Custom python ninja generator using `ninja_syntax`
* Doctest for testing
* fmt

To do:

* Set up clang-tidy checks?
* cppcheck
* cppclean
* wren or lua?

## Building

If you want to build inside an ephemeral docker container (recommended), make sure to first build the docker image
(requires `podman`):

```sh
./scripts/docker.sh build
```

Then build the third party libraries and the project:

```sh
./scripts/docker.sh ./build.py
```

The build script has more options, more info by passing `--help` as argument.

If you do not want to use the docker, just call the build python script without the docker script prepended (requires
more dependencies installed locally, see the Dockerfile)

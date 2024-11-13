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

First make sure the third party libs are fetched and built:

```sh
./scripts/
```

If you want to build inside an ephemeral docker container (recommended), make sure to first build the docker image:

```sh
./scripts/docker.sh build
```

Then build the third party libraries and the project:

```sh
./scripts/docker.sh ./scripts/setup.sh
./scripts/docker.sh ./build.py [arguments]
```

If you do not want to use the docker, just call the build python script manually (requires set up )

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

if __name__ == "__main__":
    # Standard libs
    from pathlib import Path
    import argparse
    import inspect
    import os
    import sys
    import tomllib

    # 3rd party libs
    from ninja_syntax import Writer as NinjaWriter

    my_path = str(Path(inspect.getsourcefile(lambda: 0)).absolute())
    my_dir = os.path.dirname(my_path)
    my_name = os.path.basename(my_path)
    root = str(Path(my_dir).parent)

    parser = argparse.ArgumentParser(
        prog=my_name, description="What the program does", epilog="Text at the bottom of help"
    )
    parser.add_argument("-i", "--input")
    parser.add_argument("-o", "--output")
    parser.add_argument("-p", "--profile")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()

    if args.verbose:
        print(args)
        print(sys.argv)
        print("Generating ninja build file")

    settings = str(Path(args.input).absolute())
    output = str(Path(args.output).absolute())
    profile = args.profile

    with open(settings, "rb") as f:
        settings_db = tomllib.load(f)

    build_script_file = open(output, "w")
    nw = NinjaWriter(build_script_file, width=120)

    profile_db = {}
    for profile_name, db in settings_db["profiles"].items():
        if profile_name == profile:
            profile_db = db

    all_binaries = []
    for binary_name, binary_db in settings_db["binaries"].items():
        all_binaries += [binary_name]
        ninja_db = {
            "builds": [],
        }
        for file in binary_db["dependencies"]:
            if file.endswith(".c"):
                ninja_db["builds"].append(
                    {
                        "input": f"{root}/{file}",
                        "rule": "cc",
                        "output": f"obj/{file}.o",
                    }
                )

        nw.variable("cc", "gcc")
        nw.variable("cxx", "g++")
        nw.variable("ld", "gcc")
        nw.variable("cflags", " ".join(profile_db["cflags"]))
        nw.variable("cxxflags", " ".join(profile_db["cxxflags"]))
        nw.variable("ldflags", " ".join(profile_db["ldflags"]))

        nw.rule("cc", "$cc -MD -MF $out.d $cflags -c -o $out $in", depfile="$out.d", deps="gcc")
        nw.rule("cxx", "$cxx -MD -MF $out.d $cxxflags -c -o $out $in", depfile="$out.d", deps="gcc")
        nw.rule("ld", "$ld -o $out $in $ldflags")

        nw.build(binary_name, "ld", [entry["output"] for entry in ninja_db["builds"]])
        if len(all_binaries) == 1:
            # Mark the first binary as the default one
            nw.default(binary_name)

        for entry in ninja_db["builds"]:
            nw.build(entry["output"], entry["rule"], entry["input"])

    # Create rules and builds to automatically regenerate the build.ninja
    nw.rule("regen_ninja", " ".join(sys.argv), generator=True)
    nw.build("build.ninja", "regen_ninja", settings, implicit=my_path)

    nw.build("all", "phony", all_binaries)

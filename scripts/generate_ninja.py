#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# from typing import dict


def replace_from_genvar_dict(string_in: str, genvars_dict: dict[str, str]) -> str:
    string_out = string_in
    match_found = True

    while match_found:
        match_found = False
        for key, value in genvars_dict.items():
            if "{" + key + "}" in string_out:
                string_out = string_out.replace("{" + key + "}", value)
                match_found = True  # Take another go around so we can nest genvars

    return string_out


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

    parser = argparse.ArgumentParser(prog=my_name, description="What the program does")
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

    # Read in settings on genvars
    genvars = {}
    for genvar_key, genvar_value in settings_db["genvar"].items():
        var = genvar_value
        if type(var) == list:
            var = " ".join(genvar_value)
        genvars[genvar_key] = var

    if args.verbose:
        print(genvars)

    # Read in the settings on profiles
    profile_db = {}
    for profile_key, profile_val in settings_db["profile"].items():
        if profile_key == profile:
            profile_db = profile_val

    if args.verbose:
        print(profile_db)

    build_script_file = open(output, "w")
    nw = NinjaWriter(build_script_file, width=120)

    # Set up some common variables and rules
    nw.variable("cc", "g++")
    nw.variable("cxx", "g++")
    nw.variable("ld", "g++")
    nw.variable("cflags", replace_from_genvar_dict((" ".join(profile_db["cflags"])), genvars))
    nw.variable("cxxflags", replace_from_genvar_dict((" ".join(profile_db["cxxflags"])), genvars))
    nw.variable("ldflags", replace_from_genvar_dict((" ".join(profile_db["ldflags"])), genvars))

    nw.rule("cc", "$cc -MD -MF $out.d $cflags -c -o $out $in", depfile="$out.d", deps="gcc")
    nw.rule("cxx", "$cxx -MD -MF $out.d $cxxflags -c -o $out $in", depfile="$out.d", deps="gcc")
    nw.rule("ld", "$ld -o $out $in $ldflags")

    all_binaries = []
    for binary_key, binary_val in settings_db["binary"].items():
        all_binaries += [binary_key]
        ninja_db = {
            "builds": [],
        }
        for file in binary_val["dependencies"]:
            if os.path.splitext(file)[-1] in [".c"]:
                ninja_db["builds"].append(
                    {
                        "input": f"{root}/{file}",
                        "rule": "cc",
                        "output": f"obj/{file}.o",
                    }
                )
            elif os.path.splitext(file)[-1] in [".cc", ".cp", ".cxx", ".cpp", ".CPP", ".c++", ".C"]:
                ninja_db["builds"].append(
                    {
                        "input": f"{root}/{file}",
                        "rule": "cxx",
                        "output": f"obj/{file}.o",
                    }
                )

        if args.verbose:
            print(ninja_db["builds"])
        nw.build(binary_key, "ld", [entry["output"] for entry in ninja_db["builds"]])
        for entry in ninja_db["builds"]:
            nw.build(entry["output"], entry["rule"], entry["input"])

        if len(all_binaries) == 1:
            # Mark the first binary as the default one
            nw.default(binary_key)

    # Create rules and builds to automatically regenerate the build.ninja
    nw.rule("regen_ninja", " ".join(sys.argv), generator=True)
    nw.build("build.ninja", "regen_ninja", settings, implicit=my_path)

    nw.build("all", "phony", all_binaries)
    nw.close()

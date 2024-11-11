#!/usr/bin/env python3
# -*- coding: utf-8 -*-


def replace_from_variable_dict(string_in: str, variables_dict: dict[str, str]) -> str:
    string_out = string_in
    match_found = True

    while match_found:
        match_found = False
        for key, value in variables_dict.items():
            if "{" + key + "}" in string_out:
                string_out = string_out.replace("{" + key + "}", value)
                match_found = True  # Take another go around so we can nest variables

    return string_out


if __name__ == "__main__":
    # Standard libs
    from pathlib import Path
    import argparse
    import git
    import inspect
    import os
    import sys
    import tomllib

    # 3rd party libs
    from ninja_syntax import Writer as NinjaWriter

    my_path = str(Path(inspect.getsourcefile(lambda: 0)).absolute())
    # my_dir = os.path.dirname(my_path)
    my_name = os.path.basename(my_path)
    git_repo = git.Repo(my_path, search_parent_directories=True)
    root = git_repo.git.rev_parse("--show-toplevel")

    parser = argparse.ArgumentParser(prog=my_name, description="Generate ninja template")
    parser.add_argument("-i", "--input")
    parser.add_argument("-o", "--output")
    parser.add_argument("-p", "--profile")
    parser.add_argument("-b", "--build-dir")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()

    if args.verbose:
        print(args)
        print(sys.argv)
        print("Generating ninja build file")

    settings = str(Path(args.input).absolute())
    output = str(Path(args.output).absolute())
    build_dir = str(Path(args.build_dir).absolute())
    profile = args.profile

    with open(settings, "rb") as f:
        settings_db = tomllib.load(f)

    # Read in settings on variables - first define default variables
    variables = {"root": f"{root}"}
    for variable_key, variable_value in settings_db["variable"].items():
        var = variable_value
        if type(var) == list:
            var = " ".join(variable_value)
        variables[variable_key] = var

    if args.verbose:
        print(variables)

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
    nw.variable("cflags", replace_from_variable_dict(" ".join(profile_db["cflags"]), variables))
    nw.variable("cxxflags", replace_from_variable_dict(" ".join(profile_db["cxxflags"]), variables))
    nw.variable("ldflags", replace_from_variable_dict(" ".join(profile_db["ldflags"]), variables))

    nw.rule("cc", "$cc -MD -MF $out.d $cflags -c -o $out $in", depfile="$out.d", deps="gcc")
    nw.rule("cxx", "$cxx -MD -MF $out.d $cxxflags -c -o $out $in", depfile="$out.d", deps="gcc")
    nw.rule("ld", "$ld -o $out $in $ldflags")  # Flag ordering here is important for ld

    all_binaries = []
    for binary_key, binary_val in settings_db["binary"].items():
        all_binaries += [binary_key]
        binary_deps = []
        binary_implicit_deps = []
        ninja_db = {
            "builds": [],
        }
        for file in binary_val["dependencies"]:
            file = replace_from_variable_dict(file, variables)
            file_relative_to_root = file.replace(f"{root}/", "")
            file_ext = os.path.splitext(file)[-1]

            if file_ext in [".c"]:
                ninja_db["builds"].append(
                    {
                        "input": file,
                        "rule": "cc",
                        "output": str(Path(f"{build_dir}/obj/{file_relative_to_root}.o").absolute()),
                    }
                )
            elif file_ext in [".cc", ".cp", ".cxx", ".cpp", ".CPP", ".c++", ".C"]:
                ninja_db["builds"].append(
                    {
                        "input": file,
                        "rule": "cxx",
                        "output": str(Path(f"{build_dir}/obj/{file_relative_to_root}.o").absolute()),
                    }
                )
            elif file_ext in [".a"]:
                binary_deps += [file]

        for file in binary_val["implicit_dependencies"]:
            file = replace_from_variable_dict(file, variables)
            file_relative_to_root = file.replace(f"{root}/", "")
            file_ext = os.path.splitext(file)[-1]

            if file_ext in [".a"]:
                binary_implicit_deps += [file]

        binary_deps += [entry["output"] for entry in ninja_db["builds"]]
        nw.build(binary_key, "ld", binary_deps, implicit=binary_implicit_deps)
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

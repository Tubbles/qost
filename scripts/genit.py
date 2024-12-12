#!/usr/bin/env python3
# -*- coding: utf-8 -*-


_FILE_EXTS = {
    ".gch": [".hh", ".H", ".hp", ".hxx", ".hpp", ".HPP", ".h++", ".tcc"],
    ".o": [".c", ".cc", ".cp", ".cxx", ".cpp", ".CPP", ".c++", ".C"],
}


def append_file_ext_if_exists(path: str, file_exts: dict[str, list[str]]) -> str:
    import os

    file_ext = os.path.splitext(path)[-1]
    for ext_key, ext_val in file_exts.items():
        if file_ext in ext_val:
            return f"{path}{ext_key}"

    return f"{path}"


def replace_from_variable_dict(string_in: str, variables: dict[str, str]) -> str:
    string_out = string_in
    match_found = True

    while match_found:
        match_found = False
        for key, value in variables.items():
            if "{" + key + "}" in string_out:
                string_out = string_out.replace("{" + key + "}", value)
                match_found = True  # Take another go around so we can nest variables

    return string_out


def append_to_variable_dict(to_add: dict[str, str | list], variables: dict[str, str]):
    for variable_key, variable_value in to_add.items():
        var = variable_value
        if type(var) == list:
            var = " ".join(variable_value)
        # var = replace_from_variable_dict(var, variables)
        variables[variable_key] = var


def parse_file_list_with_rules(ninja_db, variables, rules_paths_list, build_key, dep_key=None):
    from pathlib import Path

    root = variables["root"]
    build_dir = variables["build_dir"]

    for rules_paths_map in rules_paths_list:
        rule = rules_paths_map.get("rule", None)
        files = rules_paths_map["files"]

        for file_db in files:
            file_in = replace_from_variable_dict(file_db["in"], variables)

            if rule:
                file_relative_to_root = file_in.replace(f"{root}/", "")
                if "out" in file_db:
                    file_out_dir = str(Path(f"{build_dir}/{file_relative_to_root}").parent.absolute())
                    file_out = str(
                        Path(f"{file_out_dir}/{replace_from_variable_dict(file_db['out'], variables)}").absolute()
                    )
                else:
                    file_out = str(Path(f"{build_dir}/{file_relative_to_root}").absolute())
                    file_out = append_file_ext_if_exists(file_out, _FILE_EXTS)
                if dep_key:
                    ninja_db[dep_key].append(file_out)
                ninja_db[build_key].append({"input": file_in, "rule": rule, "output": file_out})
            else:
                if dep_key:
                    ninja_db[dep_key].append(file_in)


def main(
    input: str,
    output: str,
    profile: str,
    build_dir: str,
    verbose: bool = False,
):
    # Standard libs
    from copy import deepcopy
    from pathlib import Path
    from pprint import pprint
    import git
    import inspect
    import tomllib

    # 3rd party libs
    from ninja_syntax import Writer as NinjaWriter

    my_path_obj = Path(inspect.getsourcefile(lambda: 0)).absolute()
    my_path = str(my_path_obj)
    git_repo = git.Repo(my_path, search_parent_directories=True)
    root = git_repo.git.rev_parse("--show-toplevel")

    settings = str(Path(input).absolute())
    output = str(Path(output).absolute())
    build_dir = str(Path(build_dir).absolute())

    with open(settings, "rb") as f:
        settings_db = tomllib.load(f)

    # Read in settings on variables - first define default variables
    variables = {
        "root": f"{root}",
        "build_dir": f"{build_dir}",
        "in": "$in",  # Ninja implicit variable
        "out": "$out",  # Ninja implicit variable
    }
    append_to_variable_dict(settings_db["variable"], variables)
    # for variable_key, variable_value in settings_db["variable"].items():
    #     var = variable_value
    #     if type(var) == list:
    #         var = " ".join(variable_value)
    #     variables[variable_key] = var

    # Read in the settings on profiles
    profile_db = {}
    for profile_key, profile_val in settings_db["profile"].items():
        if profile_key == profile:
            profile_db = profile_val

    if profile_db["variable"]:
        append_to_variable_dict(profile_db["variable"], variables)

    pool_db = settings_db["pool"] if "pool" in settings_db else {}
    rule_db = settings_db["rule"] if "rule" in settings_db else {}

    if verbose:
        pprint(variables)
        pprint(profile_db)
        pprint(pool_db)
        pprint(rule_db)

    build_script_file = open(output, "w")
    nw = NinjaWriter(build_script_file, width=120)

    for pool_key, pool_val in pool_db.items():
        nw.pool(pool_key, pool_val["depth"])

    # Set up some common variables and rules
    for rule_key, rule_val in rule_db.items():
        command = replace_from_variable_dict(rule_val["command"], variables)
        temp_dict = deepcopy(rule_val)
        temp_dict.pop("command", None)
        for key, val in temp_dict.items():
            temp_dict[key] = replace_from_variable_dict(val, variables)
        nw.rule(rule_key, command, **temp_dict)

    all_binaries = []
    for binary_key, binary_val in settings_db["binary"].items():
        binary_name = replace_from_variable_dict(binary_key, variables)
        all_binaries += [binary_name]
        ninja_db = {
            "deps": [],
            "implicit_deps": [],
            "builds": [],
            "intermediate_builds": [],
        }

        if "dependencies" in binary_val:
            parse_file_list_with_rules(ninja_db, variables, binary_val["dependencies"], "builds", "deps")
        if "implicit_dependencies" in binary_val:
            parse_file_list_with_rules(
                ninja_db, variables, binary_val["implicit_dependencies"], "builds", "implicit_deps"
            )
        if "intermediates" in binary_val:
            parse_file_list_with_rules(ninja_db, variables, binary_val["intermediates"], "intermediate_builds")

        if verbose:
            pprint(ninja_db)

        nw.build(binary_name, binary_val["rule"], ninja_db["deps"], implicit=ninja_db["implicit_deps"])

        for entry in ninja_db["intermediate_builds"]:
            nw.build(entry["output"], entry["rule"], entry["input"])

        build_deps = [entry["output"] for entry in ninja_db["intermediate_builds"]]
        for entry in ninja_db["builds"]:
            nw.build(entry["output"], entry["rule"], entry["input"], implicit=build_deps)

        if len(all_binaries) == 1:
            # Mark the first binary as the default one
            nw.default(binary_name)

    # Create rules and builds to automatically regenerate the build.ninja
    equivalent_commandline = [
        my_path,
        "--input",
        input,
        "--output",
        output,
        "--profile",
        profile,
        "--build-dir",
        build_dir,
    ]
    if verbose:
        equivalent_commandline.append("--verbose")
    nw.rule("regen_ninja", " ".join(equivalent_commandline), generator=True)
    nw.build("build.ninja", "regen_ninja", settings, implicit=my_path)

    nw.build("all", "phony", all_binaries)
    nw.close()


if __name__ == "__main__":
    # Standard libs
    from pathlib import Path
    import argparse
    import inspect
    import sys

    # 3rd party libs
    from ninja_syntax import Writer as NinjaWriter

    my_path_obj = Path(inspect.getsourcefile(lambda: 0)).absolute()
    my_path = str(my_path_obj)
    my_name = my_path_obj.name

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

    args = {
        "input": args.input,
        "output": args.output,
        "profile": args.profile,
        "build_dir": args.build_dir,
        "verbose": args.verbose,
    }

    main(**args)

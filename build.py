#!/usr/bin/env python3
# -*- coding: utf-8 -*-

available_modules = [
    "doctest",
    "fmt",
    "lua",
    "wren",
]

available_modules_help_text = f"""Optionally takes zero or more module names to operate on (defaults to all, in any order):\n\
{"\n".join([f"{' '*20}* {module} {' '*(10 - len(module))}" for module in available_modules])}\n\
{' '*18}The module names can also be given without giving the `setup' command, making it implicitly selected"""

available_arguments = [
    ("extra-verbose", "Print extra verbose information suitable for debugging the build process to the terminal"),
    ("generate", "Only generate the ninja script and compile commands database, do not compile"),
    ("optimized", "Build with optimized configuration instead of debug configuration, suitable for releases"),
    ("pristine", "Clean the build directory before build"),
    ("run", "Run the resulting executable after a successful build"),
    ("setup", f"Operate on the third party libraries instead of the project itself. {available_modules_help_text}"),
    ("test", "Run all tests"),
    ("verbose", "Print verbose information to the terminal"),
]


class AmbiguousArgumentError(Exception):
    pass


def argument_expand(given: str, possible_args: list[str]):
    matches = []
    for arg in possible_args:
        if arg.startswith(given):
            matches.append(arg)

    return matches


def argument_ensure_single_match(matches: list[str]):
    match (len(matches)):
        case 0:
            return False
        case 1:
            return True
        case _:
            raise AmbiguousArgumentError(f"{matches}")


if __name__ == "__main__":
    from pathlib import Path
    import argparse
    import git
    import inspect
    import os
    import pprint
    import shutil
    import subprocess
    import sys

    sys.path.insert(0, "scripts")
    import genit

    my_path_obj = Path(inspect.getsourcefile(lambda: 0)).absolute()
    my_path = str(my_path_obj)
    my_name = my_path_obj.name
    git_repo = git.Repo(my_path, search_parent_directories=True)
    root = git_repo.git.rev_parse("--show-toplevel")
    project = str(Path(root).stem)

    parser = argparse.ArgumentParser(
        prog=my_name,
        description="Facilitates building, testing, and running this project.",
        formatter_class=argparse.RawTextHelpFormatter,
    )

    help_text = f"""can be zero or more of (in any order, partial words supported):\n\
{"\n".join([f"  * {arg[0]} {' '*(10 - len(arg[0]))} - {arg[1]}" for arg in available_arguments])}"""

    parser.add_argument("command", nargs="*", help=help_text)
    args = parser.parse_args()
    possible_arg_names = [arg[0] for arg in available_arguments]

    implicit_setup = False
    expanded_args = {}
    for given in args.command:
        matches = argument_expand(given, possible_arg_names)
        try:
            expanded_args[matches[0]] = argument_ensure_single_match(matches)
        except AmbiguousArgumentError as e:
            print(f"ERROR: Ambiguous matches on command line for '{given}': {e}\n")
            parser.print_help()
            sys.exit(1)
        except IndexError:
            if not given in available_modules:
                print(f"ERROR: Unknown argument '{given}'\n")
                parser.print_help()
                sys.exit(1)
            else:
                implicit_setup = True

    extra_verbose = "extra-verbose" in expanded_args
    generate = "generate" in expanded_args
    optimized = "optimized" in expanded_args
    pristine = "pristine" in expanded_args
    run = "run" in expanded_args
    setup = "setup" in expanded_args or implicit_setup
    test = "test" in expanded_args
    verbose = "verbose" in expanded_args or extra_verbose

    if extra_verbose:
        print(args)
        print(sys.argv)
        pp = pprint.PrettyPrinter(depth=4)
        pp.pprint({key: value for (key, value) in locals().items() if key in possible_arg_names})

    first_time_setup_needed = False
    num_third_party_files = 0
    for _, _, files in os.walk(str(Path(f"{root}/src/modules"))):
        num_third_party_files += len(files)

    if num_third_party_files == 0:
        print("Setup needed")
        first_time_setup_needed = True

    if setup or first_time_setup_needed:
        selected_modules = [module for module in sys.argv if module in available_modules]
        if not selected_modules:
            selected_modules = available_modules
        setup_args = []
        if verbose:
            setup_args.append("--verbose")
        if pristine:
            setup_args.append("--pristine")
        if test:
            setup_args.append("--test")
        setup_args += selected_modules
        returncode = subprocess.run([str(Path(f"{root}/scripts/setup.sh"))] + setup_args).returncode
        if setup:
            sys.exit(returncode)

    build = not generate
    debug = not optimized
    build_type = "debug" if debug else "optimized"
    build_dir = str(Path(f"{root}/output/{build_type}"))

    ninja_build_file = str(Path(f"{build_dir}/build.ninja"))

    genit_args = {}
    ninja_args = []
    genit_args["input"] = str(Path(f"{root}/genit.toml"))
    genit_args["output"] = ninja_build_file
    genit_args["profile"] = build_type

    if pristine:
        shutil.rmtree(build_dir)

    if not os.path.exists(ninja_build_file):
        generate = True

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    genit_args["build_dir"] = build_dir
    ninja_args += ["-C", build_dir]

    genit_args["verbose"] = verbose
    if verbose:
        ninja_args.append("--verbose")

    if generate:
        genit.main(**genit_args)

        if verbose:
            with open(ninja_build_file, "r") as ninja_build:
                print("".join(ninja_build.readlines()))

        with open(str(Path(f"{build_dir}/compile_commands.json")), "w") as compile_commands:
            subprocess.run(["ninja"] + ninja_args + ["-t", "compdb"], stdout=compile_commands)

    if build:
        subprocess.run(["ninja"] + ninja_args + [project])

    if run:
        if verbose:
            print("Running project")
        returncode = subprocess.run(str(Path(f"{build_dir}/{project}"))).returncode
        sys.exit(returncode)

import sys
import os
import argparse
import traceback
import re
import json
import pathlib

from pathlib import Path

# import Mbed's built-in tools library
project_root_dir   = Path(os.path.dirname(__file__)).parent.parent
mbed_os_dir        = os.path.join(project_root_dir, "extern/mbed-os")
custom_targets_dir = os.path.join(project_root_dir, "targets")

sys.path.append(mbed_os_dir)

try:
    from tools import build_api, options
    from tools.resources import Resources, FileType
    from tools.notifier.term import TerminalNotifier
    from tools.config import Config
    from tools.targets import Target
except ImportError:

    print("Failed to import the Mbed OS Python configuration system!")
    print("Be sure that you have installed all required python modules, see ",mbed_os_dir,"/requirements.txt")
    traceback.print_exc()
    exit(1)



def write_target_header(target_header_path:str, symbol_list:list):
    """ Write the target header to the given path using the given symbol defines"""

    target_header = open(target_header_path, "w")
    target_header.write(
    """/*
    Mbed OS Target Define Header.
    This contains all of the #defines specific to your target and device.
    It is prepended to every source file using the -include compiler option.
    AUTOGENERATED by configure_for_target.py.  DO NOT EDIT!
    */

""")

    with_value_re = re.compile(r"^([^=]+)=(.*)$")

    for definition in sorted(symbol_list):

        # convert defines in command-line format (VAR=value) to header format (#define VAR value)
        with_value_match = with_value_re.fullmatch(definition)
        if with_value_match is not None:
            target_header.write("#define " + with_value_match.group(1) + " " + with_value_match.group(2) + "\n")
        else:
            # no value given, so follow GCC command line semantics and define it to 1
            target_header.write("#define " + definition + " 1\n")

    target_header.close()

def build_cmake_list(list_name:str, contents:list):
    """ Get a string containing CMake code for a list with the given name. """

    list_string = f"set({list_name}"
    for string in contents:

        # items containing spaces need to be quoted
        if " " in string:
            list_string += "\n   \"" + string + "\""
        else:
            list_string += "\n   " + string

    list_string += ")\n\n"

    return list_string

def build_cmake_path_list(list_name:str, paths:list):
    """ Build a CMake list of file paths.  Does extra conversion to prepare file paths for CMake."""

    converted_paths = []
    for path in paths:

        # paths should be relative to the mbed-src directory
        relative_path = os.path.relpath(path, mbed_os_dir)

        # CMake always uses forward slashes, even on Windows
        converted_paths.append(relative_path.replace(os.path.sep, "/"))

    return build_cmake_list(list_name, converted_paths)

def write_cmake_config(cmake_config_path:str, mcu_compile_flags:list, mcu_link_flags:list,
    linker_script:str, include_dirs:list, source_files:list, target_name:str, toolchain_name:str, profile_flags:list,
    profile_link_flags:list, profile_names:list):

    """ Write the configuration file for CMake using info extracted from the Python scripts"""

    cmake_config = open(cmake_config_path, "w")
    cmake_config.write(
    """# Mbed OS CMake configuration file.
# This contains all of the information needed for CMake to compile and use Mbed OS.
# It was extracted from the Mbed configuration files when you originally configured this project.
# AUTOGENERATED by configure_for_target.py.  DO NOT EDIT!

""")

    cmake_config.write(build_cmake_list("MBED_TARGET_NAME", [target_name]))
    cmake_config.write(build_cmake_list("MBED_TOOLCHAIN_NAME", [toolchain_name]))

    # note: CMake convention is that variables called "FLAGS" are strings while variables called "OPTIONS" are lists.
    cmake_config.write(build_cmake_list("MCU_COMPILE_OPTIONS", mcu_compile_flags))
    cmake_config.write(build_cmake_list("MCU_LINK_OPTIONS", mcu_link_flags))
    cmake_config.write(build_cmake_path_list("MBED_LINKER_SCRIPT", [linker_script]))
    cmake_config.write(build_cmake_path_list("MBED_INCLUDE_DIRS", include_dirs))
    cmake_config.write(build_cmake_path_list("MBED_SOURCE_FILES", source_files))

    for profile_idx in range(0, len(profile_names)):
        cmake_config.write(build_cmake_list("MCU_COMPILE_OPTIONS_" + profile_names[profile_idx], profile_flags[profile_idx]))

    for profile_idx in range(0, len(profile_names)):
        cmake_config.write(build_cmake_list("MCU_LINK_OPTIONS_" + profile_names[profile_idx], profile_link_flags[profile_idx]))

    cmake_config.close()

# Function to print all known configuration variables.
# Based on tools/get_config.py
def print_config(print_descriptions:bool, toolchain):

    params, macros = toolchain.config.get_config_data()
    features = toolchain.config.get_features()

    if not params and not macros:
        print("No configuration data available.")
        sys.exit(0)
    if params:
        print("Configuration parameters")
        print("------------------------")
        for p in sorted(list(params.keys())):
            if print_descriptions:
                print(params[p].get_verbose_description())
            else:
                print(str(params[p]))
        print("")

    print("Macros")
    print("------")
    for m in Config.config_macros_to_macros(macros):
        print(m)

# Parse args
# -------------------------------------------------------------------------
GENERATED_DEFAULT_PATH = "../mbed-cmake-config/"

parser = argparse.ArgumentParser(description="Configure mbed-cmake for a processor target.")

parser.add_argument('target', type=str, help="Mbed target name to configure the build system for.")

parser.add_argument('-l', '--list-config', action="store_true", help="List all the config options that Mbed OS supports for your target")
parser.add_argument('-c', '--print-config', action="store_true", help="Print all the config options that Mbed OS supports for your target, with descriptions")
parser.add_argument('-p', '--generated-path', default=GENERATED_DEFAULT_PATH,
                    help="The relative path to the directory which will store the generated files")
parser.add_argument('-t', '--toolchain', default="GCC_ARM",
                    choices=["ARMC6", "GCC_ARM"],
                    help="Toolchain that you will be compiling with.")
parser.add_argument('-a', '--app-config', default=None,
                    help="Path to mbed_app.json")
parser.add_argument('-x', '--custom-target', action="store_true",
                    help="Tell the config script that the target is a custom target")
args = parser.parse_args()

target_name = args.target
toolchain_name = args.toolchain
get_config = args.list_config or args.print_config
verbose_config = args.print_config
generated_rpath = args.generated_path
generated_path = os.path.join(project_root_dir, generated_rpath)
app_config_path = args.app_config
is_custom_target = args.custom_target

pathlib.Path(generated_path).mkdir(parents=True, exist_ok=True)
with open(os.path.join(generated_path, "do-not-modify.txt"), 'w') as do_not_modify:
    do_not_modify.write("Files in this folder were generated by configure_for_target.py")


# Perform the scan of the Mbed OS dirs & custom targets
# -------------------------------------------------------------------------

# Add custom targets
Target.add_extra_targets(custom_targets_dir)

# profile constants
# list of all profile JSONs
profile_jsons = [os.path.join(project_root_dir, "tools/profiles/develop.json"),
                 os.path.join(project_root_dir, "tools/profiles/debug.json"),
                 os.path.join(project_root_dir, "tools/profiles/release.json")]
# CMake build type matching each Mbed profile
profile_cmake_names = ["RELWITHDEBINFO",
                       "DEBUG",
                       "RELEASE"]

print("Configuring build system for target " + target_name)

# Can NOT be the current directory, or it screws up some internal regexes inside mbed tools.
# That was a fun hour to debug...

config_header_dir = os.path.join(generated_path, "config-headers")
pathlib.Path(config_header_dir).mkdir(parents=True, exist_ok=True) # create dir if not exists

notifier = TerminalNotifier(True, False)

# create a different toolchain for each profile so that we can detect the flags needed in each configuration
profile_toolchains = []
for profile_json_path in profile_jsons:
    with open(profile_json_path) as profile_file:

        print(">> Collecting data for config " + profile_json_path)
        profile_data = json.load(profile_file)
        profile_toolchain = build_api.prepare_toolchain(src_paths=[mbed_os_dir], build_dir=config_header_dir, target=target_name, toolchain_name=toolchain_name, build_profile=[profile_data], app_config=app_config_path)
        # each toolchain must then scan the mbed dir to pick up more configs
        resources = Resources(notifier).scan_with_toolchain(src_paths=[mbed_os_dir], toolchain=profile_toolchain, exclude=True)
        profile_toolchain.RESPONSE_FILES=False


        profile_toolchains.append(profile_toolchain)


# Add custom targets
target_dir = "TARGET_" + target_name
resources.add_directory(os.path.join(custom_targets_dir, target_dir))

# Profiles seem to only set flags, so for the remaining operations we can use any toolchain
toolchain = profile_toolchains[0]
print("Generated config header: " + toolchain.get_config_header())


print("Using settings from these JSON files:\n " + "\n ".join(resources.get_file_paths(FileType.JSON)))


# Write target header
# -------------------------------------------------------------------------

target_header_path = os.path.join(config_header_dir, "mbed_target_config.h")
write_target_header(target_header_path, toolchain.get_symbols())
print("Generated target config header: " + target_header_path)

# Write CMake config file
# -------------------------------------------------------------------------

cmake_config_dir = os.path.join(generated_path, "cmake", )
pathlib.Path(cmake_config_dir).mkdir(parents=True, exist_ok=True) # create dir if not exists

cmake_config_path = os.path.join(cmake_config_dir, "MbedOSConfig.cmake")

# create include paths
inc_paths = resources.get_file_paths(FileType.INC_DIR)
inc_paths = set(inc_paths) # De-duplicate include paths
inc_paths = sorted(set(inc_paths)) # Sort include paths for consistency

# Get sources
sources = (
            resources.get_file_paths(FileType.ASM_SRC) +
            resources.get_file_paths(FileType.C_SRC) +
            resources.get_file_paths(FileType.CPP_SRC)
        )

# common flags are the intersection of flags from all configurations
common_flags = set(toolchain.flags['common'])
common_link_flags = set(toolchain.flags['ld'])

for profile_idx in range(1, len(profile_toolchains)):

    common_flags = common_flags & set(profile_toolchains[profile_idx].flags['common'])
    common_link_flags = common_link_flags & set(profile_toolchains[profile_idx].flags['ld'])

# profile flags are the flags in one profile that are not in common flags.
profile_flags = []
profile_link_flags = []
for profile_idx in range(0, len(profile_toolchains)):
    profile_flags.append(set(profile_toolchains[profile_idx].flags['common']) - common_flags)
    profile_link_flags.append(set(profile_toolchains[profile_idx].flags['ld']) - common_link_flags)

write_cmake_config(cmake_config_path,
    mcu_compile_flags=toolchain.flags['common'],
    mcu_link_flags=toolchain.flags['ld'],
    linker_script=resources.get_file_paths(FileType.LD_SCRIPT)[0],
    include_dirs = inc_paths,
    source_files = sources,
    target_name = target_name,
    toolchain_name=toolchain_name,
    profile_flags = profile_flags,
    profile_link_flags = profile_link_flags,
    profile_names = profile_cmake_names)

print("Generated CMake configuration file: " + cmake_config_path)

if get_config:
    print("")
    print_config(verbose_config, toolchain)

print("-----------------------------------------------")
print("      mbed-cmake configuration complete.")
print("")
print("      MAKE SURE TO DELETE ANY OLD CMAKE")
print("    BUILD DIRS BEFORE RUNNING CMAKE AGAIN")
print("-----------------------------------------------")

import os
import argparse

from kconfiglib import Kconfig

# TOP_DIR = os.path.join(os.path.dirname(__file__), "..")

def main():
    args = parse_args()

    print("Parsing " + args.kconfig)
    kconf = Kconfig(args.kconfig, warn_to_stderr=False,
                    suppress_traceback=True)

    if os.path.exists(args.config_out):
        print(kconf.load_config(args.config_out)) 
    else:
        print(kconf.load_config(args.configs_in[0]))
        for config in args.configs_in[1:]:
            print(kconf.load_config(config, replace=False))

    print(kconf.write_config(args.config_out))
    print(kconf.write_autoconf(args.header_out))

    # write_kconfig_filenames(kconf, args.kconfig_list_out)


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("kconfig",
                        help="Top-level Kconfig file")
    parser.add_argument("config_out",
                        help="Output config file")
    parser.add_argument("header_out",
                        help="Output header file")
    parser.add_argument("kconfig_list_out",
                        help="Output file for list of parsed Kconfig files")
    parser.add_argument("configs_in",
                        nargs="+",
                        help="Input configration fragments")
    
    return parser.parse_args()

def write_kconfig_filenames(kconf, kconfig_list_path):
    with open(kconfig_list_path, 'w') as out:
        for path in sorted({os.path.realpath(os.path.join(kconf.srctree, path))
                            for path in kconf.kconfig_filenames}):
            print(path, file=out)

if __name__ == "__main__":
    main()
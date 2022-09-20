# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
import argparse
from collections import OrderedDict
import sys
from version import __version__
from build import Build

COMMAND_GROUPS = {
    'sub parsers': [
        Build,

    ],

}

class Cmds:
    def __init__(self):
        # self.topdir = None
        self.parser = None
        self.builtins = {}
        self.builtin_groups = OrderedDict() 
        # self.subparser = None
        # self.subparser_groups = OrderedDict()
    
        for group, classes in COMMAND_GROUPS.items():
            lst = [cls() for cls in classes]
            self.builtins.update({command: command for command in lst})
            self.builtin_groups[group] = lst

        print(self.builtin_groups)


    def run(self, argv):
        self.setup_parser()
        # self.run_command(argv)

    def setup_parser(self):
        parser = argparse.ArgumentParser(description='WCH commands argument.')
        
        parser.add_argument('-v', '--version', action='version',
                            version=f'WCH version: v{__version__}',
                            help='print the program version')

        subparser = parser.add_subparsers(metavar='<command>',
                                        dest='command')

        for sub in self.builtin_groups.items():
            print(sub)

        self.parser = parser
        self.subparser = subparser

    def run_command(self, argv):
        print(argv)

        args, unknown = self.parser.parse_known_args(args=argv)
        print(args)

        try:
            cmd = self.builtins.get(args.command, self.builtins['help'])
            cmd.run(args, unknown)

        except KeyboardInterrupt:
            sys.exit(0)
        
            

class WCHHelpAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        # Just mark that help was requested.
        namespace.help = True

def main(argv=None):
    print("init")

    cmd = Cmds()
    cmd.run(argv or sys.argv[1:])


if __name__ == "__main__":
    main()
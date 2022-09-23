# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
import sys
import argparse
import log
from version import __version__
from commands.build import Build
from commands.flash import Flash
from commands.export import wchExport

class App:

    def __init__(self):
        # self.topdir = None
        self.parser = None
        self.builtins = {}

        for Class in COMMANDS:
            cmd = Class()
            self.builtins.update({cmd.name: cmd})

    def run(self, argv):
        self.setup_parser()
        self.run_command(argv)

    def setup_parser(self):
        parser = argparse.ArgumentParser(description='WCH commands argument.')

        parser.add_argument('-v', '--version', action='version',
                            version=f'WCH version: v{__version__}',
                            help='print the program version')

        subparser = parser.add_subparsers(metavar='<command>',
                                        dest='command', 
                                        help='sub-command help')

        for command in self.builtins.values():
            command.add_parser(subparser)

        self.parser = parser
        self.subparser = subparser

    def run_command(self, argv):
        args, unknown = self.parser.parse_known_args(argv)
        try:
            if args.command in self.builtins:
                cmd = self.builtins.get(args.command)
                cmd.run(args, unknown)
            else:
                log.die('unexpected arguments: '+str(argv))
        except KeyboardInterrupt:
            sys.exit(0)


COMMANDS = {
    Build,
    wchExport,
    # Flash,
    # Debug,
}

def main(argv=None):
    cmd = App()
    cmd.run(argv or sys.argv[1:])


if __name__ == "__main__":
    main()
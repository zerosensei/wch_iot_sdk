import argparse
import os
from tkinter.messagebox import NO
from command import Command

BUILD_USAGE = '''\
wch build [-h] 
'''

class AlwaysIfMissing(argparse.Action):

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, values or 'always')

class Build(Command):

    def __init__(self):
        super().__init__(
            'build',
            'build a workspace'
        )

        self.source_dir = None
        '''Source directory for the build, or None on error.'''

        self.build_dir = None
        '''Final build directory used to run the build, or None on error.'''

        self.created_build_dir = False
        '''True if the build directory was created; False otherwise.'''
        
    def do_add_parser(self, parser_add):
        parser = parser_add.add_parser(
            self.name,
            help=self.help,
            formatter_class=argparse.RawDescriptionHelpFormatter,
            usage=BUILD_USAGE)

        parser.add_argument('-S', '--SOC', 
                            help='SOC to build for with optional SOC revision')

        parser.add_argument('-s', '--source-dir', help=argparse.SUPPRESS)

        parser.add_argument('-d', '--build-dir',
                            help='build directory to create or use')

        parser.add_argument('-p', '--pristine', choices=['auto', 'always',
                            'never'], action=AlwaysIfMissing, nargs='?',
                            help='pristine build folder setting')
        return parser

    def do_run(self, args, remainder):
        print("build run")
        print(args)
        print(remainder)

        self.args = args
        source_dir = self.args.source_dir
        # TODO: 
        self._parse_remainder(remainder)

        if source_dir:
            self.args.souce_dir = source_dir

        print("source dir: "+self.args.source_dir)
        print("cmake opts: "+str(self.args.cmake_opts))

        self._setup_build_dir()

    def _parse_remainder(self, remainder):
        self.args.source_dir = None
        self.args.cmake_opts = None

        try:
            if remainder[0] != '--':
                self.args.source_dir = remainder[0]
                remainder = remainder[1:]
            if remainder[0] == '--':
                remainder = remainder[1:]
            if remainder:
                self.args.cmake_opts = remainder
        except IndexError:
            return   

    def _setup_build_dir(self):
        print(os.getcwd())



import argparse
import os

BUILD_USAGE = '''\
west build [-h] 
'''

class Build(object):

    def __init__(self):
        self.source_dir = None
        '''Source directory for the build, or None on error.'''

        self.build_dir = None
        '''Final build directory used to run the build, or None on error.'''

        self.created_build_dir = False
        '''True if the build directory was created; False otherwise.'''
        
    def add_parser(self, parser_add):
        parser = parser_add.add_parser(
            self.name,
            help=self.help,
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=self.description,
            usage=BUILD_USAGE)

        parser.add_argument('-s', '--source-dir', help=argparse.SUPPRESS)
        parser.add_argument('-d', '--build-dir',
                            help='build directory to create or use')

    def run(self, args):
        print("build")
        print(args)
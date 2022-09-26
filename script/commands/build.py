import argparse
import os
import shutil
import log
from commands.command import Command
from commands.cmake import run_cmake, run_build

BUILD_USAGE = '''\
wch build [-h] 
'''

DEFAULT_BUILD_DIR = 'build'

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
        
        self.created_build_dir = False
        '''True if the build directory was created; False otherwise.'''

        self.run_cmake = False
        '''True if CMake was run; False otherwise.

        Note: this only describes CMake runs done by this command. The
        build system generated by CMake may also update itself due to
        internal logic.'''

    def do_add_parser(self, parser_add):
        parser = parser_add.add_parser(
            self.name,
            help=self.help,
            formatter_class=argparse.RawDescriptionHelpFormatter,
            usage=BUILD_USAGE)

        parser.add_argument('-S', '--SOC', 
                            help='SOC serious to build')

        parser.add_argument('-s', '--source-dir', help='source directory to use')

        parser.add_argument('-d', '--build-dir',
                            help='build directory to create or use')

        parser.add_argument('-p', '--pristine', choices=['auto', 'always',
                            'never'], action=AlwaysIfMissing, nargs='?',
                            help='pristine build folder setting')
        return parser

    def do_run(self, args, remainder):
        log.inf("-- WCH build: run")

        self.args = args
        self._parse_remainder(remainder)
        self._find_source_dir()
        self._setup_build_dir()

        log.dbg('build dir: {}'.format(self.build_dir))

        if args.pristine is not None:
            pristine = args.pristine
        else:
            pristine = 'auto'
        
        if pristine == 'always':
            self._clear_build()
            self.run_cmake = True
        if pristine == 'auto':
            self.run_cmake = True
        if pristine == 'never':
            self.run_cmake = False

        self._run_cmake(self.args.cmake_opts)

        self._run_build()



    def _parse_remainder(self, remainder):
        self.args.cmake_opts = None
        try:
            if remainder:
                self.args.cmake_opts = remainder
        except IndexError:
            return   

    def _setup_build_dir(self):
        # δָ��build dir ʹ��default
        build_dir = DEFAULT_BUILD_DIR
        if self.args.build_dir:
            build_dir = self.args.build_dir

        build_dir = os.path.abspath(build_dir)

        if not build_dir:
            log.die('unable to determine a default build folder.')
        
        if os.path.exists(build_dir):
            if not os.path.isdir(build_dir):
                log.die('build directory {} exists but is not a directory'.
                        format(build_dir))
        else:
            os.makedirs(build_dir, exist_ok=False)
            self.created_build_dir = True
            self.run_cmake = True
        
        self.build_dir = build_dir

    def _find_source_dir(self):
        if self.args.SOC:
            self.args.cmake_opts = ['-DSOC={}'.format(self.args.SOC)]
        else:
            log.die('no defined SOC')

        if self.args.source_dir:
            source_dir = self.args.source_dir
        else:
            # ���û�д���Դ�ļ�����ǰ�ļ�ΪԴ�ļ�
            source_dir = os.getcwd()
        
        self.source_dir = os.path.abspath(source_dir)

        if self.source_dir:
            if not os.path.isdir(self.source_dir):
                log.die('source directory {} does not exist'.format(self.source_dir))
            if 'CMakeLists.txt' not in os.listdir(self.source_dir):
                log.die('{} does not contain a CMakeLists.txt'.format(self.source_dir))

        log.dbg('source dir: {}, cmake opt: {}'.format(self.source_dir, 
                                                    self.args.cmake_opts))
                                                

    def _clear_build(self):
        if os.path.exists(self.build_dir):
            shutil.rmtree(self.build_dir)

    def _run_cmake(self, cmake_opts):
        if not self.run_cmake:
            return
        
        log.inf("-- WCH build: Generating a build system")

        cmake_opts = []
        if self.args.cmake_opts:
            cmake_opts.extend(self.args.cmake_opts)
        
        cmake_opts.extend(['-S{}'.format(self.source_dir)])
        cmake_opts.extend(['-B{}'.format(self.build_dir)])
        # TODO: ��ѡ
        cmake_opts.extend(['-GNinja'])

        log.dbg('cmake_opts: {}'.format(cmake_opts))
        run_cmake(cmake_opts)


    def _run_build(self):
        run_build(self.build_dir)
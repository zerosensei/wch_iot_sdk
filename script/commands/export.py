# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
import argparse
from pathlib import Path
import log
from commands.command import Command
from commands.cmake import run_cmake

class wchExport(Command):

    def __init__(self):
        super().__init__(
            'wch-export',
            'export wch installation as a CMake config package'
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name,
            help=self.help,
            formatter_class=argparse.RawDescriptionHelpFormatter
        )

        return parser
    
    def do_run(self, args, unknown):
        share = Path(__file__).parents[2] / 'cmake' / 'share'

        log.dbg('share path: {}'.format(share))
        run_cmake(['-P', str(share / 'wch_export.cmake')])
        
        
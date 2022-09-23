# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

import argparse
from commands.command import Command

BUILD_USAGE = '''\
west build [-h] 
'''

class Flash(Command):

    def __init__(self):
        super().__init__(
            'flash',
            'flash and run a binary on a SOC'
        )

        
    def do_add_parser(self, parser_add):
        pass

    def do_run(self, args, remainder):
        print("flash run")
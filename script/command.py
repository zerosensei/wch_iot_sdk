# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0
from abc import ABC, abstractmethod
from typing import List
import argparse


class Command(ABC):
    def __init__(self, name: str, help: str):
        self.name: str = name
        self.help: str = help
    
    def run(self, args: argparse.Namespace, unknown: List[str]):
        self.do_run(args, unknown)

    def add_parser(self, parser_adder) -> argparse.ArgumentParser:
        parser = self.do_add_parser(parser_adder)

        if parser is None:
            raise ValueError('do_add_parser did not return a value')

        self.parser = parser
        return self.parser

    @abstractmethod
    def do_add_parser(self, parser_adder) -> argparse.ArgumentParser:
        '''Subclass method for registering command line arguments.

        This is called by `WestCommand.add_parser` to register the
        command's options and arguments.

        Subclasses should ``parser_adder.add_parser()`` to add an
        ``ArgumentParser`` for that subcommand, then add any
        arguments. The final parser must be returned.

        :param parser_adder: The return value of a call to
            ``argparse.ArgumentParser.add_subparsers()``
        '''
    @abstractmethod
    def do_run(self, args: argparse.Namespace, unknown: List[str]):
        '''Subclasses must implement; called to run the command.

        :param args: ``argparse.Namespace`` of parsed arguments
        :param unknown: If ``accepts_unknown_args`` is true, a
            sequence of un-parsed argument strings.
        '''


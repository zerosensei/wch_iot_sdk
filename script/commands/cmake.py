# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

import shutil
import sys
import subprocess
import log
import packaging.version

def run_cmake(args):
    cmake = shutil.which('cmake')

    if cmake is None:
        log.die('CMake is not installed or cannot be found.')
    _ensure_min_version(cmake)

    cmd = [cmake] + args

    log.dbg('cmd: {}'.format(cmd))

    p = subprocess.Popen(cmd)
    out, _ = p.communicate()

    if p.returncode == 0:
        if out:
            return out.decode(sys.getdefaultencoding()).splitlines()
        else:
            return None
    else:
        # A real error occurred, raise an exception
        raise subprocess.CalledProcessError(p.returncode, p.args)

def run_build(build_dir, **kwargs):
    return run_cmake(['--build', build_dir], **kwargs)

def _ensure_min_version(cmake):
    cmd = [cmake, '--version']

    try:
        version_out = subprocess.check_output(cmd, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as cpe:
        log.die('cannot get cmake version:', str(cpe))
    decoded = version_out.decode('utf-8')
    lines = decoded.splitlines()
    if not lines:
        log.die('can\'t get cmake version: ' +
                'unexpected "cmake --version" output:\n{}\n'.
                format(decoded) +
                'Please install CMake ' + _MIN_CMAKE_VERSION_STR +
                ' or higher (https://cmake.org/download/).')
    version = lines[0].split()[2]
    if '-' in version:
        # Handle semver cases like "3.19.20210206-g1e50ab6"
        # which Kitware uses for prerelease versions.
        version = version.split('-', 1)[0]
    if packaging.version.parse(version) < _MIN_CMAKE_VERSION:
        log.die('cmake version', version,
                'is less than minimum version {};'.
                format(_MIN_CMAKE_VERSION_STR),
                'please update your CMake (https://cmake.org/download/).')
    else:
        log.dbg('cmake version', version, 'is OK; minimum version is',
                _MIN_CMAKE_VERSION_STR)

_MIN_CMAKE_VERSION_STR = '3.13.1'
_MIN_CMAKE_VERSION = packaging.version.parse(_MIN_CMAKE_VERSION_STR)
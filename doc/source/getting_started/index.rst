
.. _getting_started:

快速入门
##########

本文档旨在指导用户搭建WCH芯片的软件开发环境，并通过示例展示配置，编译以及下载等步骤。


准备工作
=========

硬件
~~~~~~~~

* WCH开发板
* USB数据线（A转Type-C）
* 电脑（Windows 或 Linux） linux正在开发

软件
~~~~~~~

软件安装
*************

WCH_IOT_SDK需依赖一些软件包，其最低要求版本如下：

.. list-table::
   :header-rows: 1

   * - Tool
     - Min. Version

   * - `CMake <https://cmake.org/>`_
     - 3.20.0

   * - `Python <https://www.python.org/>`_
     - 3.6


.. tabs::

   .. group-tab:: Windows

        #. 下载安装CMake

            打开 `CMake官网下载页面 <https://cmake.org/download/>`_ ，根据您的操作系统选择对应版本下载即可。
            **安装时注意需要勾选** ``Add CMake to the system PATH for all users`` 。


        #. 下载安装Python

            打开 `Python官网下载页面 <https://www.python.org/downloads/>`_ ，根据您的操作系统选择对应版本下载即可。
            **安装时注意需要勾选** ``Add Python to PATH`` 。

        #. 下载Ninja（可选）: (预期可选 暂时不可选)

            打开 `下载页面 <https://github.com/ninja-build/ninja/releases>`_， 根据您的操作系统选择对应版本下载，
            下载后选择一个路径进行解压，并将Ninja当前的路径添加至系统变量PATH中。
        
        #. 验证安装软件的版本

            打开 ``Windows PowerShell`` 或 ``cmd.exe``，检查各个软件版本信息:

            .. code-block:: bash
                
                cmake --version
                python3 --version
                ninja --version

   .. group-tab:: Ubuntu
        .. _install_dependencies_ubuntu:

        NOT SUPPORT FOR NOW!


WCH_IOT_SDK以及Python依赖项安装
*********************************

接下来需要克隆WCH_IOT_SDK，并且安装一些WCH_IOT_SDK所需要的Python依赖项。

.. tabs::

    .. group-tab:: Windows

        #. 克隆WCH_IOT_SDK

            .. code-block:: bash

                git clone https://github.com/Ken-wch/wch_iot_sdk.git

        #. 通过 ``pip3`` 安装Python依赖项

            .. code-block:: bash
            
                pip3 insatll -r ./wch_iot_sdk/script/requirement.txt

        #. 设置WCH_IOT_SDK所需要的环境变量

            打开 **高级系统设置** 中的 **环境变量** ， 
            添加变量 ``WCH_BASE`` 为 ``wch_iot_sdk`` 所在位置，
            添加变量 ``WCH_TOOLCHAIN_PATH`` 为WCH工具链所在位置。
            （暂定）

        #. 导出WCH CMake包

            重新打开一个终端：

            .. code-block:: bash

                cd ./wch_iot_sdk
                python script/app.py wch-export


    .. group-tab:: Ubuntu

        NOT SUPPORT FOR NOW!


编译第一个工程
===============

编译button工程，选择一个系列的芯片，如 ``CH57x``:

.. code-block:: bash

    cd %WCH_BASE%/wch_iot_sdk
    python script/app.py build -p -S ch57x -s samples/basic/button
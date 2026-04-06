Introduction
============

This repository contains the board files and some demos for the [AC7020C](https://www.en.alinx.com/Product/SoC-System-on-Modules/Zynq-7000-SoC/AC7020C.html) board. The examples were tested using Vivado/Vitis 2025.2.

Quickstart
==========

To build the `blink` example, change to the demo's directory, and run (don't forget to source Xilinx's tools if necessary)
```sh
vivado -mode batch -source ../../common/tcl/ac7020c_blink.tcl -tclargs --project_name blink --project_dir vivado
```

This creates a project called `blink/blink.xpr` in the `vivado` folder. To export the XSA file of the hardware, run
```sh
vivado -mode batch -source ../../xil_z7_scripts/vivado/build_plat.tcl -tclargs --p vivado/blink/blink.xpr --o blink --j 8
```

To create and build the Vitis project, enter Vitis in shell mode by running
```sh
vitis -i
```

Then, run
```sh
run ../../xil_z7_scripts/vitis/plat_create_build.py --ws vitis --config config.json
```

Creating the Vivado project
===========================

Each demo has a `tcl` script to build the Vivado project. To create the project, run
```sh
vivado -mode batch -source path/script.tcl -tclargs --project_name proj --project_dir path
```
where
- `path/script.tcl`: the `tcl` script of the example
- `proj`: the name of the new project
- `project_dir`: location to create the new project

To generate the hardware platform for the project, run
```sh
vivado -mode batch -source path/build_hw.tcl -tclargs --p path/proj.xpr --o plat
```
where
- `path/build_hw.tcl`: path to the `build_hw.tcl` in the submodule `xil_z7_scripts/vivado`.
- `--p`: location of the existing project.
- `--o`: name of the (XSA) file. (Default: plat.xsa, example: `path/to/plat` to create `plat.xsa` at `path/to/`)
- `--j`: number of launch jobs. (Default: 4)

Creating the Vitis project
==========================

To create and build the Vitis project, change to the directory of the demo, start Vitis in the shell mode (by running `vitis -i`) and then run
```sh
run path/plat_create_build.py --ws path/ws --config path/to/config.json
```
where
- `path/plat_create_build.py`: the `plat_create_build.py` script in the submodule  `xil_z7_scripts/vitis`.
- `ws`: location to create Vitis project.
- `config`: the json file with the settings for the project.

Notes
=====

- The current `lwip` library does not support the JL2121 Ethernet PHY of the AC7020C board. To get examples using `lwip` to work, one of the following is required:
    - Generate the vitis code, and then copy `common/lwip/xemacpsif_physpeed.c` to `${ws}/${platform}/ps7_cortexa9_0/cpu0/bsp/libsrc/lwip220/src/lwip-2.2.0/contrib/ports/xilinx/netif/`, where `ws` is the workspace of the example and `platform` is the name used to create the platform. This only applies the changes to the project, and if the project is deleted, the changes need to be applied again.
    - Alternatively, you can directly modify the file inside the Xilinx installation directory. In this case, copy `common/lwip/xemacpsif_physpeed.c` to `${xilinx_install_dir}/2025.2/data/embeddedsw/ThirdParty/sw_services/lwip220_v1_3/src/lwip-2.2.0/contrib/ports/xilinx/netif`, where `xilinx_install_dir` is the directory of your Xilinx installation.
  
  Credits: [Lozart](https://github.com/Lozart/LwIP-Driver) and [dkargus](https://github.com/dkargus/LwIP_mod)

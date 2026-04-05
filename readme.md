Introduction
============

This repository contains the board files and some demos for the [AC7020C](https://www.en.alinx.com/Product/SoC-System-on-Modules/Zynq-7000-SoC/AC7020C.html) board.

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

Vivado
======

Each demo has a `tcl` script to builds the Vivado project. To create the project, run
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

Vitis
=====

To create and build the Vitis project, start Vitis in the shell mode (by running `vitis -i`) and then run
```sh
run path/plat_create_build.py --ws path/ws --config path/to/config.json
```
where
- `path/plat_create_build.py`: the `plat_create_build.py` script in the submodule  `xil_z7_scripts/vitis`.
- `ws`: location to create Vitis project.
- `config`: the json file with the settings for the project.

# Spice Fake Generator

This generator is able to create new fake pdn circuits from real ones.

## How to use

First you need to build the project
```
cmake --build ./ build
```

Then you can execute the generator using command line
```
./build/fake-data-generator.exe --source ./netlist.sp --destination ./ --mode 1 --irDropDiff 0.75
```

## CLI Arguments

#### 1. `--help` `-h`

Show help information.

```
fake-data-generator --help
```

#### 2. `--source` or `-s`

Path to source .sp file.
(*Default - ./netlist.sp*)

```
fake-data-generator --source /path/to/source/file.sp
```

#### 3. `--destination` or `-d`

Path to destination folder where will be stored all generated fakes.
(*Default - ./*)

```
fake-data-generator --destination /path/to/destination/folder/
```

#### 4. `--mode` or `-m`

Mode of generator: 
 - `1` - Moves current sources from origin nodes to random ones.
 - `2` - Add new current sources to random nodes.
 - `3` - Increase volume of current sources.
 
(*Default - 1*)

```
fake-data-generator --mode 1
```
#### 5. `--irDropPrecision` or `-irp`

Precision of ir-drop calculation.
(*Default - 1e-8*)

```
fake-data-generator --irDropPrecision 1e-8
```

#### 6. `--maxIterations` or `-mi`

Maximum number of iterations of ir-drop calculation.
(*Default - 100000*)

```
fake-data-generator --maxIterations 100000
```

#### 7. `--irDropDiff` or `-ird`

Expected max difference of fake's pdn's ir-drop values from original pdn.
(*Default - 0.75*)

```
fake-data-generator --irDropDiff 0.75
```

#### 8. `--numOfFakes` or `-nof`

Numb of fake pdn to generate.
(*Default - 10*)

```
fake-data-generator --numOfFakes 10
```

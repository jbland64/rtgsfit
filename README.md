# rtgsfit
Real-Time equilibrium reconstruction code

## Compilation
A .mat datafile will be required with the same variable names of that of the 
global constants specified in constants.h or constants_template.c. This should 
contain all matrices in row major order, with indexing also in row major order 
and starting from 0.  The conda environment is only required for running the 
python tests

```bash
conda create -n rtgsfit python=3.9
pip install -r requirements.txt
cd src
make DATAFILE=<PATH/TO/DATAFILE.mat>
cd ../tests
make
```


## Program Structure
* shared libraries
* dependancy graph

rtgsfit

lapacke.h
cblas.h
math.h 
float.h
stdio.h

## Naming convention
* Global constants are fully capitilised e.g. R_GRID

## Function conventions
* docstring format
* order of inputs outputs


## Grid convention
R_GRID[0, 0], Z_GRID[0, 0] - bottom left of grid i.e. (R_MIN, Z_MIN)
R_GRID[0, N_R], Z_GRID[0, N_R] - bottom right of grid i.e. (R_MAX, Z_MIN)
R_GRID[N_Z, 0], Z_GRID[N_Z, 0] - top left of grid i.e. (R_MIN, Z_MAX)
R_GRID[N_Z, N_R], Z_GRID[N_Z, N_R] - top right of grid i.e. (R_MAX, Z_MAX)

[[ (N_Z - 1, 0), (N_Z - 1, 1)  , ... , (N_Z- 1, N_R -1)  ],
 [ (N_Z - 2, 0), (N_Z - 2, 1)  , ... , (N_Z - 2, N_R -1)],
                    ...
 [ (0, 0)      , (0, 1)       , ... , (0, N_R -1)       ]]
 
## Boundary Convention 
* LTRB
* (R_MIN, Z_MIN) -> (R_MIN, Z_MAX) -> (R_MAX, Z_MAX) -> (R_MAX, Z_MIN) -> (R_MIN, Z_MIN)


 
## Testing
* pytest
* seting up python
* freegs

## To Do
* limit points
* deglss vs dgelsd
* python flux testing
* interpolate hess_rr hess_det
* boundary index convention
* timing
* improve comments code
* write matlab wrapper for code 
* create matlab function that takes pulse and produces the .mat file
* vessel filaments



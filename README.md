# GNUChess for PC/GEOS
The game requires you to set up the PC/GEOS SDK [instructions](https://github.com/bluewaysw/pcgeos).

## How to build (once your SDK is set up):
1. Unpack the `Appl` and `Installed` root level folders to `%ROOT_DIR%`/`$ROOT_DIR`
2. In the terminal, navigate to `Installed/Appl/GNUChess`
3. Then run:
   
`mkmf`

`pmake depend`

`pmake full`

The GNUChess back-end is written in C++ `local.mk` will automatically call a Perl helper script to compile the C++ source into object files. These are then imported into the PC/GEOS Project when `pmake` is invoked.

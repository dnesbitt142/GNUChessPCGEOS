# GNU Chess for PC/GEOS

## Install

Copy one of the four standalone geodes to the target's `WORLD` folder.
Use NC for the standard kernel and EC for the error-checking kernel.

| Language | NC | EC |
| --- | --- | --- |
| English | `dist/English/NC/GNUCHESS.GEO` | `dist/English/EC/GNUCHSEC.GEO` |
| German | `dist/Deutsch/NC/GNUCHESS.GEO` | `dist/Deutsch/EC/GNUCHSEC.GEO` |

The corresponding `.sym` files are for SWAT. Each geode has a built-in
language. An existing `USERDATA/GNUCHESS.VM` overrides it. See
[languages/README.md](languages/README.md) for editing the optional
catalogs. If you used an earlier VM, install its updated counterpart
to refresh the application title and About text.

## Build

With the [PC/GEOS SDK](https://github.com/bluewaysw/pcgeos), Open Watcom
and Perl set up, unpack the `Appl` and `Installed` directories over
the SDK root. Build from `Installed/Appl/GNUChess`.

```sh
mkmf
pmake depend
pmake full
```

The normal build compiles both GEOS geodes and sets the About date from
the compiler. The default build is English. For German, use a clean
build directory and pass `GNUCHESS_LANGUAGE=de` to both `pmake` commands.
See [BUILDING-GNUCHESS.txt](BUILDING-GNUCHESS.txt) for the SDK layout
and [docs/TEST-RESULTS.md](docs/TEST-RESULTS.md) for checks.


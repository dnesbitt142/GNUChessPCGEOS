# GNU Chess language catalogs

Each NC and EC geode includes its own default language. To override it,
exit GNU Chess, copy `English/GNUCHESS.VM` or `Deutsch/GNUCHESS.VM` to
the PC/GEOS `USERDATA` directory, and restart GNU Chess. Keep the filename
`GNUCHESS.VM`. The same file works with all four geodes. Move an
installed override out of `USERDATA` to use the built-in language again.

The catalog supplies GNU Chess's own labels, window title, board hint, fallback
piece initials and engine messages. PC/GEOS supplies system text such as its
automatic Exit menu item and standard dialog buttons in the host locale. The
build date comes from the compiler. File and rank labels and typed move notation
follow the engine's coordinate notation. Use `q/r/b/n` for promotion in either
language.

## Edit a catalog

From the project root, use the included Perl tool to extract and repack the
string table in the GEOS VM map block. The editable text is UTF-8 and the VM
uses the GEOS single-byte character set for English and German.

```sh
perl Appl/GNUChess/tools/compile_lang.pl unpack languages/Deutsch/GNUCHESS.VM strings.txt
# Edit strings.txt.
perl Appl/GNUChess/tools/compile_lang.pl pack strings.txt GNUCHESS.VM
```

The language folders also contain the original `strings.txt` catalogs. Keep all
keys and edit the value after the first `=` on each line. Turn messages use
`%d` for the depth digit and `%m` for the last move, which can be up to five
characters. Keep the formatted status within 119 characters and keep labels
short enough to fit the existing layout. The `PIECES` entry contains 12 initials
in king, queen, rook, bishop, knight, pawn order, first for White and then
for Black.

To update the built-in German catalog from its text file before a
German SDK build, regenerate the checked-in GOC header.

```sh
perl Appl/GNUChess/tools/compile_lang.pl embed languages/Deutsch/strings.txt Appl/GNUChess/language_de.goh
```

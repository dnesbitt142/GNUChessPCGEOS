#!/usr/bin/env perl
# SPDX-License-Identifier: GPL-3.0-or-later
# Compile one GNUChess C++ translation unit for PC/GEOS with Open Watcom.
# Usage: compile_cpp.pl nc|ec source.cpp target.obj|target.eobj

use strict;
use warnings;
use File::Basename qw(basename dirname);
use File::Spec;
use FindBin qw($Bin);

require File::Spec->catfile($Bin, 'omf_normalize.pl');

my ($mode, $source, $target) = @ARGV;
die "usage: compile_cpp.pl nc|ec source.cpp target.obj|target.eobj\n"
    unless defined $target && ($mode eq 'nc' || $mode eq 'ec');

my $root   = $ENV{ROOT_DIR} or die "ROOT_DIR is not set\n";
my $watcom = $ENV{WATCOM}   or die "WATCOM is not set\n";

my $base = basename($source);
$base =~ s/\.cpp$//i or die "$source: expected .cpp source\n";
(my $segbase = $base) =~ s/_//g;
my $segment = 'GC' . uc($segbase);

my $engine = dirname($source);

my @cmd = (
    'wpp',
    '-zq',
    '-fr',                 # diagnostics on console, no .err side file
    '-D__GEOS__',
    '-D__WATCOM__',
    '-D_NO_EXT_KEYS',
    '-w3',
    '-fpc',
    '-zu',
    '-of',
    '-s',
    '-ecc',
    '-zp1',
    '-ei',
    '-zdp',
    '-d0',
    '-hc',
    '-zld',
    '-zl',
    '-ml',
    '-zt512',
    '-3',
    '-ox',
);

push @cmd, '-DDO_ERROR_CHECKING' if $mode eq 'ec';

push @cmd,
    '-i=' . $engine,
    '-i=' . File::Spec->catdir($root, 'Installed', 'CInclude'),
    '-i=' . File::Spec->catdir($root, 'CInclude'),
    '-i=' . File::Spec->catdir($root, 'CInclude', 'Ansi'),
    '-i=' . File::Spec->catdir($watcom, 'h'),
    '-nt' . $segment,
    '-fo=' . $target,
    $source;

print "=== $base.cpp -> $target [$segment", ($mode eq 'ec' ? ' EC' : ''), "] ===\n";

my $rc = system(@cmd);
if ($rc == -1) {
    die "failed to start wpp: $!\n";
}
if ($rc & 127) {
    die sprintf("wpp terminated by signal %d\n", ($rc & 127));
}
my $exit = $rc >> 8;
exit $exit if $exit;

die "$target: wpp reported success but object was not created\n" unless -f $target;

my @changes = normalize_file($target);
print "$target : ", join(',', @changes), "\n";

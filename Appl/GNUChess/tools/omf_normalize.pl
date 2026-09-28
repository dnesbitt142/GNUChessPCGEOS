#!/usr/bin/env perl
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Normalize Open Watcom OMF objects for PC/GEOS Glue.
#
# Watcom C and C++ can emit different SEGDEF alignments for shared named data
# segments. Glue rejects that mixture. This script strengthens the alignment of
# CONST, CONST2, _DATA and _BSS to paragraph (16-byte) alignment and removes the
# empty Watcom debug marker that Glue can mistake for CodeView32 metadata.
#
# The operation is intentionally narrow: no code, fixups, resource/LMem
# segments, or symbol values are modified.

use strict;
use warnings;
use Fcntl qw(:DEFAULT);

my %COMMON = map { $_ => 1 } qw(CONST CONST2 _DATA _BSS);

sub _read_index {
    my ($data, $pos) = @_;
    die "truncated OMF index\n" if $pos >= length($data);
    my $v = ord(substr($data, $pos, 1));
    ++$pos;
    if ($v & 0x80) {
        die "truncated OMF index\n" if $pos >= length($data);
        $v = (($v & 0x7f) << 8) | ord(substr($data, $pos, 1));
        ++$pos;
    }
    return ($v, $pos);
}

sub normalize_file {
    my ($path) = @_;
    die "usage: omf_normalize <object> [object ...]\n" unless defined $path;

    open(my $in, '<:raw', $path) or die "$path: $!\n";
    local $/;
    my $source = <$in>;
    close($in) or die "$path: $!\n";

    my @names = ('');
    my $pos = 0;
    my $out = '';
    my @changes;
    my $has_symbols = index($source, '$$SYMBOLS') >= 0;

    while ($pos < length($source)) {
        die "$path: truncated OMF header\n" if $pos + 3 > length($source);

        my $type = ord(substr($source, $pos, 1));
        my $len  = unpack('v', substr($source, $pos + 1, 2));
        die "$path: invalid OMF record length\n" if $len < 1;

        my $raw_len = 3 + $len;
        die "$path: truncated OMF record\n"
            if $pos + $raw_len > length($source);

        my $raw = substr($source, $pos, $raw_len);
        my $payload = substr($raw, 3, $len - 1); # omit checksum byte

        # Watcom -d0 may emit an empty Microsoft debug-extension marker.
        # Glue can mistake it for CodeView32 and then consume PUBDEF records as
        # debug records. Remove only the two known empty forms, and only when a
        # real $$SYMBOLS stream is absent.
        if ($type == 0x88 && !$has_symbols &&
            ($payload eq "\x80\xa1" || $payload eq "\x80\xe9")) {
            push @changes, 'empty-debug-marker';
            $pos += $raw_len;
            next;
        }

        # LNAMES
        if ($type == 0x96) {
            my $j = 0;
            while ($j < length($payload)) {
                my $n = ord(substr($payload, $j, 1));
                ++$j;
                die "$path: truncated LNAMES record\n"
                    if $j + $n > length($payload);
                push @names, substr($payload, $j, $n);
                $j += $n;
            }
        }

        # SEGDEF / SEGDEF32
        if ($type == 0x98 || $type == 0x99) {
            die "$path: truncated SEGDEF record\n" unless length($payload);
            my $attr = ord(substr($payload, 0, 1));
            my $j = 1;
            $j += 3 if (($attr >> 5) == 0);      # absolute frame+offset
            $j += ($type == 0x99) ? 4 : 2;       # segment length

            my ($name_index, $new_j) = _read_index($payload, $j);
            $j = $new_j;
            die "$path: bad LNAMES index $name_index\n"
                if $name_index > $#names;

            my $name = $names[$name_index];
            my $align = $attr >> 5;
            if ($COMMON{$name} && $align != 3) {
                die "$path: absolute common segment unsupported\n"
                    if $align == 0;

                # First payload byte is byte 3 of the complete record.
                substr($raw, 3, 1, chr(($attr & 31) | (3 << 5)));

                # Recompute OMF checksum: sum of all bytes including checksum
                # must be 0 modulo 256.
                my $body = substr($raw, 0, length($raw) - 1);
                my $sum = 0;
                $sum = ($sum + $_) & 0xff for unpack('C*', $body);
                substr($raw, -1, 1, chr((- $sum) & 0xff));
                push @changes, $name;
            }
        }

        $out .= $raw;
        $pos += $raw_len;
    }

    # Preserve mtime when no byte changed. This keeps incremental pmake builds
    # from relinking needlessly.
    if ($out ne $source) {
        my @st = stat($path);
        open(my $fh, '>:raw', $path) or die "$path: $!\n";
        print {$fh} $out or die "$path: $!\n";
        close($fh) or die "$path: $!\n";
        chmod($st[2] & 07777, $path) if @st;
    }

    return @changes;
}

unless (caller) {
    die "usage: omf_normalize <object> [object ...]\n" unless @ARGV;
    for my $path (@ARGV) {
        my @changes = normalize_file($path);
        print "$path : ", join(',', @changes), "\n";
    }
}

1;

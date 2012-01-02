#! /usr/bin/perl
print << 'EOL' # Copyright notice for this file as well as generated files.
/*  Copyright (C) 2012, Axis Communications AB, LUND, SWEDEN
 *
 *  This file is part of RAPP.
 *
 *  RAPP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  You can use the comments under either the terms of the GNU Lesser General
 *  Public License version 3 as published by the Free Software Foundation,
 *  either version 3 of the License or (at your option) any later version, or
 *  the GNU Free Documentation License version 1.3 or any later version
 *  published by the Free Software Foundation; with no Invariant Sections, no
 *  Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the documentation section entitled
 *  "GNU Free Documentation License".
 *
 *  RAPP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License and a copy of the GNU Free Documentation License along
 *  with RAPP. If not, see <http://www.gnu.org/licenses/>.
 */
EOL
;
# Let's also give a usage warning, except it's not on the first line:
print << 'EOL'
/**
 *  GENERATED FILE -- ANY CHANGES WILL BE OVERWRITTEN.
 *  See gen-rapp-logmacros.pl.
 */
EOL
;

# FIXME: conftest for size_t and uintmax_t; might have to fall back to %lu
# or %u.
sub type2format
{
    my ($x) = @_;
    my $a = "%d";
    $a = "%u" if $x =~ /^(unsigned|uint)/;
    $a = "%zu" if $x =~ /^size_t/;
    $a = "%ju" if $x =~ /^uintmax_t/;
    $a = "%p" if $x =~ /[\*\[]/;
    $a = "" if $x =~ /^void$/;
    $a;
}

$maxargsformatlen = 0;
$maxretformatlen = 0;

while (<>) {
    next unless /RAPP_API\s*\(/;
    # There will be a partial match for any /RAPP_API\s*\(/ line, but that
    # will be trumped by a subsequent intended match, unless of course
    # rapp_api.h is the last file.
    # This regexp needs updating if function arguments (i.e. parentheses in
    # the argument list) are introduced.
    if ($_ !~ /RAPP_API\s*\(([^,]+),([^,]+),\s*\(([^)]+)\)\s*\)/) {
        $_ .= <>;
        die ("$0: invalid partial RAPP_API match: $_\n") if eof();
        redo;
    }

    print "\n";

    $t = $1;
    $f = $2;
    $args = $3;
#    print "t: $t, f: $f, args: $args\n";

    # Clean up whitespace and canonicalize ", " at argument boundaries.
    $t =~ s/\s+//g;
    $f =~ s/\s+//g;
    $args =~ s/^\s+//g;
    $args =~ s/\s+$//g;
    $args =~ s/\s+/ /g;
    $args =~ s/,(\S)/, $1/g;
#    print "t: $t, f: $f, args: $args\n";

    # Output a macro expanding its argument for non-void functions,
    # empty for void functions. Also output a similar macro with a
    # leading comma for non-void functions.
    print "#define RAPP_LOG_IDENTITY_IF_RETURN_$f(x)";
    print " x" if $t ne "void";
    print "\n";

    # Output a macro expanding to a comma-separated list of the
    # parameter names. Also output a similar macro with a leading
    # comma when the list is non-empty.
    @args = split /, /, $args;
    @argnames = map {
            $a = $_;
            $a =~ s/.*\s+[*]*([a-zA-Z0-9_]+)(\[[0-9]*\])*$/$1/;
            $a;
        } @args;

    print "#define RAPP_LOG_IDENTITY_IF_ARGS_$f(x)";
    print " x" if $args ne "void";
    print "\n";
    print "#define RAPP_LOG_ARGLIST_$f";
    print " " . join ", ", @argnames if $args ne "void";
    print "\n";

    # Output a macro expanding to a string suitable to pass to
    # vprintf-type functions for printing the parameter list of a
    # logged function call, with the parameters ", "-separated.
    @argformats = map { type2format($_); } @args;
    $argsformat = join ", ", @argformats;

    printf ("#define RAPP_LOG_ARGSFORMAT_$f \"%s\"\n", $argsformat);

    # Similar for the function return type.
    $retformat = type2format($t);
    printf ("#define RAPP_LOG_RETFORMAT_$f \"%s\"\n", $retformat);

    # Note: the total length of the *strings*, not including \0 terminators.
    $argsformatlen = length ($argsformat);
    $retformatlen = length ($retformat);
    $maxretformatlen = $retformatlen if $retformatlen > $maxretformatlen;
    $maxargsformatlen = $argsformatlen if $argsformatlen > $maxargsformatlen;
}


print "\n";
print "#define RAPP_LOG_MAX_RETURN_FORMAT_LENGTH $maxretformatlen\n";
print "#define RAPP_LOG_MAX_ARGS_FORMAT_LENGTH $maxargsformatlen\n";

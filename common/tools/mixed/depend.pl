#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Framework
#*******************************************************************************
#  Description      This tool formats the dependency list
#                   from the cpp-preprocessor.
#*******************************************************************************
use strict;
use warnings;

my $filename   = $ARGV[0];
my $dependcy_string;
if (not defined $filename) {
  die "File $filename does not exist!\nUsage: depend.pl cppDepFile\n";
  exit;
}
# Read dependencies from cpp file
open(my $filehandle, '<', $filename) or die "Could not open file '$filename'";
my @depend_list = <$filehandle>;
close $filehandle;
# join elements of array to singel string variable
# and remove unnecessary characters
$dependcy_string = join(" ",@depend_list);
$dependcy_string =~ s/\\\n//g;
$dependcy_string =~ s/.*?:\s+//;
$dependcy_string =~ s/\s+/ /g;
print "$dependcy_string\n";


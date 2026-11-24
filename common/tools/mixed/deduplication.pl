#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework
#*******************************************************************************
#  Description      The script deduplicates strings in the file specified by
#                   filename and print the new list to the console
#                   Arguments: filename
#*******************************************************************************

use List::Util 'uniq';

my $filename   = $ARGV[0];

if (not defined $filename) {
  die "File $filename does not exist!\nUsage: deduplicate.pl FileName \n";
  exit;
}
open(FILE, '<', $filename) or die "Could not open file '$filename'";

my @comple_list;

while(<FILE>)
{
   # remove newline character from the line
   chomp;
   # split the line by space character
   push(@comple_list, split /\s+/, $_)
}
close FILE;

my @deduplicated_list = uniq @comple_list;
foreach my $item (@deduplicated_list) {
   # printing strings to separate line
   print "$item\n";
}


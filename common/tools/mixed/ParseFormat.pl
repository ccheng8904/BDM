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
#  Description      Parses the format output and puts results on stdout
#*******************************************************************************
use strict;
use warnings;
use File::Basename;

# open the file given as parameter
my $format_file_with_path   = $ARGV[0];
if (not defined $format_file_with_path) {
  die "File $format_file_with_path does not exist!\nUsage: ParseFormat.pl Format_File\n";
  exit;
}

#my $format_file_path   = $ARGV[1];
#open(FH, ">>", $format_file_path) or die "File couldn't be opened"; 
# Appending the content to file 
#print FH $format_file_with_path; 
#print FH "\n";
#close FH;

my $source_file_name = $ARGV[1];

my($format_filename, $dirs, $suffix) = fileparse($source_file_name, qr/\.[^.]*/);
 # open file for reading and put content of file into buffer
open(my $filehandle, '<', $format_file_with_path) or die "Could not open file '$format_file_with_path'";
my @buffer = <$filehandle>;
close $filehandle;

my $Warnings = undef;
my $string = join(" ",@buffer);

# count FormatCount
my $FormatCount = ($string =~ s/\swarning:\s//g);

if($FormatCount > 0){
    $Warnings = "!!  ";
}
else {
    $Warnings = ":-) ";
}

# puts results to stdout
printf ("%-40s  %20s         Format violations count.: %4d \n", $format_filename.$suffix, $Warnings, $FormatCount);



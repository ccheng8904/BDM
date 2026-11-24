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
#  Description      Parses the axmetric output and puts results on stdout
#                   The tool takes one argument: name of the mstc-file
#*******************************************************************************
use strict;
use warnings;
use File::Basename;

# open the file given as parameter
my $full_file_name   = $ARGV[0];
if (not defined $full_file_name) {
  die "File $full_file_name does not exist!\nUsage: ParseMetric.pl Axivion_File\n";
  exit;
}

#Store MSTC file path in to axivion out file for CI Processing
my $axivion_paths_file_name   = $ARGV[1];
open(FH, ">>", $axivion_paths_file_name) or die "File couldn't be opened"; 
# Appending the content to file 
print FH $full_file_name; 
print FH "\n";
close FH;

#parse only file name from full path,file name and extension
my($filename, $dirs, $suffix) = fileparse($full_file_name, qr/\.[^.]*/);

 # open file for reading and put content of file into buffer
open(my $filehandle, '<', $full_file_name) or die "Could not open file '$full_file_name'";
my @buffer = <$filehandle>;
close $filehandle;
# calculation of numbers axivion outputs
my $Warnings = undef;
my $string = join(" ",@buffer);

# count Statement metric
my $StatementCount = ($string =~ s/\sSTATEMENTS:\s//g);

# count complexity metric
my $ComplexityCount = ($string =~ s/\sCOMPLEXITY:\s+//g);

# count lines metric
my $LinesCount = ($string =~ s/\sLINES_IN_FILE:\s+//g);

#Count Bmi count
my $BmiCount = ($string =~ s/\Bosch Maintainability Index:\s+//g);


if($StatementCount > 0){
    $Warnings = "!!  ";
}
elsif ($ComplexityCount > 0){
    $Warnings = "!   ";
}
elsif ($LinesCount > 0){
    $Warnings = "    ";
}
else {
    $Warnings = ":-) ";
}

printf ("Creating metric for %-30s %4s   Statements.:%4d   Complexity.:%4d   Lines in file.:%4d   BMI.:%4d\n", $filename, $Warnings, $StatementCount, $ComplexityCount, $LinesCount, $BmiCount);

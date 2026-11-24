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
#  Description      Parses the axsyle output and puts results on stdout
#                   The tool takes one argument: name of the ustc/nstc-file
#*******************************************************************************
use strict;
use warnings;
use File::Basename;
use IO::Handle;

# open the file given as parameter
my $full_file_name   = $ARGV[0];
if (not defined $full_file_name) {
  die "File $full_file_name does not exist!\nUsage: ParseAxivion.pl Axivion_File\n";
  exit;
}

#Store USTC file path in to axivion out file for CI Processing
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

# count Misra mandatory
my $MisraManCount = ($string =~ s/\s+mandatory\s+//g);
# count Misra required
my $MisraReqCount = ($string =~ s/\s+required\s+//g);
# count Misra advisory
my $MisraAdvCount = ($string =~ s/\s+advisory\s+//g);


if($MisraManCount > 0){
    $Warnings = "!!  ";
}
elsif ($MisraReqCount > 0){
    $Warnings = "!   ";
}
elsif ($MisraAdvCount > 0){
    $Warnings = "    ";
}
else {
    $Warnings = ":-) ";
}


# puts results to stdout
my $result = sprintf ("%-30s %4s MISRA man.:%4d MISRA req.: %4d MISRA adv.: %4d\n", $filename, $Warnings, $MisraManCount, $MisraReqCount, $MisraAdvCount);
STDOUT->printflush($result);

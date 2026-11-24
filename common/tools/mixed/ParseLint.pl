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
#  Description      Parses the lint output and puts results on stdout
#                   The tool takes one argument: name of the lnt-file
#*******************************************************************************
use strict;
use warnings;
use File::Basename;
use IO::Handle;

# open the file given as parameter
my $full_file_name   = $ARGV[0];
if (not defined $full_file_name) {
  die "File $full_file_name does not exist!\nUsage: ParseLint.pl lin_File\n";
  exit;
}

#parse only file name from full path,file name and extension
my($filename, $dirs, $suffix) = fileparse($full_file_name, qr/\.[^.]*/);

 # open file for reading and put content of file into buffer
open(my $filehandle, '<', $full_file_name) or die "Could not open file '$full_file_name'";
my @buffer = <$filehandle>;
close $filehandle;
# calculation of numbers lint outputs
my $Warnings = undef;
my $string = join(" ",@buffer);
# count Infos
my $InfoCount = ($string =~ s/\s+Info\s+//g);
# count Errors
my $ErrorCount = ($string =~ s/\s+Error\s+//g);
# count Warnings
my $WarningCount = ($string =~ s/\s+Warning\s+//g);
# count Misra mandatory
my $MisraManCount = ($string =~ s/\s+mandatory\]//g);
# count Misra required
my $MisraReqCount = ($string =~ s/\s+required\]//g);
# count Misra advisory
my $MisraAdvCount = ($string =~ s/\s+advisory\]//g);


if($ErrorCount > 0 or $MisraManCount > 0){
    $Warnings = "!!  ";
}
elsif ($MisraReqCount > 0 or $WarningCount > 0){
    $Warnings = "!   ";
}
elsif ($MisraAdvCount > 0){
    $Warnings = "    ";
}
else {
    $Warnings = ":-) ";
}


# puts results to stdout
my $result = sprintf ("%-30s %4s Errors:%4d MISRA man.:%4d MISRA req.: %4d Warnings: %4d MISRA adv.: %4d Infos: %4d\n", $filename, $Warnings, $ErrorCount, $MisraManCount, $MisraReqCount, $WarningCount, $MisraAdvCount, $InfoCount);
STDOUT->printflush($result);

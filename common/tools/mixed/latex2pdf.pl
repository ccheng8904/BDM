#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT      MCU Framework
#*******************************************************************************
#  Description: This tool creates the document refman.pdf from refman.tex 
#               using pdflatex and makeindex. The pdflatex is called up to
#               8 times to solve all open references.
#               Look into the a logfile pdflatex.log in any problems with
#               creating the pdf file.
#
# usage: latex2pdf  -i input_tex_filename -o output_pdf_filename -d output_directory
#******************************************************************************
use strict;
use warnings;
use 5.24.0;
use Getopt::Long qw(GetOptions);
use File::Basename;
use File::Copy;
use Cwd;
use Cwd 'abs_path';

# Variables
# input file for pdflatex tool, usually refman.tex
my $input_file;
# output file from pdflatex tool, usually refman.pdf
my $output_file;
# destination file - usually specified by project 
my $destination_file;
# destination folder - usually specified by project 
my $destination_folder;
# maximal number of re-run of pdflatex tool
my $latex_runs_count_max=8;
# output directory for latex
my $latex_out_dir;
# list of file types related to latex tool
my @type_files =qw(*.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl);
# only for testing, it can be removed in final version
my $test;
my $log_file;

# get parameters from command line and store them in corresponding variables
# parameters have two forms: 
# - short form - a minus sign and one character e.g. -i
# - long form  - a double minus sign and word e.g. --input_file
GetOptions(
    'input_file=s' => \$input_file,
    'output_file=s' => \$output_file,
    'path_latex=s' => \$latex_out_dir,
    'destination_file=s' => \$destination_file,
    'folder=s' => \$destination_folder,
    'test' => \$test
    ) or die "Usage: $0 -i input_tex_filename -o output_pdf_filename -p latex_out_directory -d destination_pdf_file -f destination_folder\n";

# get the base name of input_file and use it as name of log file
my ($base,$path,$suffix) = fileparse($input_file, qr/\.[^.]*/);
$log_file = $base.".log";

if($test){
#start TESTING/DEBUGING - could be removed
    print "Called script     : $0\n";
    print "Input file        : $input_file\n";
    print "Output file       : $output_file\n";
    print "Destination file  : $destination_file\n";
    print "Destination folder: $destination_folder\n";
    print "Input file base   : $base\n";
    print "Input file path   : $path\n";
    print "Log file          : $log_file\n";
    print "Latex directory   : $latex_out_dir\n";
    print "CWD is            : ".getcwd()."\n";
#end TESTING/DEBUGING - could be removed
}

# Remove all types of old files from output directory
foreach (@type_files){
    unlink glob($latex_out_dir."/".$_);
}
unlink $latex_out_dir."refman.pdf";

# remember original working directory 
my $previous_dir = getcwd();
# Changing of working directory is necessary. The reason of that is a fact that
# TeX binaries don't allow  writing files specified with absolute paths or above
# the working directory. The -output-directory option allows only to specify 
# a directory below the working one. It is for security reason.
chdir "$latex_out_dir";

if($test){
#start TESTING/DEBUGING - could be removed
    print "CWD is          : ".getcwd()."\n";
#end TESTING/DEBUGING - could be removed
}

# Executing pdflatex tool
print "Starting pdflatex tool ...\n";
system("pdflatex -interaction=nonstopmode $input_file > pdflatex.log");
system("makeindex $input_file >> pdflatex.log");
system("pdflatex -interaction=nonstopmode $input_file >> pdflatex.log");
# read log file created by pdflatex and put content into variable data
my $data = slurp($log_file);
# initialize counter of pdflatex runs to zero
my $latex_runs_count = 0;
# repeat runs of pdflatex until max count of runs is reached and log file contains 
# string "Rerun" as indiator that not all references were solved
while ($data =~ m/Rerun/ && $latex_runs_count_max < $latex_runs_count) {
    print "Re-runing pdflatex ...$latex_runs_count\n";
    system("pdflatex -interaction=nonstopmode $input_file >> pdflatex.log");
    $latex_runs_count += 1;
    $data = slurp($log_file);
}
# run pdflatex once again - official recommendation
system("makeindex $input_file >> pdflatex.log");
system("pdflatex -interaction=nonstopmode -output-directory=$latex_out_dir $input_file >> pdflatex.log");
print "Pdflatex tool is done.\n";
if (-e $output_file)
{
    print "The file $output_file was created.\n";
} else
{
    print "The file $output_file was not created! Look into refman.log file.\n";
    exit 1;
}
# change back to original working directory at the end of pdflatex work
chdir "$previous_dir";

if($test){
#start TESTING/DEBUGING - could be removed
    print "CWD is          : ".getcwd()."\n";
#end TESTING/DEBUGING - could be removed
}

# prepare source file name and destination file name for copy command including paths
my $source_file_abs_path = abs_path($latex_out_dir.$output_file);
my $destination_file_rel_path = $destination_folder.$destination_file;

if($test){
#start TESTING/DEBUGING - could be removed
    print "Source file with absolute path: $source_file_abs_path\n";
    print "Destination file with relative path: $destination_file_rel_path\n";
#end TESTING/DEBUGING - could be removed
}
# copy pdflatex output file from latex directory to a destination directory with new filename
copy("$source_file_abs_path", "$destination_file_rel_path") or die "Copy $output_file to target $destination_file failed.\n";
if (-e $destination_file_rel_path)
{
    print "A creation of file $destination_file was finished.\n";
}

# Function slurp opens file and read it and return its content to variable 
# without any EOL characters
sub slurp {
    my $file = shift;
    open(my $fh, '<', $file) or die "Could not open file '$file' $!\n"; 
    local $/ = undef;
    my $content = <$fh>;
    close $fh;
    return $content;
}

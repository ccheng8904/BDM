#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT         MCU Framework
#*******************************************************************************
#  Description    This tool filters the created dependency list.
#                 It is able to get all header files in specified folders.
#                 This is useful to get a header list for doxygen.
#                 Normally doxygen documents all files in specified folders
#                 If no search folder is defined than all headers are extracted
#                 usage: scan_dep -fdepfile -dfolder1 -dfolder2
#*******************************************************************************
use strict;
use warnings;
use 5.24.0;
use Getopt::Long qw(GetOptions);
#Getopt::Long::Configure qw(gnu_getopt);
#use File::Spec;


use Cwd qw(cwd);
use Cwd 'abs_path';
# Dumper is used only for debuging
use Data::Dumper qw(Dumper);

my $dependency_file;
my $dir;
my $excluded_files;
my $filehandle;
# variable $list contains list all words form dependency_file
my $list;
my @header_list;
my @folder_list;
# variable contains final list without items filtered by items from array excluded_files
my @without_excluded;
my @words;
# only for debuging, it can be removed in final version
my $debug;
# only for testing, it can be removed in final version
my $test;

# get parameters from command line and store them in corresponding variables
# parameters have two forms: 
#           short form consists from minus sign and one character e.g. -f
#           long form consists from double minus sighn and word e.g. --filedep
GetOptions(
    'filedep=s' => \$dependency_file,
    'dir=s@' => \$dir,
#   'exclude=s@' => \$excluded_files,
    'debug' => \$debug,
    'test' => \$test
    ) or die "Usage: $0 --filedep dependency_file_name --exclude list_of_exlcuded_files --dir folder1 folder2 ...\n";
# create excluded file list from arguments passed after parameters
my @excluded_files = @ARGV;
# add absolute path for all files in the list and make them lowercased
foreach(@excluded_files){$_ =lc(abs_path($_));}
# append directories to the list if they exist
# TODO rewrite to have more then 2 directories e.g. --dir Folder1 Folder2 Folder3 ...
if($test){
#start TESTING/DEBUGING - could be removed
    print "Directory list:\n";
    foreach (@$dir){ print $_."\n";}
    print "Called script $0\n";
    print "Dependency file: $dependency_file\n";
    print "Arguments file list:\n";
    print Dumper \@ARGV;
    print "Excluded files:\n";
    print Dumper \@excluded_files;
    print "Excluded files as list:\n";
    foreach (@excluded_files){print  $_."\n";}
#end TESTING/DEBUGING - could be removed
}
# get the list of filenames separated by space
$list = slurp ($dependency_file);
# split one big string $list into array by space
# every item of array is one filename with relative path
@words = split / /, $list;
if($debug){
# only for debuging
    print "Splitted: \n";
    print Dumper \@words;
}
# filter out only h-files
my @filtered = grep(/.+\.h/, @words);
if($debug){
# only for debuging
    print "Filtered (only h-files): \n";
    print Dumper \@filtered;
}
# change path to the cannonical pathname for every filename
foreach (@filtered){
# symbolic links and relative-path components ("." and "..") 
# are resolved to return the canonical pathname,
# just like realpath(3). On error returns undef,
# with $! set to indicate the error.
    $_ = abs_path($_);
}
if($debug){
# only for debuging
    print "Filtered with absolute path: \n";
    print Dumper \@filtered;
}

# make list unique
my @unique = uniq_no_case(@filtered);
if($debug){
# only for debuging
    print "Unique: \n";
    print Dumper \@unique;
}
#use locale;
# sort function is not used because does not ignore case 
#my @sorted_words = sort @filtered;
# now case-insensitively version 1 - not work
#my @sorted_words = sort {fc($a) cmp fc($b)} @unique;
# now case-insensitively version 2 - not work
#sub case_insensitive { "\L$a" cmp "\L$b" }
#my @sorted_words = sort case_insensitive @unique;
# now case-insensitively version 3 
my @sorted_words = sort {lc($a) cmp lc($b)} @unique;
# remove those array items that matching with items come from excluded_files array 
@without_excluded = grep { my $f = $_; ! grep $_ eq $f, @excluded_files } @sorted_words;

if($debug){
# only for debuging
    print "Sorted: \n";
    print Dumper \@sorted_words;
    print "List without excluded: \n";
    print Dumper \@without_excluded;}
if($test){
    print "Raw output: \n";
}
# real output
print "@without_excluded\n";


# function slurp will get the name of the file and
# it returns the content as a single string.
sub slurp {
    open(my $fh, $dependency_file) or die "Could not open file '$dependency_file' $!";
# reading file not stops at new line, it will continue to the EOF
    local $/ = undef;
# move all content of file to scalar variable
    my $data = <$fh>;
# replace CRLF from string by spaces
    $data =~ s/\r\n/ /g;
    close $fh;
    return $data;
}

# function uniq is basically a filter.
# You provide an array on the right hand side and an expression in the block.
# The grep function will take each value of the array one-by-one,
# put it in $_, the default scalar variable of Perl and then execute the block.
# If the block evaluates to TRUE, the value can pass. 
# If the block evaluates to FALSE the current value is filtered out. 
sub uniq { my %seen; grep !$seen{$_}++, @_ }
# Explanation of how this works:
# @_ contains the args to the subroutine, and they are fed to a grep statement.
# Any elements that return true when passed through the code block are returned by 
# the grep statement. The code block consist of a few finer points:
#
#    $seen{$_}++ returns 0 the first time an element is seen. The value is still 
#  incremented to 1, but after it is returned (as opposed to ++$seen{$_} who 
#  would inc first, then return).
#    By negating the result of the incrementation, we get true for the first key,
#  and false for every following such key. Hence, the list is deduped.
#    grep as the last statement in the sub will return a list, which in turn is
#  returned by the sub.
#    map lc, @_ simply applies the lc function to all elements in @_
sub uniq_no_case { my %seen; grep { not $seen{$_}++ } map lc, @_ }


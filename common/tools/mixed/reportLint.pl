#!/usr/bin/perl
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
#  Description      Parses the lint output files and prints a sorted summery
#                   The tool takes one argument: directory of the file with *.lin files
#*******************************************************************************
use strict;
use warnings;

# starting directory
my $directory = $ARGV[0];

# recursive search of files
my @files = get_files_recursive($directory);    # get all the files in directory
my @lines = merge_lin_files(@files);            # merge all lint lines in an array
my %lintData = parse_lint_data(@lines);         # parse the lint-lines into a data hash
my @lintArray = hash_to_array(%lintData);       # convert data to sorted array

# print the data tab separated
foreach (@lintArray) {
  print "$_->{note}\t$_->{count}\t$_->{message}\t$_->{misra}\n";
}
my $lint_lines = scalar @lines;
print "\n$lint_lines lint lines parsed\n";
  


# get recursive all files in specified folder
sub get_files_recursive {
    my ($dir) = @_;
    my @files;

    opendir(my $dh, $dir) or die "Could not open folder $dir!";
    while (my $entry = readdir($dh)) {
        next if ($entry eq '.' or $entry eq '..');

        my $path = "$dir/$entry";
        if (-d $path) {
            push @files, get_files_recursive($path);
        } else {
            push @files, $path;
        }
    }
    closedir($dh);

    return @files;
}

sub hash_to_array
{
    my %hash  = @_;
    my @array;
    foreach my $key (keys %hash)
    {
        push @array, {note => $key, count => $hash{$key}{count}, message => $hash{$key}{message}, misra => $hash{$key}{misra}};
    }
    @array = sort { $b->{count} <=> $a->{count} } @array; 
    return @array;
}

sub merge_lin_files
{
    my @file = @_;
    my @lines;
    foreach my $file (@files)
    {
        if ($file =~ /\.lin$/)
        { # do for lin-files only
            open(my $fh, '<', $file) or die "Konnte Datei $file nicht öffnen: $!";
            while (my $line = <$fh>) {
                push @lines, $line;
            }
            close($fh);
        }
    }
    return @lines;
}

sub parse_lint_data
{
    my @lines = @_;
    my %data;
    my $key = '';
    my $message = '';
    my $misra = '';
    foreach my $line (@lines)  # read line by line of merged content
    {#  match the line with pattern:           "lint: file.c  176    8        Info                       760  redun..[MISRA"
        if(($key, $message, $misra) = $line =~ /lint:\s+\S+\s+\d+\s+\d+\s+((?:Warning|Note|Error|Info)\s+\d+):(.+?)(\[MISRA.+)?$/)
        {
            $message =~ s/'.+?'/'xxx'/g; # replace e.g. 'srv::Stream' by 'xxx'
            $message =~ s/\s*$//g;       # remove spaces and LF at line end
            $misra ||= '';

            if(exists $data{$key})
            {
                $data{$key}{count}++;
            }
            else
            {
                $data{$key}{count} = 1;
                $data{$key}{message} = $message;
                $data{$key}{misra}   = $misra;
            }

        }
    }
    return %data;
}
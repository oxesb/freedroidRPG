#!/usr/bin/perl
#
# Script based on a macosx packager from http://www.stellarium.org/
# under GPL General Public License
#
# Note: Stellarium is now (as of April 2019) using a python
# script, probably worth replacing this old perl script.
#

use strict;
use Cwd;

my $inmt = "/usr/bin/install_name_tool";
my $ch_inmt = qq{$inmt -change \%s \%s \%s};
my $id_inmt = qq{$inmt -id \%s \%s};

my $otool = qq{/usr/bin/otool -L \%s};

my $appdir = shift(@ARGV);
chdir $appdir;
my $main_executable = shift(@ARGV);
my $frameworks_dir = shift(@ARGV);

&recurse( $main_executable, $frameworks_dir );

sub recurse {
    my($main_executable, $frameworks_dir) = @_;

    my $cmd1 = sprintf($otool, $main_executable);
    my($app, @names) = `$cmd1`;

    my $name;
  NAME_LOOP:
    foreach $name ( @names ) {
	chomp($name);
	$name =~ s,^\s*,,;
	$name =~ s,\s*\(compa.+$,,;

	## we've already dealt with this name on this executable, ie, itself
	if ( index($name, $main_executable) >= 0 ||
	     index($main_executable, $name) >= 0 ) {
	    next NAME_LOOP;
	}

	## leave sys libraries alone and don't include them
	if ( $name =~ m,^(/System/Library|/usr/lib|\@executable_path), &&
	     $name !~ m,^(/usr/lib/libiconv), ){ 
	    next NAME_LOOP; 
	}

	## a rooted dylib
	if ( $name =~ m,^/.+?([^/]+\.dylib)$, ) {
	    my $basename = $1;
	    my $absname = $name;

	    my $relPath = "\@executable_path/../Frameworks/$basename";
	    my $fwPath = "$frameworks_dir/$basename";

	    my $not_existed = 1;
	    if ( ! -e $fwPath ) {
	        my $c = "cp -P $absname $frameworks_dir";
                #print "$c\n";
	        `$c`;
	    } else {
	        $not_existed = 0;
	    }

	    my $c = sprintf($id_inmt, $relPath, $fwPath);
            #print "$c\n";
	    `$c`;
	    $c = sprintf($ch_inmt, $absname, $relPath, $main_executable);
            #print "$c\n";
	    `$c`;

	    if ( $not_existed ) {
	        &recurse($fwPath, $frameworks_dir);
	    }
            next NAME_LOOP;
	}

	## something else?
	warn qq{$0: [3] for $main_executable: what to do with $name?!?!?\n};
    }
}

#!/usr/bin/env perl
use strict;
use warnings;

my @files = sort glob("*.mxvm");
my ($test_passed, $test_failed) = (0, 0);
my $current_path = $ARGV[0] // "/usr/local/lib";
my @failed_files = ();

foreach my $f (@files) {
  print ">>> Testing $f\n";
  my $rc2 = system("mxvmc", "--path", $current_path, "--dry-run", $f);
  if ($rc2 == -1) {
    warn "Failed to exec mxvmc (run) for $f: $!\n";
    push @failed_files, $f;
    $test_failed++;
  } elsif ($rc2 & 127) {
    warn sprintf("Run stage for %s died with signal %d\n", $f, ($rc2 & 127));
    $test_failed++;
    push @failed_files, $f;
  } else {
    my $exit2 = $rc2 >> 8;
    if ($exit2 != 0) {
      warn "Run stage failed for $f (exit $exit2)\n";
      $test_failed++;
      push @failed_files, $f;
    } else {
      print "test for $f passed!\n";
      $test_passed++;
    }
  }
}
print "Test passed: $test_passed  Test failed: $test_failed\n";
if($test_failed > 0) {
    print "Files {\n ";
    foreach my $i  (@failed_files) {
        print "$i ";
    }
    print "\n}\n Failed.";
}

exit($test_failed ? 1 : 0);

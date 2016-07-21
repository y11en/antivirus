#!/usr/bin/perl
#

# ---------------- Configuration -------------------------------

# Release storage FTP server
my $ftp_host_pkg = '';
my $ftp_dir_pkg  = '';
my $ftp_user_pkg = '';
my $ftp_pass_pkg = '';


# Dump storage FTP server
my $ftp_host_dump = '';
my $ftp_dir_dump  = '';
my $ftp_user_dump = '';
my $ftp_pass_dump = '';

use strict;
use warnings;
use Net::FTP;

# ---------------- Logging -------------------------------------


sub PrintLog
{
	my $format = shift(@_);

	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	my $dateTime = sprintf("%04u.%02u.%02u %02u:%02u:%02u", $year + 1900, $mon + 1, $mday, $hour, $min, $sec);

	printf("<$dateTime> $format", @_);
}

# ---------------- FTP -----------------------------------------

sub CreateDumpDir
{
  my $dump_dir = $_[0];

  my $dump_ftp = Net::FTP->new($ftp_host_dump);
  unless(defined($dump_ftp))
  {
    PrintLog("ERROR: cannot connect to $ftp_host_dump: $@\n");
    return 1;
  }
  unless($dump_ftp->login($ftp_user_dump,$ftp_pass_dump))
  {
    PrintLog("ERROR: cannot login to $ftp_host_dump: $dump_ftp->message\n");
    return 1;
  }
  unless($dump_ftp->cwd($ftp_dir_dump))
  {
    PrintLog("ERROR: cannot cd to $ftp_dir_dump: $dump_ftp->message\n");
    return 1;
  }
  my @dirlist = $dump_ftp->ls();
  foreach my $ftpdir(@dirlist)
  {
    if($ftpdir eq $dump_dir)
    {
      PrintLog("WARNING: dump directory already exists: $dump_dir\n");
      return 0;
    }
  }

  unless($dump_ftp->mkdir($dump_dir))
  {
    PrintLog("ERROR: cannot create dump directory: $dump_dir: $dump_ftp->message\n");
    return 1;
  }

  return 0;
}

sub PublishPackage
{
  my $local_pkg_name  = $_[0];
  my $remote_pkg_name = $_[1];
  my $remove_spec = $_[2];

  my $pkg_ftp = Net::FTP->new($ftp_host_pkg);
  unless(defined($pkg_ftp))
  {
    PrintLog("ERROR: cannot connect to $ftp_host_pkg: $@\n");
    return 1;
  }
  unless($pkg_ftp->login($ftp_user_pkg,$ftp_pass_pkg))
  {
    PrintLog("ERROR: cannot login to $ftp_host_pkg: $pkg_ftp->message\n");
    return 1;
  }
  unless($pkg_ftp->cwd($ftp_dir_pkg))
  {
    PrintLog("ERROR: cannot cd to $ftp_dir_pkg: $pkg_ftp->message\n");
    return 1;
  }
  my @dirlist = $pkg_ftp->ls($remove_spec);
  foreach my $old_pkg(@dirlist)
  {
    PrintLog("Removing: $old_pkg\n");
    unless($pkg_ftp->delete($old_pkg))
    {
      PrintLog("ERROR: cannot remove $old_pkg: $pkg_ftp->message\n");
    }
  }

  PrintLog("Uploading: $remote_pkg_name\n");
  unless($pkg_ftp->put($local_pkg_name, $remote_pkg_name))
  {
    PrintLog("ERROR: cannot upload package: $remote_pkg_name: $pkg_ftp->message\n");
    return 1;
  }

  return 0;
}

# ---------------- Main ----------------------------------------

if($#ARGV < 1)
{
  PrintLog("required parameters missing, use RELEASE_NO PACKAGE_FILE\n");
  exit(1);
}

my $release_no = $ARGV[0];
my $package_file = $ARGV[1];
my $err;

PrintLog("Release no: $release_no, package file: $package_file\n");

$err = CreateDumpDir($release_no);
if($err != 0)
{
  exit($err);
}

$err = PublishPackage($package_file, "kis8.en.$release_no.msi", "kis8.en.*.msi");
if($err != 0)
{
  exit($err);
}

exit(0)


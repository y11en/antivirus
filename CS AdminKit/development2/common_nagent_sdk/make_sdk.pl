

use File::Find;
use Cwd;

# constants 
    my $Src_Dir = "../include";
    my $Dst_Dir = "./include";
    my $InitialCurrentDir = getcwd;
    my $szDefPublicHeader = "KLCSAK_PUBLIC_HEADER";
    my $szDefPrivBegin = "KLCSAK_BEGIN_PRIVATE";
    my $szDefPrivEnd = "KLCSAK_END_PRIVATE";
# global vars
    my $szLastDstDir = '';
    
sub process_header
{   
    ($basename, $fullname) = @_ ;
    $dst_path = $fullname;
    $dst_path =~ s/$Src_Dir/$Dst_Dir/;
    #print "Processing file '$fullname' into '$dst_path'\n";
    ;    
    my $tempCurrentDir = getcwd;
    chdir($InitialCurrentDir);
    ;
    my $hSrc;
    my $hDst;
    my $bDeleteDest = 1;
    open($hSrc, "<", $fullname)
        or die "Couldn't open $fullname for reading: $!\n";    
    open($hDst, ">", $dst_path)
        or die "Couldn't open $dst_path for writing: $!\n";
    
    my $bProcessingOn = 1;
    my $nCount = 0;
    while ($line = <$hSrc>)
    {
        if( $nCount < 32 && 
            $bDeleteDest != 0 && 
            $line =~ m/^(\s*)(\/\*)(\s*)$szDefPublicHeader(\s*)(\*\/)(\s*)$/s )
        {
            $bDeleteDest = 0;
            print "Header '$fullname' is public\n";
        }
        if($bProcessingOn != 0)
        {
            if($line =~ m/^(.+?)?$szDefPrivBegin(.*)$/s)
            {
                # print "Processing off\n";
                $bProcessingOn = 0;
            }
            else
            {
                if($line =~ m/^(\s*)(\/\*)(\s*)$szDefPublicHeader(\s*)(\*\/)(\s*)$/s)
                {
                    ; #do nothing
                }
                else
                {
                    print $hDst "$line";
                }
            }
        }
        else
        {
            if($line =~ m/^(.+?)?$szDefPrivEnd(.*)$/s)            
            {
                # print "Processing on\n";
                $bProcessingOn = 1;
            }
        }
        $nCount = $nCount + 1;
    };
    
    #do processing here
    close($hSrc);
    close($hDst);
    
    if($bDeleteDest != 0)
    {
        unlink($dst_path);
    }
    
    chdir($tempCurrentDir);    
}

sub clear_last_dir
{
    ($newval) = @_ ;
    my $tempCurrentDir = getcwd;
    chdir($InitialCurrentDir);
    if(($szLastDstDir cmp '') != 0)
    {
        rmdir($szLastDstDir);# try to delete previous dir
    };
    $szLastDstDir = $newval;
    chdir($tempCurrentDir);
};

sub process_fso
{
    my $fullname = $File::Find::name . (-d && "/");
    my $basename = $_;
    if(-d)
    {
        if( $fullname =~ m/^$Src_Dir\/(((std)|(avp)|(kca)|(transport)|(nagent)|(common))\/(.*))?$/i )
        {            
            $dst_path = $fullname;
            $dst_path =~ s/$Src_Dir/$Dst_Dir/;
            #print "Processing directory '$fullname' into '$dst_path'\n";
                        
            my $tempCurrentDir = getcwd;
            chdir($InitialCurrentDir);
            mkdir($dst_path);
            chdir($tempCurrentDir);
            clear_last_dir($dst_path);
        }
        else
        {
            $File::Find::prune = false;
            #print "Prune '$fullname', '$basename'\n";
        }
    }
    else
    {
        if($basename =~ m/^(.)+\.h$/i)
        {
            #print "Processing file $fullname\n";
            process_header($basename, $fullname)
        };
    }
    #print $File::Find::name, -d && "/", "\n"
    #print -d, "\n"; #" base= $_ , dir=$File::Find::dir, $File::Find::name\n";
}

find(\&process_fso, $Src_Dir);
clear_last_dir("");

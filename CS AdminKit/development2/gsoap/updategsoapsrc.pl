# updategsoapsrc.pl
# Update already compiled gSOAP source files
# D.Shiyafetdinov 2005

use strict;
use File::Copy;

my $outSrc = 'soapC.cpp.new';
open outFile, '>'.$outSrc or die "Error open output file: $!";

my $inputSrc = 'soapC.cpp'; 
open inputFile, '<' . $inputSrc or die "Error open input file: $!";
my @str = <inputFile>; 		

my @oldInputFileStrings = ( 	
	'if \(soap_in_xsd__klft_file_chunk_buff\(soap, "buff", \&a->buff, "xsd:klft-file-chunk-buff"\)\)',
	'soap_out_xsd__klft_file_chunk_buff\(soap, "buff", -1, \&a->buff, ""\);',
	'if \(soap_in_xsd__klrpt_output\(soap, "output", \&a->output, "xsd:klrpt-output"\)\)',
	'soap_out_xsd__klrpt_output\(soap, "output", -1, \&a->output, ""\);'
	);

my @newOutputFileStrings = ( 	
	'// by updategsoapsrc.pl if (soap_in_xsd__klft_file_chunk_buff(soap, "buff", &a->buff, "xsd:klft-file-chunk-buff"))' . "\n\t\t\t\t" . 'if (soap_in_xsd__klft_file_chunk_buff(soap, "buff", &a->buff, "xsd:unsignedByte"))',
	'// by updategsoapsrc.pl soap_out_xsd__klft_file_chunk_buff(soap, "buff", -1, &a->buff, "");' . "\n\t" . 'soap_out_xsd__klft_file_chunk_buff(soap, "buff", -1, &a->buff, "xsd:unsignedByte");',
	'// by updategsoapsrc.pl if (soap_in_xsd__klrpt_output(soap, "output", &a->output, "xsd:klrpt-output"))' . "\n\t\t\t\t" . 'if (soap_in_xsd__klrpt_output(soap, "output", &a->output, "xsd:unsignedByte"))',
	'// by updategsoapsrc.pl soap_out_xsd__klrpt_output(soap, "output", -1, &a->output, "");' . "\n\t" . 'soap_out_xsd__klrpt_output(soap, "output", -1, &a->output, "xsd:unsignedByte");'
	);

foreach(@str)
{
	my $outStr = $_;
	for( my $i = 0; $i <= $#oldInputFileStrings; $i++ )
	{
		my $instr = $oldInputFileStrings[$i];
 		my $outstr = $newOutputFileStrings[$i];	
		$outStr =~ s/$instr/$outstr/;	
	}
	print outFile $outStr;
}

close outFile;
close inputFile;

move( $outSrc, $inputSrc ) or die "Error move output file: $!";




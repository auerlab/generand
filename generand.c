/***************************************************************************
 *  Description:
 *      Generate random genomic files.  This eliminates the need to create
 *      and keep FASTA/FASTQ/SAM/VCF test files for basic code testing.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-11  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int     gen_vcf(int argc, char *argv[]);
void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    if ( strcmp(argv[1], "vcf") == 0 )
	return gen_vcf(argc, argv);
    else
    {
	fprintf(stderr, "%s: %s: Not yet implemented.\n", argv[0], argv[1]);
	return EX_UNAVAILABLE;
    }
}


/***************************************************************************
 *  Description:
 *      Generate a random VCF stream
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-11  Jason Bacon Begin
 ***************************************************************************/

int     gen_vcf(int argc, char *argv[])

{
    char    *vcf_version = "4.3",
	    *end,
	    *id = ".",
	    *bases = "ACGT",
	    *filter = ".",
	    *info = ".",
	    *format = "GT:AD:DP",
	    *hets[] = { "0|1", "1|0" },
	    *genotype;
    unsigned int    chromosomes = 22,
		    chr,
		    ref,
		    alt,
		    samples = 5,
		    c,
		    qual,
		    ref_ad,
		    alt_ad,
		    dp;
    unsigned long   calls_per_chromosome,
		    call,
		    pos,
		    max_call_separation = 1000;

    if ( argc != 4 )
	usage(argv);
    
    chromosomes = strtoul(argv[1], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    calls_per_chromosome = strtoul(argv[2], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    printf("##fileformat=VCFv%s\n", vcf_version);
    printf("#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT");
    for (c = 0; c < samples; ++c)
	printf("\tsample%u", c+1);
    putchar('\n');
    srandom(time(NULL));
    for (chr = 1; chr <= chromosomes; ++chr)
    {
	for (call = 0, pos = 0; call < calls_per_chromosome; ++call)
	{
	    pos += random() % max_call_separation;
	    ref = bases[random() % 4];
	    do
		alt = bases[random() % 4];
	    while ( alt == ref );
	    qual = random() % 60;
	    printf("%u\t%lu\t%s\t%c\t%c\t%u\t%s\t%s\t%s",
		    chr, pos, id, ref, alt, qual, filter, info, format);
	    for (c = 0; c < samples; ++c)
	    {
		genotype = hets[random()%2];
		ref_ad = random() % 30;
		alt_ad = random() % 30;
		dp = ref_ad + alt_ad;
		printf("\t%s:%u,%u:%u", genotype, ref_ad, alt_ad, dp);
	    }
	    putchar('\n');
	}
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s fasta|fastq reads\n", argv[0]);
    fprintf(stderr, "       %s sam|vcf chromosomes lines_per_chromosome\n",
	    argv[0]);
    exit(EX_USAGE);
}

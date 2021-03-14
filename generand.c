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

#define RNAME_MAX   1024

typedef enum { FASTA, FASTQ } fast_t;

int     gen_vcf(int argc, char *argv[]);
int     gen_sam(int argc, char *argv[]);
void    usage(char *argv[]);
void    gen_seq(unsigned long len);
void    gen_phred(unsigned long len);
int     gen_reads(int argc, char *argv[], fast_t file_type);

int     main(int argc,char *argv[])

{
    if ( argc < 3 )
	usage(argv);

    if ( strcmp(argv[1], "vcf") == 0 )
	return gen_vcf(argc, argv);
    else if ( strcmp(argv[1], "sam") == 0 )
	return gen_sam(argc, argv);
    else if ( strcmp(argv[1], "fasta") == 0 )
	return gen_reads(argc, argv, FASTA);
    else if ( strcmp(argv[1], "fastq") == 0 )
	return gen_reads(argc, argv, FASTQ);
    else
    {
	fprintf(stderr, "%s: %s: Unsupported format.\n", argv[0], argv[1]);
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
		    samples,
		    c,
		    qual,
		    ref_ad,
		    alt_ad,
		    dp;
    unsigned long   calls_per_chromosome,
		    call,
		    pos,
		    max_call_separation = 1000;

    if ( argc != 5 )
	usage(argv);
    
    chromosomes = strtoul(argv[2], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    calls_per_chromosome = strtoul(argv[3], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    samples = strtoul(argv[4], &end, 10);
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
	    printf("chr%u\t%lu\t%s\t%c\t%c\t%u\t%s\t%s\t%s",
		    chr, pos, id, ref, alt, qual, filter, info, format);
	    for (c = 0; c < samples; ++c)
	    {
		genotype = hets[random()%2];
		ref_ad = random() % 30;
		alt_ad = random() % (ref_ad + 1);
		dp = ref_ad + alt_ad;
		printf("\t%s:%u,%u:%u", genotype, ref_ad, alt_ad, dp);
	    }
	    putchar('\n');
	}
    }
    return EX_OK;
}


/***************************************************************************
 *  Description:
 *      Generate a random SAM file
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-12  Jason Bacon Begin
 ***************************************************************************/

int     gen_sam(int argc, char *argv[])

{
    char    rname[RNAME_MAX + 1],   // Generated from chromosome loop var
	    *cigar = "*",
	    *rnext = "*",
	    *sam_version = "1.6",
	    *end;
    unsigned int    flag = 0x10,    // All properly aligned for now
		    mapq,           // Randomized
		    pnext = 0,      // Unavailable for now
		    tlen = 0,       // Undefined for now
		    chromosomes,
		    chr;
    unsigned long   pos,
		    seq,
		    alignments_per_chromosome,
		    alignment,
		    max_alignment_separation = 1000,
		    seq_len;        // Required command line arg
    
    if ( argc != 5 )
	usage(argv);
    
    chromosomes = strtoul(argv[2], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    alignments_per_chromosome = strtoul(argv[3], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    seq_len = strtoul(argv[4], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    printf("@HD VN:%s SO:coordinate\n", sam_version);
    srandom(time(NULL));
    for (chr = 1, seq = 1; chr <= chromosomes; ++chr)
    {
	for (alignment = 0, pos = 0; alignment < alignments_per_chromosome; ++alignment)
	{
	    snprintf(rname, RNAME_MAX, "chr%u", chr);
	    pos += random() % max_alignment_separation;
	    mapq = random() % 40;
	    printf("seq%lu\t%u\t%s\t%lu\t%u\t%s\t%s\t%u\t%u\t",
		    seq++, flag, rname, pos, mapq, cigar, rnext,
		    pnext, tlen);
	    gen_seq(seq_len);
	    putchar('\t');
	    gen_phred(seq_len);
	    putchar('\n');
	}
    }
    return EX_OK;
}


/***************************************************************************
 *  Description:
 *      Generatea random nucleotide sequence of the given length
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-12  Jason Bacon Begin
 ***************************************************************************/

void    gen_seq(unsigned long len)

{
    static char *nucleotides = "ACGT";
    unsigned long   c;
    
    for (c = 0; c < len; ++c)
	putchar(nucleotides[random()%4]);
}


/***************************************************************************
 *  Description:
 *      Generatea random phred sequence of the given length
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-12  Jason Bacon Begin
 ***************************************************************************/

void    gen_phred(unsigned long len)

{
    unsigned long   c,
		    reps;
    int             ch;
    
    /*
     *  FIXME: Generate a more realistic distribution than uniform
     *  For now, generate mostly good data, PHRED 25+, in blocks with
     *  the same score
     */
    for (c = 0; c < len - 4;)
    {
	reps = random() % (len / 2);
	ch = 33 + 25 + random() % 15;
	while ( (reps > 0) && (c < len - 4) )
	{
	    putchar(ch);
	    --reps;
	    ++c;
	}
    }
    
    /*
     *  Let quality dip and vary for the last few bases like typical
     *  Illumina data
     */
    while ( c++ < len )
    {
	ch = 33 + 15 + random() % 25;
	putchar(ch);
    }
}


/***************************************************************************
 *  Description:
 *      Generate a random FASTA stream
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-03-12  Jason Bacon Begin
 ***************************************************************************/

int     gen_reads(int argc, char *argv[], fast_t file_type)

{
    char            *end;
    unsigned long   reads,
		    len,
		    c;
    
    if ( argc != 4 )
	usage(argv);
    
    reads = strtoul(argv[2], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    len = strtoul(argv[3], &end, 10);
    if ( *end != '\0' )
	usage(argv);
    
    switch(file_type)
    {
	case    FASTA:
	    for (c = 0; c < reads; ++c)
	    {
		printf(">seq%lu\n", c);
		gen_seq(len);
		putchar('\n');
	    }
	    break;
	case    FASTQ:
	    for (c = 0; c < reads; ++c)
	    {
		printf("@seq%lu\n", c);
		gen_seq(len);
		putchar('\n');
		printf("+seq%lu\n", c);
		gen_phred(len);
		putchar('\n');
	    }
	    break;
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr,
	    "Usage: %s fasta|fastq reads length\n", argv[0]);
    fprintf(stderr,
	    "       %s vcf chromosomes lines_per_chromosome samples\n",
	    argv[0]);
    fprintf(stderr,
	    "       %s sam chromosomes lines_per_chromosome length\n",
	    argv[0]);
    exit(EX_USAGE);
}

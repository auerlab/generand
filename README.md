# generand

Generand is a simple and fast tool to generate random genomic data in
FASTA/FASTQ, SAM, or VCF format, output to the standard output.

Output data is completely random and suitable for basic testing and
benchmarking of new programs, or for quickly generating small samples for
academic purposes.  For FASTA/FASTQ, the data generated are "ideal", in
that each base content is close to 25%, quality stores are good, etc.
Try running fastqc on generand fastq output to see for yourself.

Generand is especially useful for generating large test inputs on-the-fly
so that they need not be manually downloaded, generated and/or stored. Hence,
it offers extreme convenience if you don't care much about the content of
your files.

Currently the randomization is simplistic, but we plan to add paramaters
in the future to give the user some control over data properties such as GC
content, quality distribution, etc.

# Usage

generand fasta sequences sequence-length

generand fastq sequences sequence-length

generand sam chromosomes alignments-per-chromosome sequence-length

generand vcf chromosomes calls-per-chromosome samples

# Description

generand fast[aq] sequences sequence-length

generates a FASTA or FASTQ stream of 
"sequences" sequences, each of length "sequence-length".  The sequence
content is random with a uniform distribution of bases, so that GC content
should be very close to 50%.

PHRED scores in FASTQ streams are generated in blocks of equal scores and
are mostly high-quality.  The last few scores are lower quality and
independent to simulate Illumina sequencing, where quality tends to drop
near the end of each read.

generand sam chromosomes alignments-per-chromosome sequence-length

generates a SAM stream with chromosomes * alignments-per-chromosome total
alignments.  It outputs increasing indexes for QNAME and CHROM, randomly
increasing POS, random QUAL scores, and random sequences and PHRED scores
as stated for FASTQ above.

generand vcf chromosomes calls-per-chromosome samples

generates a VCF stream with chromosomes * calls-per-chromosome calls.
It outputs chromosomes with increasing indexes, randomly increasing POS,
uniformly random REF and ALT, uniformly random QUAL scores, and random
sample columns including GT (genotype), AD (allelic depth) and DP (depth).
REF counts are always >= ALT counts in the AD data and DP = REF count + ALT
count.

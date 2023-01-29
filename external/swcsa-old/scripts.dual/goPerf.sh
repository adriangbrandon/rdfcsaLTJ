perf record -g ../benchmarkwcsa_PSI_HUFFMANRLE indexes/dbpedia32 dbpedia.queries/spo.txt 12  100000
perf report --sort cpu

#!/bin/bash

rm ???.dat

##Usage: ../benchmark <graphIdx> <queryFile> <tipoQuery> <iters> [<numQueries> [<numPred> <numSO> ]]

#PSI="16 32 64 256 256b 256c 256d 256e"
PSI="16 32 64 256 256b "
for i in $PSI;
do
	echo "--- G.CSA psi= $i"
	echo '----- SPO ------------------------------' 
	../benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 1 1000 #500 
															 
done





#echo '----- 71'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/VVo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 71
#echo '----- 72'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/VVo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 72
#echo '----- 6'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/VpV.txt 496  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 6
#echo '-------- 51'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/sVV.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 51
#echo '-------- 52'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/sVV.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 52
#echo '--------- 41'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/sVo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 41
#echo '--------- 42'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/sVo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 42
#echo '--------- 43'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/sVo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 43
#echo '---------- 3'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/Vpo.txt 496  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 3
#echo '--------- 2'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/spV.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 2
#echo '--------- 1'
#bin/use_tree ../data/dbpedia/k2triples/prueba 39672 ../data/dbpedia/queries/simples/spo.txt 500  ../data/dbpedia/dacS/spDAC ../data/dbpedia/dacO/spDAC 1 1

mv spo.dat  dbpedia.spo.dat
mv spV.dat  dbpedia.spV.dat
mv sVo.dat  dbpedia.sVo.dat
mv sVV.dat  dbpedia.sVV.dat
mv Vpo.dat  dbpedia.Vpo.dat
mv VpV.dat  dbpedia.VpV.dat
mv VVo.dat  dbpedia.VVo.dat

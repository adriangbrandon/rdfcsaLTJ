#!/bin/bash

rm ???.dat ????.dat

##Usage: ./benchmark <graphIdx> <queryFile> <tipoQuery> <iters> [<numQueries> [<numPred> <numSO> ]]

#PSI="4 8 32 64 512"
PSI="4 32"
for i in $PSI;
do
	echo "--- G.CSA psi= $i"
	echo '----- SPO ------------------------------' 
	./benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 1 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 10 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 11 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 12 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/spo.txt 19 1000 #500 
															 
	echo "--- G.CSA psi= $i"
	echo '----- SP* ------------------------------'          
	echo '----- SP* ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/spV.txt 2  1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/spV.txt 20 1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/spV.txt 21 1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/spV.txt 22 1000   #500  
	./benchmark indexes/dbpedia$i dbpedia.queries/spV.txt 29 1000   #500  
															 
	echo "--- G.CSA psi= $i"
	echo '----- *PO ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/Vpo.txt 3  1000 #496 
	./benchmark indexes/dbpedia$i dbpedia.queries/Vpo.txt 30 1000 #496 
	./benchmark indexes/dbpedia$i dbpedia.queries/Vpo.txt 31 1000 #496 
	./benchmark indexes/dbpedia$i dbpedia.queries/Vpo.txt 32 10 #496     ## como 100 veces más lento que opción "3"
	./benchmark indexes/dbpedia$i dbpedia.queries/Vpo.txt 39 1000 #496 
															 
	echo "--- G.CSA psi= $i"
	echo '----- S*O ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/sVo.txt 4  1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/sVo.txt 40 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/sVo.txt 41 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/sVo.txt 42 1000 #500 
	./benchmark indexes/dbpedia$i dbpedia.queries/sVo.txt 49 1000 #500 

															 
	echo "--- G.CSA psi= $i"
	echo '----- S** ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/sVV.txt 5  1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/sVV.txt 50 1000   #500  
	./benchmark indexes/dbpedia$i dbpedia.queries/sVV.txt 59 1000   #500   
															 
	echo "--- G.CSA psi= $i"
	echo '----- *P* ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/VpV.txt 6  100   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/VpV.txt 60 100   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/VpV.txt 69 100   #500   
															 
	echo "--- G.CSA psi= $i"
	echo '----- **O ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/VVo.txt 7  1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/VVo.txt 70 1000   #500   
	./benchmark indexes/dbpedia$i dbpedia.queries/VVo.txt 79 1000   #500   

	echo "--- RDFCSA psi= $i"
	echo '----- *** ------------------------------'          
	./benchmark indexes/dbpedia$i dbpedia.queries/queryVVV.txt 8  1    #1000   
		
done

# #INTERCAMBIO FEITO EN LKE. AQUI NON O IMOS FACER POR AGORA
# #OJO INTERCAMBIO Vpo  (2select +backward search en este caso) por Vpo2.
# #OJO INTERCAMBIO Vpo2 (2select +forward  check en este caso) por Vpo.
# # De este modo ***2 siempre es "backward search"   y spo,spV,***  siempre es "forward check"
# mv Vpo.dat   Vpo22.dat
# mv Vpo2.dat  Vpo.dat
# mv Vpo22.dat Vpo2.dat



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

#mv spo.dat   dbpedia.spo.dat
#mv spo0.dat  dbpedia.spo0.dat
#mv spo1.dat  dbpedia.spo1.dat
#mv spo2.dat  dbpedia.spo2.dat
#
#mv spV.dat   dbpedia.spV.dat
#mv spV0.dat  dbpedia.spV0.dat
#mv spV1.dat  dbpedia.spV1.dat
#mv spV2.dat  dbpedia.spV2.dat
#
#mv Vpo.dat   dbpedia.Vpo.dat
#mv Vpo0.dat  dbpedia.Vpo0.dat
#mv Vpo1.dat  dbpedia.Vpo1.dat
#mv Vpo2.dat  dbpedia.Vpo2.dat
#
#mv sVo.dat   dbpedia.sVo.dat
#mv sVo0.dat  dbpedia.sVo0.dat
#mv sVo1.dat  dbpedia.sVo1.dat
#mv sVo2.dat  dbpedia.sVo2.dat
#
#mv sVV.dat   dbpedia.sVV.dat
#mv sVV0.dat  dbpedia.sVV0.dat
#
#mv VpV.dat   dbpedia.VpV.dat
#mv VpV0.dat  dbpedia.VpV0.dat
#
#mv VVo.dat   dbpedia.VVo.dat
#mv VVo0.dat  dbpedia.VVo0.dat

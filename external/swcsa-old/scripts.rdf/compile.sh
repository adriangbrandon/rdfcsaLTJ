cd .. && make clean && make

echo "../benchmark indexes/dbpedia16 dbpedia.queries/spo.txt 1 1000"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 1000"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 10"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 4 100"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/sVV.txt 5 100"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/VpV.txt 6 100"
echo "../benchmark indexes/dbpedia16 dbpedia.queries/VVo.txt 7 100"

echo "../benchmarkTest indexes/dbpedia16 4"

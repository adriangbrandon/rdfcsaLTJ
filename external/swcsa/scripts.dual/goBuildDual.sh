
##BUILD rdfcsa-dual indexes

 ../BUILDALLDUALwcsa ./texts/jamendo.hdt                              ./indexes/jamendo    "sPsi=16; nsHuff=16;psiSF=4"
 ../BUILDALLDUALwcsa ./texts/dbpedia.hdt                              ./indexes/dbpedia    "sPsi=16; nsHuff=16;psiSF=4"
#../BUILDALLDUALwcsa ./texts/wiki/wikidata-filtered-enumerated.dat.new ./indexes/wiki1.64   "sourceFormat=ADRIAN;sPsi=64; nsHuff=16;psiSF=4"
 ../BUILDALLDUALwcsa ./texts/wiki/wikidata-filtered-enumerated.dat.new ./indexes/wiki1      "sourceFormat=ADRIAN;sPsi=16; nsHuff=16;psiSF=4"
 
#../BUILDALLDUALwcsa ./texts/wiki/wikidata-enumerated.dat.new         ./indexes/wiki2.64   "sourceFormat=ADRIAN;sPsi=64; nsHuff=16;psiSF=4"
 ../BUILDALLDUALwcsa ./texts/wiki/wikidata-enumerated.dat.new         ./indexes/wiki2      "sourceFormat=ADRIAN;sPsi=16; nsHuff=16;psiSF=4"
 
 
 ##STATS dual
 ./STATSDUALwcsa scripts.dual/indexes/wiki1
 ./STATSDUALwcsa scripts.dual/indexes/wiki2
 
 
 
 ## BUILD-UNCOMPRESS-INT-INDEX-wcsa
 ../BUILDUNCOMPRESSINTINDEXwcsa indexes/wiki2 wiki2.se
 cmp wiki2.se.source indexes/wiki2.se
 
 ../BUILDUNCOMPRESSINTINDEXwcsa indexes/wiki2-dualOPS wiki2-dual.se
 cmp wiki2-dual.se.source indexes/wiki2-dualOPS.se
#cmp wiki2.64-dual.se.source indexes/wiki2.64-dualOPS.se

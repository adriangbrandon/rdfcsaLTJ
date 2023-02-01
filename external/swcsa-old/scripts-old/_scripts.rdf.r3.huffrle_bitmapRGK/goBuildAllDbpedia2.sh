ln -s ../../sandra texts

#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia2   "sPsi=2;  nsHuff=16; psiSF=1"
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia4   "sPsi=4;  nsHuff=16; psiSF=1"
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia8   "sPsi=8;  nsHuff=16; psiSF=1"
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia16  "sPsi=16;  nsHuff=16; psiSF=1"
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia32  "sPsi=32;  nsHuff=16; psiSF=1"
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia64  "sPsi=64;  nsHuff=16; psiSF=1" 
./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia512  "sPsi=512;  nsHuff=16; psiSF=1" 


#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia8   "sPsi=8;  nsHuff=16; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia16  "sPsi=16;  nsHuff=16; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia32  "sPsi=32;  nsHuff=16; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia64  "sPsi=64;  nsHuff=16; psiSF=4" 

#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia256 "sPsi=256; nsHuff=16; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia256b "sPsi=256; nsHuff=8 ; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia256c "sPsi=256; nsHuff=4 ; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia256d "sPsi=256; nsHuff=2 ; psiSF=4"
#./BUILDALLwcsa ./texts/dbpedia.hdt indexes/dbpedia256e "sPsi=256; nsHuff=1 ; psiSF=4"


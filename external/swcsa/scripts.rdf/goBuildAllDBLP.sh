mkdir indexes
#zcat ./texts/cnlong.txt.gz | ../BUILDALLwcsa stdin ./indexes/cnlong "sPsi=16; nsHuff=16;psiSF=4"
#zcat ./texts/cnlong.txt.gz | ../BUILDALLwcsa stdin ./indexes/cnlong "sPsi=64; nsHuff=16;psiSF=4"

ln -s ../../sandra texts
../BUILDALLwcsa ./texts/dblp.hdt ./indexes/dblp "sPsi=16; nsHuff=16;psiSF=4"

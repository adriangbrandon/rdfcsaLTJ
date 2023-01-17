mkdir -p indexes
#zcat ./texts/cnlong.txt.gz | ../BUILDALLwcsa stdin ./indexes/cnlong "sPsi=16; nsHuff=16;psiSF=4"
#zcat ./texts/cnlong.txt.gz | ../BUILDALLwcsa stdin ./indexes/cnlong "sPsi=64; nsHuff=16;psiSF=4"

ln -s ../BUILDALLwcsa_PSI_R3H_GAPS   BUILDALLwcsa
ln -s ../BUILDALLwcsa_PSI_R3HYBRID_GAPS


ln -s ../../sandra texts
./BUILDALLwcsa                   ./texts/jamendo.hdt ./indexes/jamendo              "sPsi=16; nsHuff=16;psiSF=4"
./BUILDALLwcsa_PSI_R3HYBRID_GAPS ./texts/jamendo.hdt ./indexes/jamendoR3HYBRID_GAPS "sPsi=16; nsHuff=16;psiSF=4"

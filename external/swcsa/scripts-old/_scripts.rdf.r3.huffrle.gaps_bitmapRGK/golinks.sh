rm -f benchmark BUILDALLwcsa

ln -s ../benchmarkwcsa_PSI_R3H_GAPS_RGK benchmark
ln -s ../BUILDALLwcsa_PSI_R3H_GAPS_RGK  BUILDALLwcsa

rm -f indexes
ln -s ../indexes/r3/gaps indexes

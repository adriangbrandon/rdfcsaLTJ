rm -f benchmark BUILDALLwcsa

ln -s ../benchmarkwcsa_PSI_HUFFMANRLE benchmark
ln -s ../BUILDALLwcsa_PSI_HUFFMANRLE  BUILDALLwcsa

rm -f indexes
ln -s ../indexes/huffrle indexes

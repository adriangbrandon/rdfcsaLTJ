rm -f benchmark BUILDALLwcsa


ln -s ../BUILDALLwcsa_PSI_PLAIN  BUILDALLwcsa
ln -s ../benchmarkwcsa_PSI_PLAIN benchmark

rm -f indexes  
ln -s ../indexes/plain/ indexes

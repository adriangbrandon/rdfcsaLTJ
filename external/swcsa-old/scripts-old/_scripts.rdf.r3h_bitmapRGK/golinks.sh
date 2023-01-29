rm -f benchmark BUILDALLwcsa


ln -s ../benchmarkwcsa_PSI_R3HYBRID_RGK benchmark
ln -s ../BUILDALLwcsa_PSI_R3HYBRID_RGK BUILDALLwcsa

rm -f indexes  
ln -s ../indexes/r3-hybrid/ indexes

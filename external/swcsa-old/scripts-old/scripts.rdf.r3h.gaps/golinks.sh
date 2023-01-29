rm -f benchmark BUILDALLwcsa


ln -s ../benchmarkwcsa_PSI_R3HYBRID_GAPS benchmark
ln -s ../BUILDALLwcsa_PSI_R3HYBRID_GAPS BUILDALLwcsa

rm -f indexes
ln -s ../indexes/r3-hybrid/gaps indexes

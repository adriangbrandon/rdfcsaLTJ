rm -f benchmark BUILDALLwcsa


ln -s ../benchmarkwcsa_PSI_R3HYBRID_UINT32 benchmark
ln -s ../BUILDALLwcsa_PSI_R3HYBRID_UINT32  BUILDALLwcsa


rm -f indexes
ln -s ../indexes/r3-hybrid/uint32 indexes

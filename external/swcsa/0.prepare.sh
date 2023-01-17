#Create destination directory for the indexes created at building time.
#or create a symbolic link to the TARGET_DIR
mkdir -p indexes
#ln -s TARGET_DIR indexes 
#ln -s /media/SEAGATE.2.EXT4/ESPACIO/RDFIDXs indexes

mkdir -p indexes/huffrle
mkdir -p indexes/plain


mkdir -p indexes/r3         ##both for b375 and rrr  (as the builder creates both bitmaps)
mkdir -p indexes/r3/gaps    ##both for b375 and rrr  (as the builder creates both bitmaps)

mkdir -p indexes/r3-hybrid              
mkdir -p indexes/r3-hybrid/gaps
mkdir -p indexes/r3-hybrid/uint32    



  #cd scripts.rdf.huffrle                    ; sh golinks.sh ; cd ..
  #cd scripts.plain                          ; sh golinks.sh ; cd ..

  #cd scripts.rdf.r3.huffrle                 ; sh golinks.sh ; cd ..
  #cd scripts.rdf.r3.huffrle_bitmapRGK       ; sh golinks.sh ; cd ..
cd scripts.rdf.r3.huffrle.gaps            ; sh golinks.sh ; cd ..
#cd scripts.rdf.r3.huffrle.gaps_bitmapRGK  ; sh golinks.sh ; cd ..

  #cd scripts.rdf.r3h                        ; sh golinks.sh ; cd ..
  #cd scripts.rdf.r3h_bitmapRGK              ; sh golinks.sh ; cd ..
cd scripts.rdf.r3h.gaps                   ; sh golinks.sh ; cd ..
#cd scripts.rdf.r3h.gaps_bitmapRGK                   ; sh golinks.sh ; cd ..
  #cd scripts.rdf.r3h.uint32                 ; sh golinks.sh ; cd ..



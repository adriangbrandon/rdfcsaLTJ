#				#######################################################################
#				#export PSITYPE=PSI_HUFFMANRLE
#				#export BITMAPTYPE=BITMAP_375
#				#make clean;
#				#make 
#
#				#export PSITYPE=PSI_HUFFMANRLE
#				#export BITMAPTYPE=BITMAP_RGK
#				#make clean;
#				#make 
#				#######################################################################
#
#
#				#######################################################################
#				#export PSITYPE=PSI_PLAIN
#				#export BITMAPTYPE=BITMAP_375
#				#make clean;
#				#make 
#
#
#				#######################################################################
#
#
#				#v2 [compresion de Psi como 3 rangos independientes, con PsiHuffmanRle3R]
#				#	#######################################################################
#				#	#PSI considerando 3 rangos independientes S,P,O que se codifican con HuffRlE3R
#				#	export PSITYPE=PSI_R3H
#				#	export BITMAPTYPE=BITMAP_375
#				#	make clean;
#				#	make
#				#
#				#	export PSITYPE=PSI_R3H
#				#	export BITMAPTYPE=BITMAP_RGK
#				#	make clean;
#				#	make
#				#	#######################################################################
#
#					#######################################################################
#					#PSI considerando 3 rangos independientes S,P,O que se codifican con HuffRlE3R
#					# pero en cada rango se representan los valores con respecto al mínimo de ese rango.
#	export PSITYPE=PSI_R3H_GAPS
#	export BITMAPTYPE=BITMAP_375
#	make clean;
#	make
#	
#				#	export PSITYPE=PSI_R3H_GAPS
#				#	export BITMAPTYPE=BITMAP_RGK
#				#	make clean;
#				#	make
#					#######################################################################
#
#
#				#v2-->v3 [buscar velocidad: S y O en plano (uin32 o log2bits), P con PsiHuffmanRle3R]
#					#######################################################################
#					#  UINTS EN S, y O (codificados con log_k bits), HuffmanRlE3R en P.
#				#	export PSITYPE=PSI_R3HYBRID
#				#	export BITMAPTYPE=BITMAP_375
#				#	make clean;
#				#	make
#					
#	export PSITYPE=PSI_R3HYBRID_GAPS
#	export BITMAPTYPE=BITMAP_375
#	make clean;
#	make
#
#				#	#  UINTS EN S y O (como array de uin32), HuffmanRlE3R en P.
#				#	export PSITYPE=PSI_R3HYBRID_UINT32
#				#	export BITMAPTYPE=BITMAP_375
#				#	make clean;
#				#	make
#				#	########################################################################
#
#
#
#				#**NOTA COMPILACIÓN/EJECUCIÓN
#				#Para todos os índices créanse sempre o bitmap 375 E RGK (rrr-20 por defecto). 
#				#Isto permite que SÓ HAXA QUE LANZAR  o proceso de construcción 1 vez.
#				#  - Despois, en función do benchmark ou benchmark_bitmapRGK que usemos, o buscador usará 
#				#     un ou outro índice, e reportará o espacio usado correctamente.
#				#
#
#


##hybrid option	(predicates with huffmanRLE, Subjects and Objects keep explicit values encoded with log2_k bits.
#	export PSITYPE=PSI_R3HYBRID_GAPS
#	export BITMAPTYPE=BITMAP_375
#	make clean;
#	make


##default option  (3 psi ranges encoded with huffmanRLE).
	export PSITYPE=PSI_R3H_GAPS
	export BITMAPTYPE=BITMAP_375
	make clean;
	make


#default: hybrid option	(subjects, predicates, and objects with huffmanRLE, Plain log2_k bits, or PlainRLE depending on build options "psitypespo=213, psitypeops=312")
	#1=HuffmanRLE-gaps, #2=Plain-gaps #3=plainRLE-gaps
	export PSITYPE=PSI_R3CLS_GAPS
	export BITMAPTYPE=BITMAP_375
	make clean;
	make



unset PSITYPE

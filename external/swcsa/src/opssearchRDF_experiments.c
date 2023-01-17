
#define USE_SELECT   //initial binary search on A rather than selects over D bitmap.






/****************************************************************************/
/** functions used for testing **********************************************/
/****************************************************************************/

double getTime2 (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec +
		(double) usage.ru_utime.tv_usec / 1000000.0;
	systime = (double) usage.ru_stime.tv_sec +
		(double) usage.ru_stime.tv_usec / 1000000.0;

	return (usertime + systime);
}



/****************************/
int rand01() {
	double inicio = 0;
	double end = 2.0;
	return inicio+(int) (end *rand()/(RAND_MAX+1.0));
}
/****************************************************************************/


#include "ops_spo.cpp"
#include "ops_sp.cpp"
#include "ops_po.cpp"
#include "ops_so.cpp"
#include "ops_s.cpp"
#include "ops_p.cpp"
#include "ops_o.cpp"
#include "ops_vvv.cpp"



#include "blu3.h" 



/* [signal  quit] */  
void sod_signal( void * data, int signal_num) {
 
	struct  pw_main_loop *main_loop = data;
	
	pw_main_loop_quit(main_loop);

	SOD_PUT; 

}


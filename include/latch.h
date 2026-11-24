
#ifndef LATCH_H 
 

#include <string.h> 

#define LATCH_H 



typedef enum { 
         
	SINK_SPEAKER =1 , 

	SINK_HEADPHONES , 

	SINK_HDMI, 

	SINK_BLUETOOTH, 

	SINK_DISPLAYPORT,

	SINK_HEADSET, 

        SINK_OTHER_PORT
  



}sod_sink_latch; 




sod_sink_latch  sod_latch_c(const char *name_); 

#endif 

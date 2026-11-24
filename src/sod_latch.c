#include "latch.h" 







sod_sink_latch  sod_latch_c(const char *node_name_) {


   if ( strstr (node_name_, "__HDMI") )
   { 
		   return SINK_HDMI;
   } 
	   
	   if  ( strstr (node_name_, "__Speaker__")) 
	   { 
		  
		   return SINK_SPEAKER;
	   } 
       
       if  ( strstr (node_name_, "__Headphones") )
       { 
	       
	       return SINK_HEADPHONES;
       } 
	       
	       if (strstr (node_name_, "bluez_output"))
	       { 
		       
		       return SINK_BLUETOOTH; 

	       }

	       if ( strstr (node_name_, "Headset")) 
	       { 
		       return SINK_HEADSET; 
	       } 

	       return SINK_OTHER_PORT;
 



        }  

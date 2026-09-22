
#ifndef BLU3_SKY

#define BLU3_SKY



#include <stdio.h>
#include <stdlib.h>
#include <spa/debug/format.h>
#include <spa/param/props-types.h>
#include <spa/pod/iter.h>
#include <time.h>
//#include <conf.h> 
#include "profile.h" 




/* [EACH APP PROXY HANDLER] */

typedef struct {

	char Node_name[512];
	char Media_class[256]; 
	int32_t n_input_ports;
	int32_t n_output_ports;
        uint32_t  id;
        struct pw_proxy *sod_node;      /* object */
        struct spa_hook sod_node_hook; /* subscriber aka event handler */
        uint32_t Version;               /* node version */ 
	enum pw_node_state last_state;
        enum pw_node_state pending_state;
        struct spa_source *db_timer;   /* timer for each node */ 
	struct pw_loop  *sod_update_loop; 


}sod_app_handler;


#ifdef __COLOR__

#define RED "\033[1;31m" 
#define RESET "\033[0m"
#define GREEN "\033[1;32m" 
#define CYAN  "\033[1;36"

#endif 


typedef struct{ 

  struct pw_main_loop * sod_loop;         /* event loop */  
  struct pw_context   * sod_context;        /* sod obj */ 
  struct pw_core    * sod_core;               /* proxy  */ 
  struct pw_registry * sod_registry; 
  struct spa_hook     sod_registry_listener;  /* subscriber track for registry */
  sod_app_handler sod_node_tracker[TRACK_APP_SIZE]; /* array of struct Sod_app_handler */ 
  size_t n_counter; 
  struct pw_loop *sod_cached_loop;


} sod_info; 


 /* debounce timer */ 

struct timespec debounce_time = { 
	.tv_sec=0,        /* sec */ 
	.tv_nsec=300 * 1000 * 1000,      /* nano sec */  
}; 













#endif         /* BLU3_SKY */ 


#ifndef BLU3_SKY__

#define BLU3_SKY__ 


#include <spa/utils/hook.h>
#include <stdio.h>
#include <stdlib.h>
#include <spa/debug/format.h>
#include <spa/param/props-types.h>
#include <spa/pod/iter.h>
#include <time.h>
#include "conf.h" 
#include "profile.h" 
#include <spa/pod/iter.h>
#include <pipewire/pipewire.h> 
#include <spa/param/audio/format-utils.h>

struct sod_info_s;
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
	enum pw_node_state last_state;  /* get the last state after debounce */ 
        enum pw_node_state pending_state;  /* pending state when it flooding */ 
        struct spa_source *db_timer;   /* timer for each node */ 
	struct pw_loop  *sod_update_loop;
        struct sod_info_s   *parent;	
        bool pending_param;
        bool sod_auto_mt;               /* store what sod muted */ 	

}sod_app_handler;



typedef struct sod_info_s{ 

  struct pw_main_loop * sod_loop;         /* event loop */  
  struct pw_context   * sod_context;        /* sod obj */ 
  struct pw_core    * sod_core;               /* proxy  */ 
  struct pw_registry * sod_registry; 
  struct spa_hook     sod_registry_listener;  /* subscriber track for registry */
  sod_app_handler sod_node_tracker[TRACK_APP_SIZE]; /* array of struct Sod_app_handler */ 
  size_t n_counter; 
  struct pw_loop *sod_cached_loop;             /* cached loop for db timer */ 
  bool	 T_mic_on; 
  float   T_volume; 


} sod_info; 


#define  sod_print(...)  fprintf(stdout,__VA_ARGS__) 
 /* debounce timer */ 

extern struct timespec debounce_time;  



void  sod_apply_profile(const struct sod_profile *p , sod_info * data);


void sod_set_volume(sod_app_handler *sod_data, sod_info *info );

void sod_mute_node(sod_app_handler *slot_mt, bool muted  );



#endif         /* BLU3_SKY__ */ 

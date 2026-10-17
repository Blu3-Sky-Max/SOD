
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
#include "capture.h"
#include <signal.h>
#include <pthread.h>


struct sink;  
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

	struct pw_loop  *sod_update_loop;      /* loop for cached loop for db_timer */ 

        struct sod_info_s   *parent;	

        bool pending_param;

        bool sod_auto_mt;               /* store what sod muted */ 

        char media_name[1024];           /* add media name to know name given by node_name */ 

       _Bool     sod_capture_auto_mt;            /* store what mic muted */ 
	

}sod_app_handler;


#define MIC_MAX_SIZE 12


/* [sod_mic] */  

typedef struct {
        char mic_name[256];                  /* node name of the mic */ 

        uint32_t mic_id;                    /* node id of the mic */ 

	char mic_description[1024];      /* mic description Ex: "Ryzen HD Audio Controller Headset Mono Microphone" */ 
	
	struct pw_node *mic_node;          /* mic binding node */ 
	
	struct spa_hook mic_node_listener;    /* mic event tracker */ 

	enum pw_node_state mic_last_state;          /* mic last state */ 

	enum pw_node_state mic_pending_state;       /* mic pending state */ 

        struct spa_source *mic_db_timer;           /* mic db_timer source */ 

       struct pw_loop   *sod_mic_loop; 	          /* timer loop */ 

       struct sod_info_s   *mic_parent;          





}sod_mic; 

typedef struct sod_info_s{ 

  struct pw_main_loop * sod_loop;         /* event loop */

  struct pw_context   * sod_context;        /* sod obj */

  struct pw_core    * sod_core;               /* proxy  */ 

  struct pw_registry * sod_registry;

  struct spa_hook     sod_registry_listener;  /* subscriber track for registry */

  sod_app_handler  **sod_node_tracker; /* array of struct Sod_app_handler */

  size_t n_counter;                       /* number of count for node  */  

  size_t Track_Capacity;                     /* track size of sod_node_tracker */  

  struct pw_loop *sod_cached_loop;             /* cached loop for db timer */

  bool	 T_mic_on; 

  float   T_volume; 

  sod_mic *mics[MIC_MAX_SIZE]; 

  uint32_t mic_count;

  sod_capture *captures[CAP_MAX_SIZE]; 
  
   uint32_t  n_capture;  
  
   uint8_t    capture_running;

   struct sink *sod_sink; 

} sod_info; 



#define SOD_PUT puts("")

#define sod_error(fmt, ...)   fprintf(stderr,RED fmt RESET, ##__VA_ARGS__)

#define  sod_print(...)  fprintf(stdout,__VA_ARGS__) 

/* debounce timer */ 

extern struct timespec debounce_time;  


void  sod_apply_profile(const struct sod_profile *p , sod_info * data);


void sod_set_volume(sod_app_handler *sod_data, sod_info *info );

void sod_mute_node(sod_app_handler *slot_mt, bool muted  );

void sod_capture_w(sod_info * , sod_app_handler * );

void sod_signal( void * data, int signal_num); 


/* [EXIT CODE] */ 
typedef enum  {

	INC_ARG=5,       /* incomplete argc */ 

       	BAD_OPTION,     /* bad option */ 

	ALLOC_FAIL,      /* allocation fail */  

	UNKOWN_PROFILE    /* unkown profile */ 


}sod_exit_status;	



#endif         /* BLU3_SKY__ */ 

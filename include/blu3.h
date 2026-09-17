




#include <conf.h> 
#ifndef BLU3_SKY

#define BLU3_SKY




/* [EACH APP PROXY HANDLER] */

typedef struct {

        uint32_t  id;
        struct pw_proxy *sod_node;      /* object */
        struct spa_hook sod_node_hook; /* subscriber aka event handler */
        const char *name;
        uint32_t Version;


}sod_app_handler;






typedef struct{ 

  struct pw_main_loop * sod_loop;         /* event loop */  
  struct pw_context   * sod_context;        /* sod obj */ 
  struct pw_core    * sod_core;               /* proxy  */ 
  struct pw_registry * sod_registry; 
  struct spa_hook     sod_registry_listener;  /* subscriber track for registry */
  sod_app_handler sod_node_tracker[TRACK_APP_SIZE]; /* array of struct Sod_app_handler */ 
  size_t n_counter; 



} sod_info; 








/*  [Operating modes] */ 

#ifndef  _OPERATING_MODES 

#define _OPERATING_MODES

#define Library_Vol  80 
#define Focus_Vol    45 
#define Office_Vol   30
#define Night_Vol    10
#define Meeting_Vol  60 
#define Gaming_Vol   90



#endif /* OPERATING_MODES */ 


#include <stdio.h> 
#include <stdlib.h> 
#include <spa/debug/format.h> 
#include <spa/param/props-types.h> 
#include <spa/pod/iter.h> 










#endif         /* BLU3_SKY */ 

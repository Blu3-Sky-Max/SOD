
#ifndef BLU3_SKY

#define BLU3_SKY

typedef struct{ 

  struct pw_main_loop * sod_loop;         /* event loop */  
  struct pw_context   * sod_context;        /* sod obj */ 
  struct pw_core    * sod_core;               /* proxy  */ 
  struct pw_registry * sod_registry; 
  struct spa_hook     sod_registry_listener;  /* subscriber track for registry */
  struct pw_node    * sod_node;               /* object */ 
  struct spa_hook   sod_node_hook;    






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




#ifndef PROFILE_H__ 

#define PROFILE_H__ 

#include <getopt.h> 

#define __COLOR__
#include "color.h"

 
/*  [Operating profile] */

#ifndef  _OPERATING_PROFILE__  

#define _OPERATING_PROFILE__ 


#define _OPERATING_VOLUME       

#define Library_Vol  80 
#define Focus_Vol    45 
#define Office_Vol   60
#define Night_Vol    10
#define Meeting_Vol  60 
#define Gaming_Vol   100



struct sod_profile { 

	 const char  *P_name;     /* profile name */

	int Volume;               /* profile volume */ 

	bool mic_on;             /* profile mic setting */ 




}; 


/* [enivronment profile] */ 

static const struct sod_profile profiles[] = { \
	{ "library", Library_Vol, FALSE},    /* Library modes */ 

	{ "focus",  Focus_Vol, FALSE },     /* focus modes */ 
	{ "gaming", Gaming_Vol, TRUE	},    /* gaming modes */ 
	{ "office", Office_Vol, FALSE },   /* office modes */ 
	{ "meeting", Meeting_Vol , TRUE},  /* meeting modes */ 
	{ "night", Night_Vol, TRUE},      /* night modes */ 



	} ; 

#define N_PROFILE sizeof(profiles)/sizeof(profiles[0]) 

#endif /* _OPERATING_PROFILE__ */



/* options */ 
extern struct option sod_option[]; 

/* show_case */
extern const char  *show_case; 	 




#endif /* PROFILE_H__ */

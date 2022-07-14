/* place.x: Remote call to find city’s latitude and longitude */


const MAXNAMELEN_P = 255;    /* maximum length of state or city name */
const AIRPORTNUM_P = 5;
const NOCITY = 200;        /* value to indicate no city exists */

typedef string nametype_p <MAXNAMELEN_P>;   /* a state or city name */
typedef struct airportnode* airportlist;       /* a node in the airport listing */

struct place {
     nametype_p state;
     nametype_p city;
};

/* a node in the airport listing */
struct airportnode {
     nametype_p code;    /* 3-digit airport code */
     nametype_p airname; /* name of airport */
     nametype_p city;    /* city the airport serves */ 
     double distance;  /* distance from original location */
};


/* result of GETAIRPORTS() operation */
union getairport_res switch (int err) {
  case 0:
      airportnode list[AIRPORTNUM_P];  /* no error: return airport listing */
  default:
      void;              /* error occurred: nothing else to return */
 };


/* The place program definition */
program PLACEPROG {
     version PLACEVERS {
          getairport_res GETAIRPORTS(struct place) = 1; 
     } = 1;
} = 0x3ab29c27;

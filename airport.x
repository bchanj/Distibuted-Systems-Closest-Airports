/* airport.x: Remote call to return top N airport codes, names, city that each airport serves, and distance from this city to that airport */


const MAXNAMELEN = 255;    /* maximum length of airport or city name */
const AIRPORTNUM = 5;      /* number of nearest airports returned */

typedef string nametype <MAXNAMELEN>; /* a city and an airport name */
typedef struct airnode* airlist;             /* a link in the airport listing */

struct cord {
     double lng;
     double lat;
};

/* a node in the airport listing*/
struct airnode {
     nametype code;    /* 3-digit airport code */
     nametype airname; /* name of airport */
     nametype city;    /* city the airport serves */
     double distance;  /* distance from original location  */
};


/* result of FINDAIRPORT() operation */
union findairport_res switch (int err) {
  case 0:
      airnode list[AIRPORTNUM];  /* no error: return airport listing airnode list[AIRPORTNUM]*/
  default:
      void;           /* error occurred: nothing else to return */
 };

/* The AIRPORT program definition */
program AIRPORTPROG {
     version AIRPORTVERS {
           findairport_res FINDAIRPORT(cord) = 1;
     } = 1;
}= 0x3de92e72;

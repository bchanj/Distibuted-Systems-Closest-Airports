#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "airport.h"

#define AIRPORTNAME 1
#define AIRPORTCITY 2
#define AIRPORTCOUNTRY 3
#define AIRPORTCODE 4
#define LATITUDE 6
#define LONGITUDE 7
#define INVALID 10
#define CONSTRUCTED 123

struct airport
{
	double lat;
	double lng;
	nametype code;
	nametype airname; 
	nametype city;  
	struct airport* next;
};

typedef struct airport* airportlist;

airportlist root;
int data = 0;   // Flag to check if loaded and created

struct airport* airport_parser(char* file_name);
struct airnode* closest_five_airports(double lat, double longitude, struct airport* temp);
double deg2rad(double deg);
double rad2deg(double rad);
double distance(double lat1, double lon1, double lat2, double lon2, char unit);


findairport_res *findairport_1_svc(struct cord *c, struct svc_req *sr)
{
    if(data != CONSTRUCTED) {
        data = CONSTRUCTED;
        root = airport_parser("airports.dat");
    }

    airportlist temp = root;
    airlist array = closest_five_airports(c->lat, c->lng, temp);

    static findairport_res result;
    result.err = 0;

   for(int i = 0; i < AIRPORTNUM; i++) {
        result.findairport_res_u.list[i].airname = strdup(array[i].airname);
        result.findairport_res_u.list[i].city =  strdup(array[i].city);
        result.findairport_res_u.list[i].code =  strdup(array[i].code);
        result.findairport_res_u.list[i].distance = array[i].distance;
    } 

    return &result;
}

struct airnode* closest_five_airports(double lat, double longitude, struct airport* temp) {
	static struct airnode closestArr[5];
	for (int i = 0; i < AIRPORTNUM; i++) {
		closestArr[i].airname = "";
		closestArr[i].city = "";
		closestArr[i].code = "";
		closestArr[i].distance = -1;
	}

	while (temp != NULL) {
        double dist = distance(lat, longitude, temp->lat, temp->lng, 'M');
        for(int i = 0; i < AIRPORTNUM; i++)
        {
            if (dist < closestArr[i].distance || closestArr[i].distance == -1) {

                for(int j = AIRPORTNUM - 1; j > i; j--)
                {
                    closestArr[j] = closestArr[j-1];
                }
                closestArr[i].distance = dist;
                closestArr[i].city = temp->city;
                closestArr[i].code = temp-> code;
                closestArr[i].airname = temp->airname;
                break;
            }
        }
        temp = temp->next;
    }
    return closestArr;
}


struct airport* airport_parser(char* file_name)
{
	FILE *fp = fopen(file_name, "r");
	
	if(!fp)
	{
		printf("Cannot open file");
		return 0;
	}
	
	char buff[1024];
	int field = 0;
		
	struct airport* temp = (struct airport*)malloc(sizeof(struct airport));
	struct airport* index = temp;
	index->airname = (char*)malloc(MAXNAMELEN * sizeof(char));
	
	index->airname[0] = '\0';
	index->city = (char*)malloc(MAXNAMELEN * sizeof(char));
	index->code = (char*)malloc(MAXNAMELEN * sizeof(char));
	
	while(fgets(buff, 1024, fp))
	{
		field = 0;
		
		if(index->airname[0] != '\0')
		{
			index->next = (struct airport*)malloc(sizeof(struct airport));
			index = index-> next;
			index->airname = (char*)malloc(MAXNAMELEN * sizeof(char));
			index->city = (char*)malloc(MAXNAMELEN * sizeof(char));
        	index->code = (char*)malloc(MAXNAMELEN * sizeof(char));
			index->airname[0] = '\0';	
		}

		char *token = strtok(buff, ",");
		while(token)
		{
			if(field == AIRPORTNAME)
				strcpy(index->airname, token);
			if(field == AIRPORTCITY)
				strcpy(index->city, token);
			
			if(field == AIRPORTCOUNTRY && strcmp(token, "\"United States\"") != 0)
			{
				index->airname[0] = '\0';
				index->city[0] = '\0';
				break;
			}
			if(field == AIRPORTCODE)
				strcpy(index->code, token);
            if(strcmp(token, "\\N") == 0)
			{
					index->airname[0] = '\0';
                    index->city[0] = '\0';
                    break;
			}
			if(field == LATITUDE)
				index->lat = strtod(token, NULL);
			if(field == LONGITUDE)
			{
				index->lng = strtod(token, NULL);
				break;			
			}
			
			token = strtok(NULL, ",");
			field++;
		}
	}
	index->next = NULL;
	return temp;
}


double deg2rad(double deg)
{
    return (deg * M_PI / 180);
}

double rad2deg(double rad)
{
    return (rad * 180 / M_PI);
}

double distance(double lat1, double lon1, double lat2, double lon2, char unit)
{
    double theta, distance;

    theta = lon1 - lon2;
    distance = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    distance = acos(distance);
    distance = rad2deg(distance);
    distance = distance * 60 * 1.1515;

    switch (unit)
    {
    case 'M':
        break;
    case 'K':
        distance = distance * 1.609344;
        break;
    case 'N':
        distance = distance * 0.8684;
        break;
    }

    return (distance);
}

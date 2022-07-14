#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "place.h"
#include "airport.h"

#define STATENUM 50
#define CITY 2
#define STATE 3
#define LATITUDE 7
#define LONGITUDE 8
#define CONSTRUCTED 123

typedef struct citynode* citytree;

struct citynode {
    nametype_p cityname;
    double lat;
    double lng;
    citytree left_node;
    citytree right_node;
};

struct statenode {
    char* name;
    citytree root;
};

struct statenode statelist[STATENUM];
int data = 0;   // Flag to check if loaded and created

citytree insert(citytree c, char* name, double lt, double ln);
citytree search(struct citynode* c, char* name);
void create_BSTs();

getairport_res* getairports_1_svc(struct place *location, struct svc_req *sr)
{
    // Load data into state array with city BSTs
    if(data != CONSTRUCTED) {
        data = CONSTRUCTED;
        create_BSTs();
    }

    // Search BST and find latitude and longitude if city exists
    citytree temp = NULL;
    for(int i = 0; i < STATENUM; i++)
        if(strcasecmp(location->state, statelist[i].name) == 0)
            temp = search(statelist[i].root, location->city);

    static getairport_res error;
    if(temp == NULL) {
        error.err = 1;
        return &error;
    }

    // Call airport server
    CLIENT *handle;
    struct cord cords;
    cords.lng = temp->lng;
    cords.lat = temp->lat;

    if ((handle = clnt_create("localhost", AIRPORTPROG, AIRPORTVERS, "tcp")) == NULL) {
        clnt_pcreateerror("localhost");
        return NULL;
    }

    // Retrieve result from airport server
    findairport_res *result;
    result = findairport_1(&cords, handle);

    if(result == NULL) {
        clnt_perror(handle,"localhost");
        return NULL;
    }

    // Copy airport return value to return value for client
    static getairport_res res;
    for(int i = 0; i < AIRPORTNUM; i++) {
        res.getairport_res_u.list[i].airname = strdup(result->findairport_res_u.list[i].airname);
        res.getairport_res_u.list[i].city =  strdup(result->findairport_res_u.list[i].city);
        res.getairport_res_u.list[i].code =  strdup(result->findairport_res_u.list[i].code);
        res.getairport_res_u.list[i].distance = result->findairport_res_u.list[i].distance;
    } 
    
    xdr_free((xdrproc_t) xdr_findairport_res, (char*) result);
    clnt_destroy(handle);

    return &res;
}


void create_BSTs() 
{
    FILE *file;
    size_t length;
    ssize_t nread;
    char* line = NULL;
    char* token = NULL;

    nametype_p city;
    double lat;
    double lng;
    
    
    file = fopen("cities.dat", "r");

    for(int i = 0; i < STATENUM; i++) {
        statelist[i].name = NULL;
        statelist[i].root = NULL;
    }

    if (file == NULL)
        exit(1);

    int i = 0;
    int field;

    while((nread = getline(&line, &length, file) != -1) && (i < STATENUM)) {
        token = strtok(line, "\t");
        field = 0;

        while(token != NULL) {
            if(field == STATE) {
                if (statelist[i].name == NULL)
                    statelist[i].name = strndup(token, strlen(token));
                else if ((strcasecmp(statelist[i].name, token) != 0) && 
                        (strcasecmp(token, "District of Columbia") != 0) && 
                        (strcasecmp(token, "Marshall Islands") != 0))
                        i++; 
            }

            if(field == CITY) {
                city = token;
                city[strlen(token)] = '\0';
            }

            if(field == LATITUDE) {
                lat = atof(token);
            }
            
            if(field == LONGITUDE) {
              lng = atof(token);
              statelist[i].root = insert(statelist[i].root, city, lat, lng);
            }  

            token = strtok(NULL, "\t");
            field++;
        }
    }

    fclose(file);
}


citytree insert(citytree c, char* name, double lt, double ln) {
    if (c == NULL) {
        struct citynode* new = (struct citynode*)malloc(sizeof(struct citynode));
        new->cityname = strndup(name, strlen(name));
        new->lat = lt;
        new->lng = ln;
        new->left_node = NULL;
        new->right_node = NULL; 
        c = new; 
    }
    else if (strcasecmp(c->cityname, name) < 0) 
        c->left_node = insert(c->left_node, name, lt, ln);
    else
        c->right_node = insert(c->right_node, name, lt, ln); 
    return c;
}

citytree search(struct citynode* c, char* target) {
    if ((c == NULL) || (strcasecmp (c->cityname, target) == 0))  
        return c;
    else if (strcasecmp(c->cityname, target) > 0) 
        return search(c->right_node, target);
    else 
        return search(c->left_node, target);
}
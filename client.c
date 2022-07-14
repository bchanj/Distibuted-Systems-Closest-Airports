#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "place.h"
#define RMACHINE "localhost"

int main(int argc, char** argv) {
    CLIENT *handle;
    struct place location;

    if(argc != 3) {
        fprintf(stderr, "usage: %s state city \n", argv[0]);
        exit(1);
    }

    if ((handle = clnt_create(RMACHINE, PLACEPROG, PLACEVERS, "tcp")) == NULL) {
        clnt_pcreateerror(RMACHINE);
        exit(2);
    }

    location.city = argv[1];
    location.state = argv[2];

    getairport_res* result;
    result = getairports_1(&location,handle);

    if(result == NULL) {
        clnt_perror(handle,"localhost");
        exit(3);
    }

    if(result->err != 0) {
        printf("The city and / or state could not be located\n");
        exit(4);
    }

    printf("Nearest 5 airports \n");
    for(int i = 0; i < AIRPORTNUM_P; i++) {
        result->getairport_res_u.list[i].code++;
        result->getairport_res_u.list[i].code[strlen(result->getairport_res_u.list[i].code)-1] = '\0';
        result->getairport_res_u.list[i].airname++;
        result->getairport_res_u.list[i].airname[strlen(result->getairport_res_u.list[i].airname)-1] = '\0';
        result->getairport_res_u.list[i].city++;
        result->getairport_res_u.list[i].city[strlen(result->getairport_res_u.list[i].city)-1] = '\0';
        printf("    %s (%s) at %f miles serving %s \n", result->getairport_res_u.list[i].code, 
                                                    result->getairport_res_u.list[i].airname, 
                                                    result->getairport_res_u.list[i].distance, 
                                                    result->getairport_res_u.list[i].city);
    }

    // xdr_free((xdrproc_t) xdr_getairport_res, (char*) result);
    xdr_free((xdrproc_t) xdr_getairport_res, (char*) result->getairport_res_u.list);

    clnt_destroy(handle);
    return 0;
}
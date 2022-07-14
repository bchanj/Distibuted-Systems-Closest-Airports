all: client place airport

CFLAGS = -DDEBUG -c
RPCGEN = rpcgen
LIBS = -lm

clean:
	$(RM) *.o place_svc.c place_clnt.c place_xdr.c place.h client place 
	$(RM) airport_clnt.c airport_svc.c airport_xdr.c airport.h airport.h.gch airport

# rules for generating source code from place IDL
place_clnt.c: place.h place.x 
place_svc.c: place.h place.x 

# rules for generating source code from airport IDL
airport_clnt.c: airport.h airport.x
airport_svc.c: airport.h airport.x

# header files with generated output
place.h: place.x
	$(RPCGEN) place.x
	sed -i 's/#include <rpc\/rpc.h>/&\n#include <rpc\/pmap_clnt.h>\n#include <string.h>\n#include <unistd.h>/' place.h

airport.h: airport.x
	$(RPCGEN) airport.x
	sed -i 's/#include <rpc\/rpc.h>/&\n#include <rpc\/pmap_clnt.h>\n#include <string.h>\n#include <unistd.h>/' airport.h

# client program
client: place_clnt.o client.o place_xdr.o
	$(CC) -o $@ place_clnt.o client.o place_xdr.o
	chmod 775 client

# place (client/server) program
place: place_svc.o place.o place_xdr.o airport_clnt.o airport_xdr.o
	$(CC) -o $@ place_svc.o place.o place_xdr.o airport_clnt.o airport_xdr.o $(LIBS)
	chmod 775 place

# airport (server) program
airport: airport_svc.o airport_procs.o airport_xdr.o
	$(CC) -o $@ airport_svc.o airport_procs.o airport_xdr.o $(LIBS)
	chmod 775 airport



# compile client
client.o: client.c place.h place.x 
	$(CC) $(CFLAGS) client.c

# compile place (client/server) program
place.o: place.c place.h place.x airport.h airport.x 
	$(CC) $(CFLAGS) place.c airport.h

# compile airport (server) program
airport_procs.o: airport_procs.c airport.h airport.x
	$(CC) $(CFLAGS) airport_procs.c

# generate place stubs, skeleton, and xdr
place_xdr.o: place_xdr.c place.h place.x 
	$(CC) $(CFLAGS) place_xdr.c

place_clnt.o: place_clnt.c place.h place.x
	$(CC) $(CFLAGS) -w place_clnt.c 

place_svc.o: place_svc.c place.h place.x
	$(CC) $(CFLAGS) -w place_svc.c 

# generate airport stubs, skeleton, and xdr
airport_clnt.o: airport_clnt.c airport.h airport.x
	$(CC) $(CFLAGS) -w airport_clnt.c

airport_svc.o: airport_svc.c airport.h airport.x
	$(CC) $(CFLAGS) -w airport_svc.c

airport_xdr.o: airport_xdr.c airport.h airport.x
	$(CC) $(CFLAGS) airport_xdr.c
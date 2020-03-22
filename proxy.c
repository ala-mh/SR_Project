#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PPORT 4444
#define DBPORT 8000
#define FPORT  9000

void main()
{
     struct FactureV
    {	int   IDVs[100];
        char  *NameV[100];
        double montantV[100];
        double SUMV;
	    int NV;
    } factureV;
	
	struct FactureP
    {	int   IDPs[100];
        char  *NameP[100];
        double montantP[100];
        double SUMP;
	    int NP;
    } factureP;

	struct FactureT
    {	struct FactureV FV;
		struct FactureP FP;
		double SUMT;
    } factureT;

    char  MSJ[1024];
    char  MSJSYS1[1024]="envoyer une Facutre Voiture";
    char  MSJSYS2[1024]="envoyer une Facutre Parapharmaceutiques";
    int n;
     
    /*------------------- LISTEN CONNECTION SOCKET Client------------------ */
    int ListeningSocket;
    struct sockaddr_in listenAddr;
    ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    printf("[+]Socket Listen Created Sucessfully.\n");
    memset(&listenAddr, '\0', sizeof(listenAddr));
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(PPORT);
    listenAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(ListeningSocket, (struct sockaddr *)&listenAddr, sizeof(listenAddr));
    printf("[+]Bind to Port number %d.\n", PPORT);

    listen(ListeningSocket, 5);
    printf("[+]Listening...\n");


    /*------------------- SERVICE CONNECTION SOCKET Client ------------------ */
    int ServiceSocket;
    struct sockaddr_in serviceAddr;
    socklen_t addr_size;


    /*------------------- DATABASE CONNECTION SOCKET ------------------ */
    int DatabaseSocket;
	struct sockaddr_in dbsysAddr;
    int counts;

    memset(&dbsysAddr, '\0', sizeof(dbsysAddr));
    dbsysAddr.sin_family = AF_INET;
    dbsysAddr.sin_port = htons(DBPORT);
    dbsysAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
     /*------------------- File CONNECTION SOCKET ------------------ */
    int FileSocket; 
    struct sockaddr_in FilesysAddr; 

    memset(&FilesysAddr, '\0', sizeof(FilesysAddr));
    FilesysAddr.sin_family = AF_INET;
    FilesysAddr.sin_port = htons(FPORT);
    FilesysAddr.sin_addr.s_addr = htonl (INADDR_ANY) ;

    while (1){
        /*------------------- Communicate with Client ------------------ */
                ServiceSocket = accept(ListeningSocket,(struct sockaddr *)&serviceAddr, &addr_size);
                     if (ServiceSocket == -1 ){
                          printf("Error while creating ServiceSocket !\n");
                          exit(0);
                        }
                     else  {printf("Successfully created ServiceSocket %d\n",ServiceSocket);}


                // Receive Name from Client
                if (recv(ServiceSocket, MSJ, sizeof(MSJ), 0) != -1)
                        { printf("[+]%s is received from Client \n",MSJ);}
                else
                        { printf(" ---- ! Nothing Arrived from Client ! ------ \n");}
       

        /*---------------- Communication with SYSTEM1 -------------------- */
                DatabaseSocket = socket(PF_INET, SOCK_STREAM, 0);
                printf("[+]Database Connexion Socket Created Sucessfully.\n");
                connect(DatabaseSocket, (struct sockaddr *)&dbsysAddr, sizeof(dbsysAddr));
                printf("[+]Connected to Database Server %d.\n",DatabaseSocket);
       
                // Send Name To System1
                send(DatabaseSocket, MSJSYS1, sizeof(MSJSYS1), 0); 
                printf("[+]%s is sent to Sys1 ..\n",MSJSYS1);
        
                memset(&MSJSYS1, '\0', sizeof(MSJSYS1));
    
                // Receive Results from System 1 
                if (recv(DatabaseSocket, &factureV, sizeof(factureV), 0) != -1)
                        { printf("[+]Facture Voiture is received from Sys1 ..\n"); }
                else
                        { printf(" ---- ! Nothing Arrived from Sys1 ! ------ \n");}

                printf("[+]Closing the connection with System1.\n");
                shutdown(DatabaseSocket, SHUT_RDWR);
                //close(DatabaseSocket);  

        
        
    /*---------------- Communication with SYSTEM2 -------------------- */      
                // create datagram socket 
                FileSocket = socket(AF_INET, SOCK_DGRAM, 0); 

                // Send Name To System2 
                sendto(FileSocket, MSJSYS2, sizeof(MSJSYS2), 0, (struct sockaddr*)&FilesysAddr, sizeof(FilesysAddr)); 
                printf("[+]%s is sent to Sys2 ..\n",MSJSYS2);
                
                memset(&MSJ, '\0', sizeof(MSJ));

                // waiting for response From Sys2 
                recvfrom(FileSocket, &factureP, sizeof(factureP), 0, (struct sockaddr*)&FilesysAddr, NULL); 
                printf("[+]Facture Parapharmaceutiques is received from Sys2 ..\n");
            
                // close the descriptor 
                //close(FileSocket); 

    /*---------------- Traitement -------------------- */   
                   factureT.FV=factureV;
                   factureT.SUMT= factureV.SUMV;
                 //factureT.FP=factureP;
                 //factureT.SUMT= factureV.SUMV+ factureP.SUMP;
                 

    /*---------------- Communication with Client -------------------- */      
                // Send Results To Client
                send(ServiceSocket, &factureT, sizeof(factureT), 0); 
                printf("[+]Facture Finale is sent to Client ..\n");


                printf("[+]Closing the connection with client.\n");
                shutdown(ServiceSocket, SHUT_RDWR);
                //close(ServiceSocket);
    }
}

//gcc proxy.c -o proxy

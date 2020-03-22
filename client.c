#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PPORT 4444

void main(int argc, char* argv[]){

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

	char MSJ[1024]="envoyer une Facutre Globale";

	// create socket client
	int clientSocket;  
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("[+]Client Socket Created Sucessfully.\n");

	// reference to socket proxy
	struct sockaddr_in serverAddr;
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PPORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect to server 
	connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Connected to Server.\n");

	//Sen Message to proxy
	send(clientSocket, MSJ, sizeof(MSJ), 0);
	printf("[+]%s is sent to Proxy ..\n",MSJ);

	//recieve result
	if (recv(clientSocket, &factureT, sizeof(factureT), 0) != -1)
        { 	
			printf("[+] Facture Totale arrived from proxy : \n");
			
			printf("\n\n-------- # Facture Voiture # -----------\n\n");
			printf(" Client | Montant \n");
			printf(" ------   -------\n");
			for (int i=0; i<factureT.FV.NV; i++){
				printf(" ------   -------\n");
				printf("   %d    |  %.2f\n",factureT.FV.IDVs[i],factureT.FV.montantV[i]);
			}
			printf("\n[+]Somme des factures voiture : %.2f\n", factureT.FV.SUMV);
		
			/*
			printf("\n\n-------- # Facture Parapharmaceutiques  # -----------\n\n");
			printf(" Client | Montant \n");
			printf(" ------   -------\n");
			for (int i=0; i<factureT.FP.NP; i++){
				printf(" ------   -------\n");
				printf("   %d    |  %.2f\n",factureT.FP.IDPs[i],factureT.FP.montantP[i]);
			}
			printf("\[+]Somme des factures Parapharmaceutiques : %.2f\n", factureT.FP.SUMP);
			*/
			
			printf(" ------  ------  ------  ------   -------\n");
			printf(" ------  ------  ------  ------   -------\n");
			printf("\n\n-------- # Facture Finale  # -----------\n\n");
			printf("[+]Somme du facture Finale: %.2f\n", factureT.SUMT);

		}
    else
        { printf(" ---- ! Nothing Arrived from proxy ! ------ \n");}


	printf("[+]Closing the connection.\n");
	shutdown(clientSocket,SHUT_RDWR);
	//close(clientSocket);

}

//gcc client.c -o client


/*
	//verification paramtre nom
    if ( argc == 2 ){
        send(clientSocket, argv[1], sizeof(argv[1]), 0);
        printf("[+]%s is sent to Server .. \n",argv[1]);
    }
    else if ( argc == 1 ) {
		char nom[100];
        printf("Veuillez saisir le nom : ");
		scanf("%s", nom);
		send(clientSocket, nom, sizeof(nom), 0);
        printf("[+]%s is sent to Server .. \n",argv[1]);
    }*/
	
/*
	//recieve result
    if (recv(clientSocket, &facture, sizeof(buffer), 0) != -1)
        { 	
			printf("[+] Facture arrived from proxy : \n");
			printf("\n\n-------- # Facture Finale # -----------\n\n");
			printf(" Client | Montant \n");
			printf(" ------   -------\n");
			printf("   %d    |  %.2f\n",facture.ID,facture.montant);
		
		}
    else
        { printf(" ---- ! Nothing Arrived from proxy ! ------ \n");}
    */

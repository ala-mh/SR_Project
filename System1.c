#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <mysql/mysql.h>

#define DBPORT 8000

void main()
{
    /*------------------- CONNECTION To BD ------------------ */
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    char *server = "localhost";
    char *user = "ala";
    char *password = "root"; /*password is not set in this example*/
    char *database = "sar";

    struct FactureV
    {	int   IDVs[100];
        char  *NameV[100];
        double montantV[100];
        double SUMV;
	    int NV;
    } factureV;

    char MSJSYS1[1024];

    conn = mysql_init(NULL);

    /* Connect to database */
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0))
    {
        printf("[!]Failed to connect MySQL Server %s. Error: %s\n", server, mysql_error(conn));
    }
    else
    {
        printf("[+]Connection With Database established successfully\n");
    }

    
    /*------------------- LISTEN CONNECTION SOCKET Client------------------ */
    int ListeningSocket;
    struct sockaddr_in listenAddr;
    
    ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("[+]Listening Socket Created Sucessfully  %d.\n", ListeningSocket);

    memset(&listenAddr, '\0', sizeof(listenAddr));

    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(DBPORT);
    listenAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    bind(ListeningSocket, (struct sockaddr *)&listenAddr, sizeof(listenAddr));
    printf("[+]Bind to Port number %d.\n", 8000);

    listen(ListeningSocket, 5);
    printf("[+]Listening...\n");

    /*------------------- SERVICE CONNECTION SOCKET Proxy ------------------ */
    int ServiceSocket;
    struct sockaddr_in serviceAddr;
    socklen_t addr_size;
    
    while (1)
    {
        /*------------------- Communicate with Proxy ------------------ */
        
        ServiceSocket = accept(ListeningSocket, (struct sockaddr *)&serviceAddr, &addr_size);
        if (ServiceSocket == -1 ){
                printf("Error while creating ServiceSocket !\n");
                exit(0);
            }
         else  {printf("Successfully created ServiceSocket %d\n",ServiceSocket);}


        // Receive Name from proxy
        if (recv(ServiceSocket, MSJSYS1, sizeof(MSJSYS1), 0) != -1)
        { printf("[+]%s arrived from proxy : \n", MSJSYS1);}
        else
        { printf(" ---- ! Nothing Arrived from proxy ! ------ \n");}
        

        //Fetch Name
        //char query[300];
        //sprintf(query, "select * from Factures where Nom LIKE ('%s')", buffer);
        if (mysql_query(conn,"select * from Factures"))
        {
            printf("Failed to execute query. Error: %s\n", mysql_error(conn));
            exit(0);
        }

        res = mysql_use_result(conn);
        
        int i = 0;
        factureV.SUMV=0;;
        factureV.NV=0;

        printf("[+]Fetching from results\n");
        while ((row = mysql_fetch_row(res)) != NULL)
        {
            factureV.IDVs[i] = atoi(row[0]);
            factureV.NameV[i] = row[1];
            factureV.montantV[i] = atof(row[2]);
            factureV.SUMV += factureV.montantV[i];
            i = i + 1;
        }
        factureV.NV = i;
        mysql_free_result(res);

        /*
        printf("\n\n-------- # Facture Voiture # -----------\n\n");
			printf(" Client | Montant \n");
			printf(" ------   -------\n");
			for (int i=0; i<factureV.NV; i++){
				printf(" ------   -------\n");
				printf("   %d    |  %.2f\n",factureV.IDVs[i],factureV.montantV[i]);
			}
			printf("[+]Somme des factures voiture : %.2f\n", factureV.SUMV);
        */

        // Send Results To Client
        send(ServiceSocket, &factureV, sizeof(factureV), 0);
        printf("[+]Facture Voiture is sent to Proxy ..\n");

        memset(&MSJSYS1, '\0', sizeof(MSJSYS1));
        
        printf("[+]Closing the connection.\n");
        shutdown(ServiceSocket, SHUT_RDWR);
        //close(ServiceSocket); 
          
    }
}

// gcc System1.c -o System1 `mysql_config --cflags --libs`
// gcc System1.c -lmysqlclient
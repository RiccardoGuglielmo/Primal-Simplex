
// Librerie:
    
    #include <stdio.h>
    #include <stdlib.h>
    
    
// Tolleranza:
                                          
    #define TOL 0.00001
    #define M 10000


// Definizione delle strutture dati:
    
    typedef struct
    {
        	long n, n1, m, m1, ops, *Stato, *Base;
            char check, artif, *Segno;
        	double **Mat;
    } Problem;

    
// Dichiarazione dei prototipi delle funzioni:
    
    void createTableau ( Problem *P, char *filename );
    void searchBases ( Problem *P );
    void primalSimplex ( Problem *P );
    void pivoting ( Problem *P, long row, long col );
    void printResults ( Problem *P, char *filename );
    void insertSlacks ( Problem *P );
    void searchBases ( Problem *P );
    void insertArtif ( Problem *P );
    char checkOpt ( Problem *P );
	long varOUT ( Problem *P, long row );
    long varIN ( Problem *P );
	void killer ( Problem *P );
    

// Funzione Principale:
    
    int main ( )
    {
		Problem P;
        
        createTableau ( &P, "esercizio.dat" );
        
        primalSimplex ( &P );
        
        printResults ( &P, "esercizio.dat" );

		killer ( &P );
        
        return 0;
    }


// Lettura del file e creazione del tableau (matrice):
    
    void createTableau ( Problem *P, char *filename )
    {
        long i, j, no, rr;
	    double cc;
	    
        
	    // Apri il file dati:

			FILE *fdata = fopen ( filename, "r" );
        

        // Leggi il numero di righe e colonne:

            fflush(stdin);
			fscanf ( fdata, "%d %d\n", &(P->m), &(P->n));
			P->n1 = P->n + 1;
			P->m1 = P->m + 1;
			P->Mat = (double**) malloc ( (P->m + 2) * sizeof(double*) );
			P->Segno = (char*) malloc ( (P->m + 1) * sizeof(char) );
			
			
        // Controlla avvenuta allocazione:
            
        	if ( P->Mat == NULL || P->Segno == NULL )
        		exit(-1);
        

        // Inizializza il Tableau:

        	// Alloca per righe la matrice:
        	
            	for ( i = 0; i < P->m + 2; i++ )
            	{
            		P->Mat[i] = (double*) malloc ( (P->n + P->m + 2) * sizeof(double) );
            		if ( P->Mat[i] == NULL )
            			exit(-1);
            	}
        
        	// Inizializza tutti i valori della matrice:
        	
            	for ( i = 0; i < P->m + 2; i ++ )
            	{
            		for ( j = 0; j < P->n + P->m + 2; j++ )
            			P->Mat[i][j] = 0.0;
            	}
			

        // Leggi il vettore dei "termini noti":

		for ( i = 1; i <= P->m; i++ )
			fscanf ( fdata, "%lf", &(P->Mat[i][0]) );
 

		// Leggi il vettore dei "versi":

		for ( i = 1; i <= P->m; i++ )
			fscanf ( fdata, "%hd", &(P->Segno[i]) );


		// Leggi la matrice dei coefficienti:

		for ( i = 0; i <= P->m; i++ )
		{
				for ( j = 1; j <= P->n; j++ )
					fscanf ( fdata, "%lf", &(P->Mat[i][j]) );
		}


		// Chiudi il file dati:

			fclose(fdata);
    }
        
    
// Simplesso Duale:
    
    void primalSimplex ( Problem *P )
    {
        long i, row, col;
        
        P->ops = 0;
        P->artif = 0;
        P->check = 0;
        P->Stato = (long*) malloc ( ( P->n + P->m + 2 ) * sizeof(long) );
        P->Base = (long*) malloc ( ( P->m + 2 ) * sizeof(long) );

		for ( i = 0; i < P->n + P->m + 2; i++ )
   			P->Stato[i] = 0;

		for ( i = 0; i < P->m + 2; i++ )
   			P->Base[i] = 0;
            
        
        // Inserisci nel tableau le variabili di slack:
            
            insertSlacks ( P );
            
            
        // Inserisci nel tableau la variabile artificiale (se necessaria):
    	    
            insertArtif ( P );
        
        
        // Trova variabili in base:
            
            searchBases ( P );
            
            
        // Itera finchè la soluzione non è ottima:
            
            while ( ! checkOpt ( P ) )
            {
                
                P->ops++;
                
                
                // Cerca variabile in base uscente:
                
                    col = varIN ( P );
                
                
                // Cerca variabile che entra in base:
                    
                    row = varOUT ( P, col );
                    if ( row == 0 ) return;  // soluzione non ammissibile;
    				
    			
    			// Aggiorna variabili in base:
    			    
    			    P->Stato[P->Base[row]] = 0;
    			    P->Stato[col] = row;
    			    P->Base[row] = col;
                    
                    
                // Pivoting sull'elemento in posizione (row, col):
                    
                    pivoting ( P, row, col );
                    
            }
    }
    

// Inserisci nel tableau le variabili di slack:
      
    void insertSlacks ( Problem *P )
    {
        long i, j;
        
        for ( i = 1; i <= P->m; i++ )
    	{
    		if ( P->Segno[i] == 0 ) continue;
   			P->Mat[i][P->n1] = (double) P->Segno[i];
   			if ( P->Segno[i] == 1 )
   			{
   			    P->Base[i] = P->n1;
   			    P->Stato[P->n1] = i;
   			}
   			P->n1++;
    	}
    }
    
        
// Trova variabili in base:

    void searchBases ( Problem *P )
    {
        long i, j;
        
        for ( j = P->n1 - 1; j > 0; j-- )
        {
            if ( P->Stato[j] != 0 ) continue;
            for ( i = P->m1 - 1; i > 0; i-- )
            {
                if ( P->Base[i] != 0 ) continue;
                if ( ! ( ( P->Mat[i][j] >= -TOL ) && ( P->Mat[i][j] <= TOL ) ) )
                {
                    pivoting ( P, i, j );
                    P->Base[i] = j;
                    P->Stato[j] = i;
                }
            }
        }
    }
    
    
// Inserisci nel tableau la variabile artificiale:
    
    void insertArtif ( Problem *P )
    {
        long i, j;
        
        for ( i = 1; i <= P->m; i++ )
    	{
            if ( P->Segno[i] == -1 )
   			{
                for ( j = 0; j < P->n1; j++ )
                    P->Mat[i][j] *= -1;
                if ( P->Mat[i][0] > TOL )
                {
                    P->artif++;
                    P->Mat[i][P->n1] = 1;
                    P->Stato[P->Base[i]] = 0;
                    P->Base[i] = P->n1;
                    P->Stato[P->n1] = i;
                    P->n1++;
                }
            }
    	}
    }
    
    
// Controlla soluzione ottima:
    
    char checkOpt ( Problem *P )
    {
        long j;
        
        for ( j = 1; j < P->n1; j++ )
            if ( P->Mat[0][j] > TOL )
                return 0;
        
        P->check = 1;
        
        return 1;
    }
    
    
// Cerca il termine noto minimo (variabile in base uscente):

    long varIN ( Problem *P )
    {
        long j, col = 1;
        
        for ( j = 2; j < P->n1; j++ )
            if ( P->Mat[0][j] > P->Mat[0][col] )
                col = j;
                
        return col;
    }  
    
    
// Cerca il rapporto minimo positivo (variabile che entra in base):
    
    long varOUT ( Problem *P, long col )
    {
        long i, row = P->m1;
        long count = 0;
        double rMin = M;
        
        for ( i = P->m1 - 1; i > 0; i-- )
        {
            if ( P->Mat[i][col] < TOL )
            {
                count++;
                continue;
            }
            if ( P->Mat[i][0] / P->Mat[i][col] <= rMin )
            {
                rMin = P->Mat[i][0] / P->Mat[i][col];
                row = i;
            }
        }
        
        if ( count == P->m1 - 1 )  // Soluzione non ammissibile;
        {
            P->check = 3;
            return 0;
        }
        
        return row;
    }  

    
// Pivoting:
    
    void pivoting ( Problem *P, long row, long col )
    {
        long i, j;
        double temp, pivot = P->Mat[row][col];
        
        
        // Dividi la riga "row" per il pivot:
            
            if ( pivot != 1 )
                for ( j = 0; j < P->n1; j++ )
                    P->Mat[row][j] /= pivot;
        
        for ( i = 0; i < P->m1; i++ )
        {
            if ( i == row ) continue;
            if ( ( P->Mat[i][col] > -TOL ) && ( P->Mat[i][col] < TOL ) ) continue;
            temp = P->Mat[i][col];
            for ( j = 0; j < P->n1; j++ )
                P->Mat[i][j] -= P->Mat[row][j] * temp;
        }
        
    }    
    
    
// Stampa del Tableau:

    void printResults ( Problem *P, char *filename )
    {
        long j;
        
        FILE *fresults = fopen ( "result.txt", "w" );
        
        fprintf ( fresults, "\n\n   ______________________________________ ");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |    Nome Problema  | %15s  |", filename );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |          Colonne  | %15ld  |", P->n );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |            Righe  | %15ld  |", P->m );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |           Slacks  | %15ld  |", P->n1 - P->n - 2 );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |    Vincolo artif  | %15d  |", P->artif );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fprintf ( fresults, "\n  |       Iterazioni  | %15ld  |", P->ops );
        fprintf ( fresults, "\n  |___________________|__________________|");
        fprintf ( fresults, "\n  |                   |                  |");
        fflush(fresults);
        if ( P->check >= 2 )
        {
			if ( P->check == 2 )
				fprintf ( fresults, "\n  |  Costo soluzione  |      Illimitata  |");
			if ( P->check == 3 )
				fprintf ( fresults, "\n  |  Costo soluzione  | non ammissibile  |");
			fprintf ( fresults, "\n  |___________________|__________________|");
        }
        else
        {
            fprintf ( fresults, "\n  |  Costo soluzione  | %15.2lf  |", P->Mat[0][0] );
            fprintf ( fresults, "\n  |___________________|__________________|");
            
            fprintf ( fresults, "\n\n\n\n            Soluzioni:\n ");
            for ( j = 1; j < P->n1; j++ )
                if ( P->Stato[j] != 0 )
                    fprintf( fresults, "\n         x%4ld   =  %9.2lf ", j, P->Mat[P->Stato[j]][0]);
        }
        
        /*fprintf ( fresults, "\n\n\n\n            Soluzioni:\n ");
        for ( j = 1; j < P->m1; j++ )
            fprintf( fresults, "\n         x%4ld   =  %9.2lf ", P->Base[j], P->Mat[j][0]);*/
        
        fclose(fresults);
			
    }


// Distruttore:

	void killer ( Problem *P )
	{
		long i;

		free ( P->Stato );
		free ( P->Base );
		free ( P->Segno );

		for ( i = 0; i < P->m + 2; i++ )
			free ( P->Mat[i] );
		free ( P->Mat );
	}

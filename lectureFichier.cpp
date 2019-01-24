/* StockageMax.csv = tableau 1X48 */
/* Lecture des stockages max de chaque sommet. Boucle de (NbSommet + 2) car on rajoute la racine et le puit*/
LectureFichier("StockageMax.csv");
for(int j = 0; j < NbSommet + 2; j++){
	StockageMax[j] = Tab[0][j];
	if(StockageMax[j] > 0){
		cout << "lecture : le sommet " << j << " a un stockagemax de " << StockageMax[j] << endl;
	}
}

/* Succ.csv = tableau 48X48 */
/* Succ[i][j] = 1 si on peut aller de i à j, =0 sinon */

LectureFichier("Succ.csv");
for (int i = 0; i <= NbSommet + 1; i++){
	for (int j = 0; j <= NbSommet +1; j++){
		Succ[i][j] = Tab[i][j];
		if(Succ[i][j] == 1){
			cout << "lecture : liasion entre le sommet " << i << " et " << j << endl;
		}
	}
}

/* CapaMaxheure.csv = tableau 48X48*/
/* Represente combien de quantite on peut envoyer d'un sommet i à un sommet j par heure */

LectureFichier("CapaMaxHeure.csv");
for (int i = 0; i <  NbSommet + 2 ; i++){
	for (int j = 0; j < NbSommet +2; j++){
		CapaMaxHeure[i][j] = Tab[i][j]
		if ((Succ[i][j]) && (CapaMaxHeure[i][j] > 0) ){
			cout << "lecture : le sommet " << i << "peut envoyer " << CapaMaxHeure[i][j] << " de quantite par heure au sommet" << j << endl;
		}
	}
}


/* Capabilite.csv = tableau 7X7 avec 1ere ligne et 1ere colonne inutile */
/* A quoi correspond ce fichier ?? */
LectureFichier("Capabilite.csv");
for (int p = 1; p <= NbFamille; p++){
	for(int j = 1; j<= 6; j++){
		Capabilite[p][j] = Tab[p][j];
		cout << "lecture : le produit " << p << "sommet : " << j << " capabilite de : " << Capabilite[i][j] << endl;
	}
}



/* CoefC.csv (=Coef Conversion) = tableau 7X1 avec 1ere ligne inutile*/

LectureFichier("CoefC.csv");
for(int p = 1; p <= NbFamille; p++){
	CoefC[p] = Tab[p][0];
	cout << "lecture : La famille " << p << " a un coeffConversion = " << CoefC[p] << endl;
}



/*DemandeQ.csv = tableau de 37X13 avec 1ere ligne et 1ere colonne inutile */
/* Les colonnes représentent les periodes (12), puis ils faut separer les lignes par groupe de 6,
6 lignes corresponde à un quai et à la demande de quantite pour cette famille */

/*DemandeQ[p][q][t] = la demande du quai q en produit p pour la periode t*/

LectureFichier("DemandeQ.csv");
int p = 0;
int q = 1;
for (int l = 1; l <= NbFamille*NbQuai; l++){
	p = p + 1;
	for (int t = 1; t <= NbPeriode; t++){
		DemandeQ[p][q][t] = Tab[l][t];
		cout << "lecture : Le quai " << q << "veut pour la periode " << t << " " << DemandeQ[p][q][t] << "de produit de la famille" << p << endl;
	}
	if (p == NbFamille){
		q = q+1;
		p = 0;
}




		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
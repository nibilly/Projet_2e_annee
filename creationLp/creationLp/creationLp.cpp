// creationLp.cpp : définit le point d'entrée pour l'application console.
//
#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

double Tab[49][49];

unsigned int mini(unsigned int a, unsigned int b)
{
	unsigned int res = a;
	if (b < a)
		res = b;
	return b;
}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
	unsigned int pos = txt.find(ch);
	unsigned int initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos <= txt.size()) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, mini(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

int LectureFichier(char *f_name)
{
	ifstream _fin(f_name, ios::in);
	std::vector<string> v;
	if (_fin)
	{
		string ligne;
		unsigned int i = 0;
		while (getline(_fin, ligne) && i<49)  // tant que l'on peut mettre la ligne dans "contenu"
		{
			split(ligne, v, ';');
			//  v=tokenizer( ligne,';');
			for (unsigned int j = 0; j<v.size(); j++) {
				//Tab[i][j] = atof(v[j].substr(0, v[j].size()).c_str());
				Tab[i][j] = atof(v[j].c_str());
				//cout <<i<< ";"<<j<<"="<<Tab[i][j]<< endl;
			}
			i++;
		}
	}
	else {
		cerr << "Impossible d'ouvrir le fichier !" << endl;
	}

	return 0;
}

int main()
{	
	const int NbFamille = 6;
	const int NbQuai = 6;
	const int NbPeriode = 12;
	const int NbSommet = 46;
	const int NbProduit = 15;

	double DemandeQ[NbFamille+1][NbQuai+1][NbPeriode+1];
	//double QuaiLvr[99];
	double Succ[NbSommet+2][NbSommet+2];
	double StockageMax[NbSommet+3];
	double CapaMaxHeure[NbSommet+3][NbSommet+3];
	double Capabilite[NbFamille+1][7];
	double CoefC[7];

	int DispersionPrdtHangar = 6;
	int DebCharg = 1;
	double TSDcible = 0.1;
	int H = 9999999;
	int CoutChangLigneProd = 1000000;
	int CoutChangHangar = 1000000;
	int CoutOuvLigne[NbFamille + 1];
	int CoutOuvHangare[NbSommet + 1];//TO complite
	int CoutStock[NbFamille + 1][NbSommet + 1];//TO complete
	int somme;

	CoutOuvLigne[1] = 10000;
	CoutOuvLigne[2] = 10000;
	CoutOuvLigne[3] = 20000;
	CoutOuvLigne[4] = 15000;
	CoutOuvLigne[5] = 15000;
	CoutOuvLigne[6] = 15000;

	//cedric

	/* StockageMax.csv = tableau 1X48 */
	/* Lecture des stockages max de chaque sommet. Boucle de (NbSommet + 2) car on rajoute la racine et le puit*/
	LectureFichier("StockageMax.csv");
	for (int j = 0; j < NbSommet + 2; j++) {
		StockageMax[j] = Tab[0][j];
		if (StockageMax[j] > 0) {
			//cout << "lecture : le sommet " << j << " a un stockagemax de " << StockageMax[j] << endl;
		}
	}

	/* Succ.csv = tableau 48X48 */
	/* Succ[i][j] = 1 si on peut aller de i à j, =0 sinon */

	LectureFichier("Succ.csv");
	for (int i = 0; i <= NbSommet + 1; i++) {
		for (int j = 0; j <= NbSommet + 1; j++) {
			Succ[i][j] = Tab[i][j];
			if (Succ[i][j] == 1) {
				//cout << "lecture : liasion entre le sommet " << i << " et " << j << endl;
			}
		}
	}

	/* CapaMaxheure.csv = tableau 48X48*/
	/* Represente combien de quantite on peut envoyer d'un sommet i à un sommet j par heure */

	LectureFichier("CapaMaxHeure.csv");
	for (int i = 0; i < NbSommet + 2; i++) {
		for (int j = 0; j < NbSommet + 2; j++) {
			CapaMaxHeure[i][j] = Tab[i][j];
			if ((Succ[i][j]) && (CapaMaxHeure[i][j] > 0)) {
				//cout << "lecture : le sommet " << i << "peut envoyer " << CapaMaxHeure[i][j] << " de quantite par heure au sommet" << j << endl;
			}
		}
	}

	
	/* Capabilite.csv = tableau 7X7 avec 1ere ligne et 1ere colonne inutile */
	/* A quoi correspond ce fichier ?? */
	LectureFichier("Capabilite.csv");
	for (int p = 1; p <= NbFamille; p++) {
		for (int j = 1; j <= 6; j++) {
			Capabilite[p][j] = Tab[p][j];
			//cout << "lecture : le produit " << p << "sommet : " << j << " capabilite de : " << Capabilite[p][j] << endl;
		}
	}



	/* CoefC.csv (=Coef Conversion) = tableau 7X1 avec 1ere ligne inutile*/
	
	LectureFichier("CoefC.csv");
	for (int p = 1; p <= NbFamille; p++) {
		CoefC[p] = Tab[p-1][0];
		//cout << "lecture : La famille " << p << " a un coeffConversion = " << CoefC[p] << endl;
	}



	/*DemandeQ.csv = tableau de 37X13 avec 1ere ligne et 1ere colonne inutile */
	/* Les colonnes représentent les periodes (12), puis ils faut separer les lignes par groupe de 6,
	6 lignes corresponde à un quai et à la demande de quantite pour cette famille */

	/*DemandeQ[p][q][t] = la demande du quai q en produit p pour la periode t*/

	LectureFichier("DemandeQ.csv");
	int p = 0;
	int q = 1;
	for (int l = 1; l <= NbFamille*NbQuai; l++) {
		p = p + 1;
		for (int t = 1; t <= NbPeriode; t++) {
			DemandeQ[p][q][t] = Tab[l][t];
			//cout << "lecture : Le quai " << q << "veut pour la periode " << t << " " << DemandeQ[p][q][t] << "de produit de la famille" << p << endl;
		}
		if (p == NbFamille) {
			q = q + 1;
			p = 0;
		}
	}


	//Nicolas

	ofstream fichier("t.lp", ios::out | ios::trunc);
	if (fichier)
	{
		fichier << " \\*Titre*\\ " << endl << endl;//Titre a inserer
		fichier << "Maximize" << endl;
		fichier << " obj: ";
		for (int p = 1; p <= NbFamille; p++) {
			for (int q = 1; q <= NbQuai; q++) {
				for (int t = 1; t <= NbPeriode - 1; t++) {
					fichier << " + " << " TsdQuai(" << p << "," << q << "," << t << ") ";
				}
			}
		}
		fichier << " " << endl;

		//2. contraintes
		fichier << endl << "Subject To" << endl;

		//=================== Definition des contraintes

		// contrainte 0
		for (int i = 0; i <= NbSommet + 1; i++) {
			for (int p = 1; p <= NbFamille; p++) {
				if (StockageMax[i] > 0) {
					fichier << "C0(" << i << "," << p << ") : Stock(" << i << "," << p << "," << 0 << ") = 0 " << endl;
				}
			}
		}
		// contrainte 1
		for (int i = 0; i <= NbSommet + 1; i++) {
			for (int j = 0; j <= NbSommet + 1; j++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[i][j] == 1) {
						fichier << "C1(" << i << "," << j << "," << t << "): ";
						for (int p = 1; p <= NbFamille; p++) {
							fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
						}
						fichier << " <= " << CapaMaxHeure[i][j] * 24 * 30 << endl;
					}
				}
			}
		}
		// contrainte 2 :
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbFamille; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (StockageMax[j] == 0 && j >= 1 && j <= NbSommet) {
						fichier << "C2(" << j << "," << p << "," << t << "): ";
						for (int i = 0; i <= NbSommet + 1; i++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
						for (int k = 0; k <= NbSommet + 1; k++) {
							if (Succ[j][k] == 1) {
								fichier << " - Flot(" << j << "," << k << "," << p << "," << t << ")";
							}
						}
						fichier << " = 0" << endl;
					}
				}
			}
		}

		// contrainte 3 : 31
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbFamille; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
						fichier << "C3(" << j << "," << p << "," << t << "): ";
						for (int i = 0; i <= NbSommet + 1; i++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
						fichier << " + Stock(" << j << "," << p << "," << t - 1 << ")";
						for (int k = 0; k <= NbSommet + 1; k++) {
							if (Succ[j][k] == 1) {
								fichier << " - Flot(" << j << "," << k << "," << p << "," << t << ")";
							}
						}
						fichier << " - Stock(" << j << "," << p << "," << t << ")";
						fichier << " = 0 " << endl;
					}
				}
			}
		}

		// contrainte 4 : 32
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbFamille; p++) {
				if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
					fichier << "C4(" << j << "," << p << "): ";
					for (int i = 0; i <= NbSommet + 1; i++) {
						for (int t = 1; t <= NbPeriode; t++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
					}
					for (int k = 0; k <= NbSommet + 1; k++) {
						for (int t = 1; t <= NbPeriode; t++) {
							if (Succ[j][k] == 1) {
								fichier << " - Flot(" << j << "," << k << "," << p << "," << t << ")";
							}
						}
					}
					fichier << " = 0" << endl;
				}
			}
		}

		// contrainte 5 : 33

		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int t = 1; t <= NbPeriode; t++) {
				if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
					fichier << "C5(" << j << "," << t << "): ";
					for (int p = 1; p <= NbFamille; p++) {
						fichier << " + Stock("<< j << "," << p << "," << t << ")";
							
					}
					fichier << " <= " << StockageMax[j] << endl;
				}
			}
		}
		
		// contrainte 6 : C41
		/*
		for (int j = 0; j <= NbSommet + 1; j++) {
			if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
				fichier << "C6(" << j <<"): ";
				for (int f = 1; f <= NbFamille; f++){
						for (int i = 0; i <= NbSommet + 1; i++) {
							for (int t = 1; p <= NbPeriode; p++) {
								if (Succ[i][j] == 1) {
									fichier << " + Flot(" << i << "," << j << "," << f << "," << t << ")";
								}
						}
					}
				}
				fichier << " - H*F(" << j<< ") <= 0"  << endl;
			}
		}

		// contrainte 7 : C42

		for (int j = 0; j <= NbSommet + 1; j++) {
			if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
				fichier << "C7(" << j << "): ";
				for (int f = 1; f <= NbFamille; f++) {
					for (int i = 0; i <= NbSommet + 1; i++) {
						for (int t = 1; p <= NbPeriode; p++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << f << "," << t << ")";
							}
						}
					}
				}
				fichier << " - F(" << j << ") >= 0" << endl;
			}
		}*/

		// contrainte 8 C43
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbFamille; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
						fichier << "C8(" << j << "," << p << "," << t << "): ";
						for (int i = 0; i <= NbSommet + 1; i++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
						fichier << " - " << StockageMax[j] << " G(" << j << "," << p << "," << t << ") ";
						fichier << " <= 0" << endl;
					}
				}
			}
		}

		// contrainte 9 C44
		for (int p = 1; p <= NbFamille; p++) {
			for (int t = 1; t <= NbPeriode; t++) {
				fichier << "C9(" << p << "," << t << "): ";
				for (int j = 0; j <= NbSommet + 1; j++) {
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
						fichier << "+ G(" << j << "," << p << "," << t << ")";
					}
				}
				fichier << " <= " << DispersionPrdtHangar << endl;
			}
		}

		/*pas vérifié : il manque le tableau QuaiLvr
		// contrainte 10 : C51
		
		for (int p = 1; p <= NbFamille; p++) {
			for (int q = 1; q <= NbQuai; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << "C10(" << p << "," << q << "," << t << "): ";
					for (int i = 0; i <= NbSommet + 1; i++) {
						if (Succ[i][QuaiLvr[q]] == 1) {
							fichier << " + Flot(" << i << "," << QuaiLvr[q] << "," << p << "," << t << ")";
						}
					}
					for (int s = 1; s <= NbPeriode; s++) {
						if (s >= t) {
							fichier << " - K(" << p << "," << q << "," << t << "," << s << ")";
						}
					}
					fichier << " = 0" << endl;
				}
			}
		}
		*/

		// contrainte 11 C53

		for (int p = 1; p <= NbFamille; p++) {
			for (int q = 1; q < NbQuai + 1; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << "C11(" << p << "," << q << "," << t << "): ";
					fichier << " TsdQuai(" << p << "," << q << "," << t << ")";
					fichier << " >= " << (1.0 - DemandeQ[p][q][t]) << endl;
				}
			}
		}

		// contrainte 12 C6


		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbFamille; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[0][j] == 1) {
						fichier << "C12(" << j << "," << p << "," << t << "): ";
						fichier << "Flot(" << 0 << "," << j << "," << p << "," << t << ")";
						fichier << "<=" << Capabilite[p][j] * 24 * CapaMaxHeure[0][j] * 30 << endl;
					}
				}
			}
		}

		// contrainte 13 C71

		for (int f = 1; f <= NbFamille; f++) {
			fichier << "C13(" << f << "): ";
			for (int j = 0; j <= NbSommet + 1; j++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[0][j] == 1) {
						fichier << " + Flot(" << 0 << "," << j << "," << f << "," << t << ")";
					}
				}
			}
			somme = 0;
			for (int q = 1; q <= NbQuai; q++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					somme += DemandeQ[f][q][t];
				}
			}
			fichier << " <= " << somme << endl;
		}


		// contrainte 14 :c72
		/* Je ne sais pas quoi sommer 
		fichier << "C14: ";
		for (int p = 1; p <= NbFamille; p++) {
			for (int q = 1; q <= NbQuai; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					somme += ////;
				}
			}
		}
		for (int p = 1; p <= NbFamille; p++) {
			for (int q = 1; q <= NbQuai; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << " - " << somme << " TsdQuai(" << p << "," << q << "," << t << ")";
				}
			}
		}

		fichier <<" + " << " TSDMoy " ;

		fichier << " = 0" << endl;
		*/
		// contrainte 15 c8
		for (int j = 1; j <= NbFamille; j++) {
			for (int t = 1; t <= NbPeriode; t++) {
				if (Succ[0][j] == 1) {

					fichier << "C15 ("<<j<<","<<t<<") : " ;

					for (int p = 1; p <= NbFamille; p++) {
						fichier<< "+" << (1+CoefC[p])<<" Flot("<<0<<","<<j<<","<<p<<","<<t<<") " ;

					}
					fichier << "<=" << CapaMaxHeure[0][j]*24*30 << endl ;
				}
			}
		}

		// contrainte 16 : c101
		
		for (int j = 1; j <= NbFamille; j++) {
			if (Succ[0][j] == 1) {
				for (int p = 1; p <= NbFamille; p++) {
					for (int t = 1; t <= NbPeriode; t++) {
						fichier << "C16(" << j << "," << p << "," << t << "): ";
						fichier << "- Flot(0," << j << "," << p << "," << t << ")";
						fichier << " + X(" << j << "," << p << "," << t << ")";
						fichier << " <= 0" << endl;
					}
				}
			}
		}

		// contrainte 17 : c102

		for (int j = 1; j <= NbFamille; j++) {
			if (Succ[0][j] == 1) {
				for (int p = 1; p <= NbFamille; p++) {
					for (int t = 1; t <= NbPeriode; t++) {
						fichier << "C17(" << j << "," << p << "," << t << "): ";
						fichier << "- Flot(0," << j << "," << p << "," << t << ")";
						fichier << " + " << H << " X(" << j << "," << p << "," << t << ")";
						fichier << " >= 0" << endl;
					}
				}
			}
		}

		// contrainte 18 : c103

		for (int j = 1; j <= NbFamille; j++) {
			if (Succ[0][j] == 1) {
				for (int p = 1; p <= NbFamille; p++) {
					for (int t = 1; t <= NbPeriode; t++) {
						fichier << "C18(" << j << "," << p << "," << t << "): ";
						fichier << "+ Flot(0," << j << "," << p << "," << t << ")";
						fichier << " - " << CapaMaxHeure[0][j]*24*30 << " X(" << j << "," << p << "," << t << ")";
						fichier << " <= 0" << endl;
					}
				}
			}
		}

		// contrainte 19 : c111
		for (int j = 1; j <= NbFamille; j++) {
			for (int j2 = 1; j2 <= NbFamille; j2++) {
				if (j != j2) {
					for (int p = 1; p <= NbFamille; p++) {
						for (int t = 1; t < NbPeriode; t++) {
							fichier << "C19(" << j << "," << j2 << "," << p << "," << t << "): ";
							for (int s = t + 1; s <= NbPeriode; s++) {
								fichier << "+X(" << j2 << "," << p << "," << s << ") ";
							}
							fichier << "+ " << H << " X(" << j << "," << p << "," << t << ") ";
							fichier << "- " << H << " Y(" << j << "," << j2 << "," << p << "," << t << ") ";
							fichier << "<= " << H << endl;
						}
					}
				}
			}
		}

		// contrainte 20 : c112
		for (int j = 1; j <= NbFamille; j++) {
			for (int j2 = 1; j2 <= NbFamille; j2++) {
				if (j != j2) {
					for (int p = 1; p <= NbFamille; p++) {
						for (int t = 1; t < NbPeriode; t++) {
							fichier << "C20(" << j << "," << j2 << "," << p << "," << t << "): ";
							
							fichier << "- X(" << j << "," << p << "," << t << ") ";
							fichier << "+ Y(" << j << "," << j2 << "," << p << "," << t << ") ";
							fichier << "<= 0" << endl;
						}
					}
				}
			}
		}

		// contrainte 21 : c113
		for (int j = 1; j <= NbFamille; j++) {
			for (int j2 = 1; j2 <= NbFamille; j2++) {
				if (j != j2) {
					for (int p = 1; p <= NbFamille; p++) {
						for (int t = 1; t < NbPeriode; t++) {
							fichier << "C21(" << j << "," << j2 << "," << p << "," << t << "): ";

							for (int s = t + 1; s <= NbPeriode; s++) {
								fichier << "-X(" << j2 << "," << p << "," << s << ") ";
							}
							fichier << "+ Y(" << j << "," << j2 << "," << p << "," << t << ") ";
							fichier << "<= 0" << endl;
						}
					}
				}
			}
		}

		//contrainte 22 : c121
		for (int j = 1; j <= NbSommet+1; j++)
		{
			for (int f = 1; f <= NbFamille; f++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet)
					{
						fichier << "C22(" << j << "," << f << "," << t << "): ";
						for (int i = 1; i <= NbSommet+1; i++)
						{
							if (Succ[i][j] == 1)
							{
								fichier << "- Flot(" << i << "," << j << "," << f << "," << t << ") ";
							}
						}
						fichier << "+ " << H << " * Z(" << j << "," << f << "," << t << ") ";
						fichier << ">= 0" << endl;
					}
				}
			}
		}

		//contrainte 23 : c122
		for (int j = 1; j <= NbSommet + 1; j++)
		{
			for (int f = 1; f <= NbFamille; f++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet)
					{
						fichier << "C23(" << j << "," << f << "," << t << "): ";
						for (int i = 1; i <= NbSommet + 1; i++)
						{
							if (Succ[i][j] == 1)
							{
								fichier << "- Flot(" << i << "," << j << "," << f << "," << t << ") ";
							}
						}
						fichier << "+ Z(" << j << "," << f << "," << t << ") ";
						fichier << "<= 0" << endl;
					}
				}
			}
		}

		//contrainte 24 : c123
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j]>0 && StockageMax[j2]>0)
						{
							fichier << "C24(" << j << "," << j2 << "," << f << "," << t << "): ";
							for (int s = t + 1; s <= NbPeriode; s++)
							{
								fichier << "+ Z(" << j2 << "," << f << "," << s << ") ";
							}
							fichier << "+ " << H << " Z(" << j << "," << f << "," << t << ") ";
							fichier << "- " << H << " A(" << j << "," << j2 << "," << f << "," << t << ") ";
							fichier << "<= " << H << endl;
						}
					}
				}
			}
		}

		//contrainte 25 : c124
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j]>0 && StockageMax[j2]>0)
						{
							fichier << "C25(" << j << "," << j2 << "," << f << "," << t << "): ";
							fichier << "- Z(" << j << "," << f << "," << t << ") ";
							fichier << "+ A(" << j << "," << j2 << "," << f << "," << t << ") ";
							fichier << "<= 0" << endl;
						}
					}
				}
			}
		}

		//contrainte 26 : c125
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j]>0 && StockageMax[j2]>0)
						{
							fichier << "C26(" << j << "," << j2 << "," << f << "," << t << "): ";
							for (int s = t + 1; s <= NbPeriode; s++)
							{
								fichier << "- Z(" << j2 << "," << f << "," << s << ") ";
							}
							fichier << "+ A(" << j << "," << j2 << "," << f << "," << t << ") ";
							fichier << "<= 0" << endl;
						}
					}
				}
			}
		}

		//contrainte 27 : c131
		for (int j = 1; j <= NbFamille; j++)
		{
			if (Succ[0][j] == 1)
			{
				fichier << "C27(" << j << "): ";
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t <= NbPeriode; t++)
					{
						fichier << "- X(" << j << "," << f << "," << t << ") ";
					}
				}
				fichier << "+ U(" << j << ") ";
				fichier << "<= 0" << endl;
			}
		}

		//contrainte 28 : c132
		for (int j = 1; j <= NbFamille; j++)
		{
			if (Succ[0][j] == 1)
			{
				fichier << "C28(" << j << "): ";
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t <= NbPeriode; t++)
					{
						fichier << "+ X(" << j << "," << f << "," << t << ") ";
					}
				}
				fichier << "- " << H << " U(" << j << ") ";
				fichier << "<= 0" << endl;
			}
		}
		/*MANQUE TABLEAUX ET A VERIFIER
		//contrainte 29 : c141
		fichier << "C29: ";
		for (int j = 1; j <= NbFamille; j++)
		{
			for (int j2 = 1; j2 <= NbFamille; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t <= NbPeriode; t++)
					{
						if (j != j2)
						{
							fichier << "- " << CoutChangLigneProd << " Y(" << j << "," << j2 << "," << f << "," << t << ") ";
						}
					}
				}
			}
		}
		fichier << "+ CoutChaLigne = 0" << endl;

		//contrainte 30 : c142
		fichier << "C30: ";
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j] > 0 && StockageMax[j2] > 0)
						{
							fichier << "- " << CoutChangHangar << " A(" << j << "," << j2 << "," << f << "," << t << ") ";
						}
					}
				}
			}
		}
		fichier << "+ CoutChaHangar = 0" << endl;

		//contrainte 31 : c143
		fichier << "C31: ";
		for (int j = 1; j <= NbFamille; j++)
		{
			fichier << "- " << CoutOuvLigne[j] << " U(" << j << ") ";
		}
		fichier << "+ CoutOuverLigne = 0" << endl;

		//contrainte 32 : c144
		fichier << "C32: ";
		for (int j = 1; j <= NbSommet; j++)
		{
			if (StockageMax[j] > 0)
			{
				fichier << "- " << CoutOuvHangare[j] << " F(" << j << ") ";
			}
		}
		fichier << "+ CoutOuverHangar = 0" << endl;

		//contrainte 33 : c145
		fichier << "C33: ";
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int f = 1; f <= NbFamille; f++)
			{
				for (int t = 1; t < NbPeriode; t++)
				{
					fichier << "- " << CoutStock[f][j] << " Stock(" << j << "," << f << "," << t << ") ";
				}
			}
		}
		fichier << "+ CoutStokage = 0" << endl;
		MANQUE TABLEAU*/

		//A VERIFIER
		fichier << endl << "Bounds" << endl;
		for (int f = 1; f <= NbFamille; f++)
		{
			for (int f2 = 1; f2 <= NbFamille; f2++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					fichier << "0 <= TsdQuai(" << f << "," << f2 << "," << t << ") <= 1" << endl;
				}
			}
		}
		for (int f = 1; f <= NbFamille; f++)
		{
			for (int f2 = 1; f2 <= NbFamille; f2++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					fichier << "0 <= X(" << f << "," << f2 << "," << t << ") <= 1" << endl;
				}
			}
		}
		for (int f = 1; f <= NbFamille; f++)
		{
			for (int f2 = 1; f2 <= NbFamille; f2++)
			{
				if (f != f2)
				{
					for (int f3 = 0; f3 <= NbFamille; f3++)
					{
						for (int t = 1; t <= NbPeriode; t++)
						{
							fichier << "0 <= Y(" << f << "," << f2 << "," << f3 << "," << t << ") <= 1" << endl;
						}
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet + 1; j++)
		{
			for (int f = 1; f <= NbFamille; f++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet)
					{
						fichier << "0 <= Z(" << j << "," << f << "," << t << ") <= 1" << endl;
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j] > 0 && StockageMax[j2] > 0)
						{
							fichier << "0 <= A(" << j << "," << j2 << "," << t << "," << t << ") <= 1" << endl;
						}
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			if (StockageMax[j] > 0)
			{
				fichier << "0 <= F(" << j << ") <= 1" << endl;
			}
		}
		for (int j = 1; j <= NbSommet; j++) 
		{
			for (int p = 1; p <= NbFamille; p++) 
			{
				for (int t = 1; t <= NbPeriode; t++) 
				{
					if (StockageMax[j] > 0) 
					{
						fichier << "0 <= G(" << j << "," << p << "," << t << ") <= 2" << endl;
					}
				}
			}
		}
		for (int f = 1; f <= NbFamille; f++)
		{
			fichier << "0 <= U(" << f << ") <= 1" << endl;
		}
		fichier << "CoutChaLigne free" << endl << "CoutChaHangar free" << endl << "CoutOuverLigne free" << endl << "CoutOuverHangar free" << endl << "CoutStokage free" << endl;
		
		fichier << endl << "Generals" << endl;
		for (int i = 0; i <= NbSommet + 1; i++) {
			for (int j = 0; j <= NbSommet + 1; j++) {
				if (Succ[i][j] == 1) {
					for (int t = 1; t <= NbPeriode; t++) {
						for (int p = 1; p <= NbFamille; p++) {
							fichier << "Flot(" << i << "," << j << "," << p << "," << t << ")";
						}
					}
				}
			}
		}
		for (int j = 0; j <= NbSommet + 1; j++) {
			if (StockageMax[j] > 0 && j >= 1 && j <= NbSommet) {
				for (int p = 1; p <= NbFamille; p++) {
					for (int t = 1; t <= NbPeriode; t++) {
						fichier << "Stock(" << j << "," << p << "," << t << ")" << endl;
					}
				}
			}
		}
		for (int f = 1; f <= NbFamille; f++) {
			for (int p = 1; p <= NbFamille; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << "X(" << f << "," << p << "," << t << ")" << endl;
				}
			}
		}
		for (int j = 1; j <= NbFamille; j++) {
			for (int j2 = 1; j2 <= NbFamille; j2++) {
				if (j != j2) {
					for (int p = 1; p <= NbFamille; p++) {
						for (int t = 1; t <= NbPeriode; t++) {
							fichier << " Y(" << j << "," << j2 << "," << p << "," << t << ") ";
						}
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int f = 1; f <= NbFamille; f++)
			{
				for (int t = 1; t < NbPeriode; t++)
				{
					if (StockageMax[j] > 0)
					{
						fichier << "Z(" << j << "," << f << "," << t << ")" << endl;
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int j2 = 1; j2 <= NbSommet; j2++)
			{
				for (int f = 1; f <= NbFamille; f++)
				{
					for (int t = 1; t < NbPeriode; t++)
					{
						if (j != j2 && StockageMax[j]>0 && StockageMax[j2]>0)
						{
							fichier << "A(" << j << "," << j2 << "," << f << "," << t << ")" << endl;
						}
					}
				}
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			if (StockageMax[j] > 0)
			{
				fichier << "F(" << j << ")" << endl;
			}
		}
		for (int j = 1; j <= NbSommet; j++)
		{
			for (int p = 1; p <= NbFamille; p++)
			{
				for (int t = 1; t <= NbPeriode; t++)
				{
					if (StockageMax[j] > 0)
					{
						fichier << "G(" << j << "," << p << "," << t << ")" << endl;
					}
				}
			}
		}
		for (int f = 1; f <= NbFamille; f++)
		{
			fichier << "U(" << f << ")" << endl;
		}
		fichier << "CoutChaLigne" << endl << "CoutChaHangar" << endl << "CoutOuverLigne" << endl << "CoutOuverHangar" << endl << "CoutStokage" << endl << endl << "End" << endl;
		//FIN A VERIFIER
		fichier.close();
	}//fi fichier
	return 0;
}



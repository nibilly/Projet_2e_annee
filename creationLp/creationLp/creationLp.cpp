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

double Tab[500][500];

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
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
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
	std::vector<std::string> v;
	if (_fin)
	{
		string ligne;
		int i = 0;
		while (getline(_fin, ligne))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			split(ligne, v, ';');
			//  v=tokenizer( ligne,';');
			for (int j = 0; j<v.size(); j++) {
				Tab[i][j] = atof(v[j].substr(0, v[j].size()).c_str());
				// cout <<i<< ";"<<j<<"="<<Tab[i][j]<< endl;
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
	int NbFamille = 6;
	int NbQuai = 5;
	int NbPeriode = 12;

	int NbSommet = 46;
	int NbProduit = 15;
	int DispersionPrdtHangar = 6;
	int DemandeQ[99][99][99];
	int QuaiLvr[99];
	int Succ[189][189];
	double StockageMax[48];
	int CapaMaxHeure[189][189];
	/*
	for (int i = 0; i < 48; i++)
	{
		StockageMax[i] = rand() % 100;
	}
	for (int i = 0; i < 189; i++)
	{
		for (int j = 0; j < 189; j++)
		{
			Succ[i][j] = rand() % 100;
			CapaMaxHeure[i][j] = rand() % 100;
		}
	}

	for (int i = 0; i < 6; i++)
	{
		QuaiLvr[i] = rand() % 100;
	}

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 6; j++)
			for (int k = 0; k < 4; k++)
			
				DemandeQ[i][j][k] = rand() % 100;
				*/


	int DebCharg = 1;
	double TSDcible = 0.1;

	//cedric

	LectureFichier("DemandeQ.csv");
	int p = 0;
	int q = 1;
	for (int l = 1; l <= NbFamille*NbQuai; l++) {
		p = p + 1;
		for (int t = 1; t <= NbPeriode; t++) {
			DemandeQ[p][q][t] = Tab[l][t];
			cout << "lecture : famille = " << p << " quai : = " << q << " periode = " << t << " demandeQuai = " << DemandeQ[p][q][t] << endl;
		}
		if (p == NbFamille) {
			p = 0;
			q = q + 1;
		}
	}
	/*
	LectureFichier("Capabilite.csv");
	for (int p = 1; p <NbProduit; p++) {
		for (int j = 1; j <= 6; j++) {
			Capabilite[p][j] = Tab[p][j];
			cout << "lecture : produit = " << p << " sommet = " << j << " capabilite = " << Capabilite[p][j] << endl;
		}
	}*/

	LectureFichier("StockageMax.csv");
	for (int j = 0; j <= NbSommet + 2; j++) {    // Il y a 49 valeurs dans StockageMax.csv pour 46 sommets ?? 
		StockageMax[j] = Tab[0][j];
		if (StockageMax[j]>0) {
			cout << "lecture :  sommet = " << j << " stockagemax = " << StockageMax[j] << endl;
		}
	}
	/*
	LectureFichier("CoefC.csv");
	for (int p = 0; p <= NbFamille - 1; p++) {
		CoefC[p + 1] = Tab[p][0];
		cout << "lecture :  famille = " << p << " CoefC = " << CoefC[p] << endl;
	}*/

	LectureFichier("Succ.csv");
	for (int i = 0; i <= NbSommet + 1; i++) {
		for (int j = 0; j <= NbSommet + 1; j++) {
			Succ[i][j] = Tab[i][j];
			if (Succ[i][j] == 1) {
				cout << "lecture : sommet1 = " << i << " sommet2 = " << j << " Succ = " << Succ[i][j] << endl;
			}
		}
	}
	
	LectureFichier("CapaMaxHeure.csv");
	for (int i = 0; i <= NbSommet + 1; i++) {
		for (int j = 0; j <= NbSommet + 1; j++) {
			CapaMaxHeure[i][j] = Tab[i][j];
			if ((Succ[i][j] == 1) && (CapaMaxHeure[i][j]>0)) {
				cout << "lecture : sommet1 = " << i << " sommet2 = " << j << " capamaxheure = " << CapaMaxHeure[i][j] << endl;
			}
		}
	}



	LectureFichier("QuaiLvr.csv");
	for (int q = 1; q <= NbQuai; q++) {
		QuaiLvr[q] = Tab[q][0];
		cout << "lecture :  quai = " << q << " quailivraison = " << QuaiLvr[q] << endl;
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
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << " + " << " TsdQuai(" << p << "," << q << "," << t << ") ";
				}
			}
		}
		fichier << " " << endl;

		//2. contraintes
		fichier << endl << "Subject To" << endl;

		//=================== Definition des contraintes
		
		// contrainte 0
		for (int i=0;i<=NbSommet+1;i++){
			for (int p=1;p<=NbProduit;p++) {
				if(StockageMax[i]>0) {
					fichier << "C0("<<i<<","<<p<<") : Stock("<<i<<","<<p<<","<<0<<") = 0 " << endl;
				}
			}
		}
		// contrainte 1
		for (int i = 0; i <= NbSommet + 1; i++) {
			for (int j = 0; j <= NbSommet + 1; j++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[i][j] == 1) {
						fichier << "C1(" << i << "," << j << "," << t << "): ";
						for (int p = 1; p <= NbProduit; p++) {
							fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
						}
						fichier << " <= " << CapaMaxHeure[i][j] * 24 << endl;
					}
				}
			}
		}
		// contrainte 2 :
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
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

		// contrainte 3 :
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
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

		// contrainte 4 :
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
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

		// contrainte 5 :

		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int t = 1; t <= NbPeriode; t++) {
				if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
					fichier << "C5(" << j << "," << t << "): ";
					for (int p = 1; p <= NbProduit; p++) {
						for (int i = 0; i <= NbSommet + 1; i++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
					}
					fichier << " - " << StockageMax[j] << "F(" << j << ") ";
					fichier << " <= 0" << endl;
				}
			}
		}

		// contrainte 6 :
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
						fichier << "C6(" << j << "," << p << "," << t << "): ";
						for (int i = 0; i <= NbSommet + 1; i++) {
							if (Succ[i][j] == 1) {
								fichier << " + Flot(" << i << "," << j << "," << p << "," << t << ")";
							}
						}
						fichier << " - " << StockageMax[j] << "G(" << j << "," << p << "," << t << ") ";
						fichier << " <= 0" << endl;
					}
				}
			}
		}

		// contrainte 7
		for (int p = 1; p <= NbProduit; p++) {
			for (int t = 1; t <= NbPeriode; t++) {
				fichier << "C7(" << p << "," << t << "): ";
				for (int j = 0; j <= NbSommet + 1; j++) {
					if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
						fichier << " + G(" << j << "," << p << "," << t << ")";
					}
				}
				fichier << " <= " << DispersionPrdtHangar << endl;
			}
		}


		// contrainte 8 :

		for (int p = 1; p <= NbProduit; p++) {
			for (int q = 1; q <= NbQuai; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					fichier << "C8(" << p << "," << q << "," << t << "): ";
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
		// contrainte 9

		for (int p = 1; p <= NbProduit; p++) {
			for (int q = 0; q < NbQuai + 1; q++) {
				for (int t = 0; t < NbPeriode; t++) {
					fichier << "C9(" << p << "," << q << "," << t << "): ";
					fichier << " + DemandeQuai(" << p << "," << q << "," << t << ")";
					fichier << " = " << (DemandeQ[p][t]) << endl;
					fichier << " = 0" << endl;
				}
			}
		}




		// contrainte 10


		for (int p = 1; p <= NbProduit; p++) {
			for (int q = 1; q <= NbQuai + 1; q++) {
				for (int t = 1; t <= NbPeriode; t++) {
					//                     fichier << "C10("<<p<<","<<q<<","<<t<<"): " ;

					for (int s = 1; s <= NbPeriode; s++) {
						if (s >= DebCharg && s <= t) {

							//           fichier<< " + K("<<p<<","<<q<<","<<t<<","<<s<<")";
						}
					}

					//     fichier <<" - " << " DemandeQuai("<<p<<","<<q<<","<<t<<") TSD("<<p<<","<<q<<","<<t<<") = 0" << endl;
					// fichier <<" - " << DemandeQ[p][t]<< " TSD("<<p<<","<<q<<","<<t<<") = 0" << endl;
				}
			}
		}

		// contrainte 11


		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[0][j] == 1) {


						//         fichier << "C11("<<j<<","<<p<<","<<t<<"): " ;

						//        fichier << " Flot("<<0<<","<<j<<","<<p<<","<<t<<")";

						//        fichier<<"<=" << Capabilite[p][j]*24*CapaMaxHeure[0][j] << endl;
					}
				}
			}
		}


		// contrainte 12 :

		fichier << "C12: ";
		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				for (int t = 1; t <= NbPeriode; t++) {
					if (Succ[0][j] == 1) {

						//           fichier<< " - "<< Tot <<" Flot ("<<0<<","<<j<<","<<p<<","<<t<<")";

					}
				}
			}
		}
		//  fichier <<" + " << " TSDMoy " ;

		// fichier<<" = 0" << endl;

		// contrainte 13

		for (int j = 1; j <= 6; j++) {
			for (int t = 1; t <= NbPeriode; t++) {
				if (Succ[0][j] == 1) {

					//      fichier << "C13 ("<<j<<","<<t<<") : " ;

					for (int p = 1; p <= NbProduit; p++) {
						//       fichier<< "+" << (1+CoefC[p])<<" Flot("<<0<<","<<j<<","<<p<<","<<t<<") " ;

					}
					//     fichier << "<=" << CapaMaxHeure[0][j]*24 << endl ;
				}
			}
		}

		// contrainte 14 :

		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				for (int t = 1; t <= NbPeriode - 2; t++) {
					if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
						//      fichier << "C14("<<j<<","<<p<<","<<t<<"): " ;

						for (int k = 0; k <= NbSommet + 1; k++) {
							if (Succ[j][k] == 1) {

								//             fichier<< " + Flot("<<j<<","<<k<<","<<p<<","<<t+2<<")";
							}
						}

						for (int i = 0; i <= NbSommet + 1; i++) {
							for (int s = 1; s <= NbPeriode - 2; s++) {
								if (s <= t && Succ[i][j] == 1) {

									//          fichier<< " - Flot("<<i<<","<<j<<","<<p<<","<<s<<")";
								}
							}
						}

						for (int k = 0; k <= NbSommet + 1; k++) {
							for (int s = 1; s <= NbPeriode - 2; s++) {
								if (s <= t + 1 && Succ[j][k] == 1) {
									//            fichier<< " + Flot("<<j<<","<<k<<","<<p<<","<<s<<")";
								}
							}
						}
						// fichier<<"<= 0" << endl;

					}
				}
			}
		}
		// contrainte 15 :

		for (int j = 0; j <= NbSommet + 1; j++) {
			for (int p = 1; p <= NbProduit; p++) {
				if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
					fichier << "C15(" << j << "," << p << "): ";

					for (int k = 0; k <= NbSommet + 1; k++) {
						if (Succ[j][k] == 1) {
							fichier << " + Flot(" << j << "," << k << "," << p << "," << 1 << ")";
							fichier << " + Flot(" << j << "," << k << "," << p << "," << 2 << ")";
						}
					}
					fichier << " - Stock(" << j << "," << p << "," << 0 << ")";

					fichier << "<= 0" << endl;

				}
			}
		}


		// contrainte 16 :

		for (int j = 0; j <= NbSommet + 1; j++) {
			if (StockageMax[j]>0 && j >= 1 && j <= NbSommet) {
				fichier << "C16(" << j << "): ";

				for (int i = 0; i <= NbSommet + 1; i++) {
					for (int p = 1; p <= NbProduit; p++) {
						if (Succ[i][j] == 1) {

							fichier << " + Flot(" << i << "," << j << "," << p << "," << NbPeriode - 1 << ")";
							fichier << " + Flot(" << i << "," << j << "," << p << "," << NbPeriode << ")";
						}
					}
				}


				fichier << "= 0" << endl;

			}
		}

		// contrainte 17 :
		fichier << "C17: ";


		fichier << " + " << " TSDMoy ";
		fichier << " >= " << TSDcible << endl; 



			// fichier <<" + " << " TSDMoy "  ;
			// fichier <<" - " <<   TSDcible  ;
			//fichier<<" >= 0" << endl;



			// contrainte 18




			/*       for (int j=0;j<=NbSommet+1;j++){
			for( int p=1;p<=NbProduit;p++){
			for (int t=1;t<=NbPeriode;t++){
			if ( Succ[0][j]==1) {

			fichier << "C18("<<j<<","<<p<<","<<t<<"): ";

			fichier << " + X("<<j<<","<<p<<","<<t<<") " ;
			fichier<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


			fichier << " <= " << 0 << endl;

			}
			}
			}
			}
			// contrainte 19




			for (int j=0;j<=NbSommet+1;j++){
			for( int p=1;p<=NbProduit;p++){
			for (int t=1;t<=NbPeriode;t++){
			if ( Succ[0][j]==1) {

			fichier << "C19("<<j<<","<<p<<","<<t<<"): ";

			fichier << " + "<< H <<" X("<<j<<","<<p<<","<<t<<") " ;
			fichier<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


			fichier << " >= " << 0 << endl;

			}
			}
			}
			}


			// contrainte 20

			for (int j=0;j<=NbSommet+1;j++){
			for( int p=1;p<=NbProduit;p++){
			for (int t=1;t<=NbPeriode;t++){
			if ( Succ[0][j]==1) {

			fichier << "C20("<<j<<","<<p<<","<<t<<"): ";

			fichier << " + "<<ProdMin<<" X("<<j<<","<<p<<","<<t<<") " ;
			fichier<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


			fichier << " <= " << 0 << endl;

			}
			}
			}
			}

			// contrainte 21 :


			fichier << "C21 " ;
			for (int j=0;j<=NbSommet+1;j++){
			if( StockageMax[j]>0 && j>=1 && j<=NbSommet){


			fichier<< " + "<< "F("<<j<<") " ;

			}
			}
			fichier <<" = " <<   NbHangerMax << endl ;
			// fichier<<"= 0" << endl;*/



		fichier.close();
	}//fi fichier
    return 0;
}



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int LectureFichier(char *f_name);

void Generer_PL(char *f_name);



double Tab[500][500];
int DemandeQ[5][6][12];

int NbSommet=46;
int SetSommet[47];
int NbFamille = 6;
int SetFamille[6];
int NbPeriode = 12;
int SetPeriode[12];
int NbQuai=6;
int SetQuai[6];



int Demande[5][4];

double StockageMax[48];
int CapaMaxHeure[189][189];
int Capabilite[5][6];
float  CoefC[5];
int    QuaiLvr[6];
int Succ[189][189];
int NbProduit=6;


int NbSommetquai=6;
int SetSommetquai [] = {41,42,43,44,45,46};


int H=1000000;
int ProdMin=66;
int DispersionPrdtHangar=6;
double TSDcible=0.1;
int DebCharg=1;
int NbHangerMax=3;
double Tot=0.00008615;


int main ()
{
   char *f_name="Data_lp.lp";

   LectureFichier("DemandeQ.csv");
   int p = 0;
   int q = 1;
   for (int l = 1; l <=NbFamille*NbQuai; l++) {
        p = p + 1;
        for (int t =1; t <= NbPeriode; t++){
          DemandeQ[p][q][t] = Tab[l][t];
          cout << "lecture : famille = " << p <<  " quai : = " << q << " periode = " << t<< " demandeQuai = " << DemandeQ[p][q][t] << endl;
       }
       if (p == NbFamille) {
           p=0;
           q=q+1;
       }
   }

    LectureFichier("Capabilite.csv");
    for(int p = 1; p <NbProduit; p++){
        for(int j= 1;j<=6;j++){
            Capabilite[p][j]=Tab[p][j];
            cout << "lecture : produit = " << p <<  " sommet = " << j<< " capabilite = " << Capabilite[p][j] << endl;
        }
    }

    LectureFichier("StockageMax.csv");
    for( int j=0;j<=NbSommet+2;j++){    // Il y a 49 valeurs dans StockageMax.csv pour 46 sommets ?? 
        StockageMax[j]=Tab[0][j];
        if(StockageMax[j]>0){
        cout << "lecture :  sommet = " << j<< " stockagemax = " << StockageMax[j] << endl;
        }
    }

    LectureFichier("CoefC.csv");
    for (int p = 0; p <= NbFamille-1 ; p++) {
         CoefC[p+1] = Tab[p][0];
         cout << "lecture :  famille = " << p << " CoefC = " << CoefC[p] << endl;
    }

   LectureFichier("Succ.csv");
   for (int i=0;i<=NbSommet+1;i++){
      for(int j=0;j<=NbSommet+1;j++){
         Succ[i][j]=Tab[i][j];
         if(Succ[i][j]==1){
         cout << "lecture : sommet1 = " << i <<  " sommet2 = " << j<< " Succ = " << Succ[i][j] << endl;
         }
     }
   }

   LectureFichier("CapaMaxHeure.csv");
   for (int i=0;i<=NbSommet+1;i++){
      for(int j=0;j<=NbSommet+1;j++){
            CapaMaxHeure[i][j]= Tab[i][j];
            if((Succ[i][j]==1)&&(CapaMaxHeure[i][j]>0)){
             cout << "lecture : sommet1 = " << i <<  " sommet2 = " << j<< " capamaxheure = " << CapaMaxHeure[i][j] << endl;
            }
      }
   }



    LectureFichier("QuaiLvr.csv");
    for (int q = 1; q <= NbQuai ; q++) {
              QuaiLvr[q] = Tab[q][0];
               cout << "lecture :  quai = " << q<< " quailivraison = " << QuaiLvr[q] << endl;
        }



    //Generer le fichier

    Generer_PL(f_name);
    cout << "Generation LP Terminee" << endl;

    //Generer_PL_modele_PPRV2(f_name,M,E,Li,Cj);
    cout << "Please enter some integers (enter 0 to end):\n";

    return 0;
}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    unsigned int pos = txt.find( ch );
    unsigned int initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

//Génération du fichier LP
void Generer_PL(char *f_name)
{
	cout << "Creation du fichier LP.." << endl;
	ofstream _fout(f_name);
    int i;

   //Fonction objectif

     _fout << "Maximize" << endl;
     _fout << " obj: ";

     for(int p=1;p<=NbFamille;p++){
         for(int q=1;q<=NbQuai;q++){
             for(int t=1;t<=NbPeriode;t++){
                _fout <<" + "<< " TsdQuai("<<p<<","<<q<<","<<t<<") " ;
             }
         }
     }
     _fout << " " <<endl;

     //2. contraintes
	_fout << endl << "Subject To" << endl;

     //=================== Definition des contraintes
/*
// contrainte 0
	for (int i=0;i<=NbSommet+1;i++){
        for (int p=1;p<=NbProduit;p++) {
             if(StockageMax[i]>0) {
             _fout << "C0("<<i<<","<<p<<") : Stock("<<i<<","<<p<<","<<0<<") = 0 " << endl;
             }
        }
    }

// contrainte 1
    for (int i=0;i<=NbSommet+1;i++) {
         for (int j=0;j<=NbSommet+1;j++) {
              for (int t=1;t<=NbPeriode;t++){
                   if (Succ[i][j]==1) {
                       _fout << "C1("<<i<<","<<j<<","<<t<<"): " ;
                       for( int p=1;p<=NbProduit;p++){
                           _fout << " + Flot("<<i<<","<<j<<","<<p<<","<<t<<")" ;
                       }
                       _fout << " <= " << CapaMaxHeure[i][j]*24 <<endl ;
                   }
              }
		  }
      }

// contrainte 2 :
   for (int j=0;j<=NbSommet+1;j++){
        for (int p=1;p<=NbProduit;p++){
             for (int t=1;t<=NbPeriode;t++){
                  if( StockageMax[j]==0 && j>=1 && j<=NbSommet){
                     _fout << "C2("<<j<<","<<p<<","<<t<<"): ";
                     for ( int i=0;i<=NbSommet+1;i++ ){
                           if (Succ[i][j]==1) {
                               _fout<< " + Flot("<<i<<","<<j<<","<<p<<","<<t<<")";
                         }
                     }
                     for ( int k=0;k<=NbSommet+1;k++ ){
                           if (Succ[j][k]==1) {
                              _fout<< " - Flot("<<j<<","<<k<<","<<p<<","<<t<<")";
                           }
                     }
                     _fout<<" = 0" << endl;
                 }
            }
       }
   }

// contrainte 3 :
   for (int j=0;j<=NbSommet+1;j++){
        for (int p=1;p<=NbProduit;p++){
             for (int t=1;t<=NbPeriode;t++){
                  if( StockageMax[j]>0 && j>=1 && j<=NbSommet){
                     _fout << "C3("<<j<<","<<p<<","<<t<<"): ";
                     for ( int i=0;i<=NbSommet+1;i++ ){
                           if (Succ[i][j]==1) {
                              _fout<< " + Flot("<<i<<","<<j<<","<<p<<","<<t<<")";
                           }
                     }
                     _fout<< " + Stock("<<j<<","<<p<<","<<t-1<<")";
                     for ( int k=0;k<=NbSommet+1;k++ ){
                           if (Succ[j][k]==1) {
                               _fout<< " - Flot("<<j<<","<<k<<","<<p<<","<<t<<")";
                           }
                     }
                     _fout<< " - Stock("<<j<<","<<p<<","<<t<<")";
                     _fout<<" = 0 " << endl;
                 }
            }
       }
   }

   // contrainte 4 :
	  for (int j=0;j<=NbSommet+1;j++){
           for (int p=1;p<=NbProduit;p++){
                if( StockageMax[j]>0 && j>=1 && j<=NbSommet){
                    _fout << "C4("<<j<<","<<p<<"): ";
                    for ( int i=0;i<=NbSommet+1;i++ ){
                          for (int t=1;t<=NbPeriode;t++){
                               if (Succ[i][j]==1) {
                                   _fout<< " + Flot("<<i<<","<<j<<","<<p<<","<<t<<")";
                               }
                          }
                    }
                    for ( int k=0;k<=NbSommet+1;k++ ){
                          for (int t=1;t<=NbPeriode;t++){
                               if (Succ[j][k]==1) {
                                   _fout<< " - Flot("<<j<<","<<k<<","<<p<<","<<t<<")";
                               }
                          }
                    }
                    _fout<<" = 0" << endl;
                 }
            }
       }

// contrainte 5 :

   for (int j=0;j<=NbSommet+1;j++){
        for (int t=1;t<=NbPeriode;t++){
             if( StockageMax[j]>0 && j>=1 && j<=NbSommet){
                 _fout << "C5("<<j<<","<<t<<"): ";
                 for ( int p=1;p<=NbProduit;p++ ){
                       for (int i=0;i<=NbSommet+1;i++){
                            if (Succ[i][j]==1) {
                                _fout<<" + Flot("<<i<<","<<j<<","<<p<<","<<t<<")";
                            }
                       }
                 }
                 _fout<< " - "<< StockageMax[j]<<"F("<<j<<") " ;
                 _fout<<" <= 0" << endl;
            }
        }
    }

    // contrainte 6 :
    for (int j=0;j<=NbSommet+1;j++){
         for (int p=1;p<=NbProduit;p++){
              for (int t=1;t<=NbPeriode;t++){
                   if( StockageMax[j]>0 && j>=1 && j<=NbSommet){
                       _fout << "C6("<<j<<","<<p<<","<<t<<"): ";
                       for (int i=0;i<=NbSommet+1;i++){
                            if (Succ[i][j]==1) {
                                _fout<<" + Flot("<<i<<","<<j<<","<<p<<","<<t<<")";
                            }
                       }
                       _fout<< " - "<< StockageMax[j]<<"G("<<j<<","<<p<<","<<t<<") " ;
                       _fout<<" <= 0" << endl;
                   }
              }
        }
    }

// contrainte 7
   for( int p=1;p<=NbProduit;p++){
        for (int t=1;t<=NbPeriode;t++){
             _fout << "C7("<<p<<","<<t<<"): ";
             for (int j=0;j<=NbSommet+1;j++){
                  if( StockageMax[j]>0 && j>=1 && j<=NbSommet){
                      _fout << " + G("<<j<<","<<p<<","<<t<<")";
                  }
             }
             _fout << " <= " << DispersionPrdtHangar << endl;
        }
  }


// contrainte 8 :

   for (int p = 1;p <= NbProduit;p++){
	 	for (int q=1;q<=NbQuai ;q++){
             for (int t=1;t<=NbPeriode;t++){
                  _fout << "C8("<<p<<","<<q<<","<<t<<"): ";
                  for (int i=0;i<=NbSommet+1;i++){
                       if (Succ[i][QuaiLvr[q]]==1) {
                           _fout<< " + Flot("<<i<<","<<QuaiLvr[q]<<","<<p<<","<<t<<")";
                       }
                  }
                  for (int s=1;s<=NbPeriode;s++){
                       if( s>=t){
                           _fout<< " - K("<<p<<","<<q<<","<<t<<","<<s<<")";
                       }
                  }
                  _fout<<" = 0" << endl;
              }
          }
    }
*/
// contrainte 9
/*
    for (int p=1;p<=NbProduit;p++){
         for (int q=1;q<=NbQuai+1;q++){
              for (int t=1;t<=NbPeriode;t++){
                   _fout << "C9("<<p<<","<<q<<","<<t<<"): " ;
                   _fout<< " + DemandeQuai("<<p<<","<<q<<","<<t<<")";
                   _fout <<" = " << (DemandeQ[p][t]) << endl;
                   _fout<<" = 0" << endl;
              }
         }
	 }




// contrainte 10


for (int p=1;p<=NbProduit;p++){
        for (int q=1;q<=NbQuai+1;q++){
            for (int t=1;t<=NbPeriode;t++){
                //                     _fout << "C10("<<p<<","<<q<<","<<t<<"): " ;

                            for (int s=1;s<=NbPeriode;s++){
                                         if(s>=DebCharg && s<=t){

                    //           _fout<< " + K("<<p<<","<<q<<","<<t<<","<<s<<")";
                                         }
                            }

       //     _fout <<" - " << " DemandeQuai("<<p<<","<<q<<","<<t<<") TSD("<<p<<","<<q<<","<<t<<") = 0" << endl;
           // _fout <<" - " << DemandeQ[p][t]<< " TSD("<<p<<","<<q<<","<<t<<") = 0" << endl;
                      }
                   }
		      }

// contrainte 11


 for (int j=0;j<=NbSommet+1;j++) {
		for (int p=1; p<=NbProduit;p++){
             for (int t=1;t<=NbPeriode;t++){
                  if (Succ[0][j]==1) {


                //         _fout << "C11("<<j<<","<<p<<","<<t<<"): " ;

                //        _fout << " Flot("<<0<<","<<j<<","<<p<<","<<t<<")";

                //        _fout<<"<=" << Capabilite[p][j]*24*CapaMaxHeure[0][j] << endl;
                  }
        }
    }
}


// contrainte 12 :

   _fout << "C12: " ;
 for (int j=0;j<=NbSommet+1;j++){
       for (int p=1;p<=NbProduit;p++){
            for ( int t=1;t<=NbPeriode;t++ ){
                     if (Succ[0][j]==1) {

                    //           _fout<< " - "<< Tot <<" Flot ("<<0<<","<<j<<","<<p<<","<<t<<")";

            }
        }
    }
 }
//  _fout <<" + " << " TSDMoy " ;

// _fout<<" = 0" << endl;

// contrainte 13

 for ( int j=1 ; j<=6 ;j++){
    for (int  t=1 ; t<= NbPeriode ; t++){
        if( Succ[0][j]==1) {

      //      _fout << "C13 ("<<j<<","<<t<<") : " ;

            for ( int p=1 ; p<=NbProduit ; p++) {
         //       _fout<< "+" << (1+CoefC[p])<<" Flot("<<0<<","<<j<<","<<p<<","<<t<<") " ;

            }
       //     _fout << "<=" << CapaMaxHeure[0][j]*24 << endl ;
        }
    }
 }

// contrainte 14 :

            for (int j=0;j<=NbSommet+1;j++) {
                 for (int p=1;p<=NbProduit;p++){
                      for (int t=1;t<=NbPeriode-2;t++){
                          if( StockageMax[j]>0 && j>= 1 && j<= NbSommet){
                     //      _fout << "C14("<<j<<","<<p<<","<<t<<"): " ;

                         for (int k=0;k<=NbSommet+1;k++) {
                                 if (Succ[j][k]==1) {

                    //             _fout<< " + Flot("<<j<<","<<k<<","<<p<<","<<t+2<<")";
                         }
                         }

                            for ( int i=0;i<=NbSommet+1;i++ ){
                                  for (int s=1;s<=NbPeriode-2;s++){
                                    if( s <= t && Succ[i][j]==1) {

                                  //          _fout<< " - Flot("<<i<<","<<j<<","<<p<<","<<s<<")";
                                    }
                                  }
                            }

                           for ( int k=0;k<=NbSommet+1;k++ ){
                                  for (int s=1;s<=NbPeriode-2;s++){
                                    if( s <= t+1 && Succ[j][k]==1){
                                //            _fout<< " + Flot("<<j<<","<<k<<","<<p<<","<<s<<")";
                                    }
                                  }
                           }
 // _fout<<"<= 0" << endl;

                          }
                      }
                 }
            }
// contrainte 15 :

            for (int j=0;j<=NbSommet+1;j++) {
                 for (int p=1;p<=NbProduit;p++){
                          if( StockageMax[j]>0 && j>= 1 && j<= NbSommet){
                           _fout << "C15("<<j<<","<<p<<"): " ;

                         for (int k=0;k<=NbSommet+1;k++) {
                                if ( Succ[j][k]==1) {
                                 _fout<< " + Flot("<<j<<","<<k<<","<<p<<","<<1<<")";
                                   _fout<< " + Flot("<<j<<","<<k<<","<<p<<","<<2<<")";
                         }
}
                                _fout<< " - Stock("<<j<<","<<p<<","<<0<<")";

 _fout<<"<= 0" << endl;

                          }
                      }
                 }


// contrainte 16 :

            for (int j=0;j<=NbSommet+1;j++) {
                if( StockageMax[j]>0 && j>= 1 && j<= NbSommet){
                           _fout << "C16("<<j<<"): " ;

                         for (int i=0;i<=NbSommet+1;i++) {
                                 for (int p=1;p<=NbProduit;p++){
                                     if ( Succ[i][j]==1) {

                                 _fout<< " + Flot("<<i<<","<<j<<","<<p<<","<<NbPeriode-1<<")";
                                   _fout<< " + Flot("<<i<<","<<j<<","<<p<<","<<NbPeriode<<")";
                                 }
                           }
                         }


 _fout<<"= 0" << endl;

                          }
                      }

  // contrainte 17 :
   _fout << "C17: " ;


       _fout <<" + " << " TSDMoy "  ;
       _fout <<" >= " <<   TSDcible << endl ;  */



       // _fout <<" + " << " TSDMoy "  ;
      // _fout <<" - " <<   TSDcible  ;
       //_fout<<" >= 0" << endl;



// contrainte 18




 /*       for (int j=0;j<=NbSommet+1;j++){
                     for( int p=1;p<=NbProduit;p++){
                         for (int t=1;t<=NbPeriode;t++){
                                if ( Succ[0][j]==1) {

                         _fout << "C18("<<j<<","<<p<<","<<t<<"): ";

                           _fout << " + X("<<j<<","<<p<<","<<t<<") " ;
                              _fout<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


                    _fout << " <= " << 0 << endl;

                      }
                   }
                }
        }
  // contrainte 19




        for (int j=0;j<=NbSommet+1;j++){
                     for( int p=1;p<=NbProduit;p++){
                         for (int t=1;t<=NbPeriode;t++){
                                if ( Succ[0][j]==1) {

                         _fout << "C19("<<j<<","<<p<<","<<t<<"): ";

                           _fout << " + "<< H <<" X("<<j<<","<<p<<","<<t<<") " ;
                              _fout<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


                    _fout << " >= " << 0 << endl;

                      }
                   }
                }
        }


// contrainte 20

        for (int j=0;j<=NbSommet+1;j++){
                     for( int p=1;p<=NbProduit;p++){
                         for (int t=1;t<=NbPeriode;t++){
                                if ( Succ[0][j]==1) {

                         _fout << "C20("<<j<<","<<p<<","<<t<<"): ";

                           _fout << " + "<<ProdMin<<" X("<<j<<","<<p<<","<<t<<") " ;
                              _fout<<" - Flot("<<0<<","<<j<<","<<p<<","<<t<<")";


                    _fout << " <= " << 0 << endl;

                      }
                   }
                }
        }

// contrainte 21 :


 _fout << "C21 " ;
for (int j=0;j<=NbSommet+1;j++){
            if( StockageMax[j]>0 && j>=1 && j<=NbSommet){


                     _fout<< " + "<< "F("<<j<<") " ;

                        }
    }
                      _fout <<" = " <<   NbHangerMax << endl ;
                        // _fout<<"= 0" << endl;*/



        //Partie des variales bornées

  _fout << "Bounds" << endl;
/*
  for (int p=1;p<=NbProduit;p++) {
        for (int q=1;q<=NbQuai;q++) {
          for (int t=1;t<=NbPeriode;t++) {
                        _fout << "0 <= TSD("<<p<<","<<q<<","<<t<<") <= 1 " << endl;
           }
       }
  }

  for (int j=1;j<=6;j++) {
      for (int p=1;p<=NbProduit;p++) {
              for (int t=1;t<=NbPeriode;t++) {


                        _fout << " 0 <= X("<<j<<","<<p<<","<<t<<")<= 1" << endl;
                  }
              }
        }

        for (int j=0;j<=NbSommet+1;j++) {
        if( StockageMax[j]>0 && j>=1 && j<=NbSommet){

                        _fout << " 0 <= F("<<j<<") <= 1" << endl;
                  }
 }




for (int j=1;j<=NbSommet+1;j++) {
        for (int p=1;p<=NbProduit;p++) {
              for (int t=1;t<=NbPeriode;t++) {
                     if(StockageMax[j]>0 && j>=1 && j<=NbSommet){

                           _fout << " 0 <= G("<<j<<","<<p<<","<<t<<") <= 2 " << endl;
                       }
                 }
        }
}*/
    //Partie des variables entiere

   _fout << "Generals" << endl;

   for (int i=0;i<=NbSommet+1;i++) {
        for (int j=0;j<=NbSommet+1;j++) {
            for ( int p=1;p<=NbFamille;p++) {
               for (int t=1;t<=NbPeriode;t++) {
                    if (Succ[i][j]==1) {

                       _fout << " Flot("<<i<<","<<j<<","<<p<<","<<t<<") "<< endl;
                    }
               }
            }
        }
   }



   for (int i=0;i<=NbSommet+1;i++) {
                 for (int p=1;p<=NbFamille;p++) {
                      for (int t=0;t<=NbPeriode;t++) {
                         if (StockageMax[i]>0 ){
                           _fout << " Stock("<<i<<","<<p<<","<<t<<") " << endl;
                       }
                 }
             }
      }


/*
 for (int p=1;p<=NbProduit;p++) {
        for  (int q=1;q<=NbQuai;q++) {
                      for (int t=1;t<=NbPeriode;t++) {
                                    _fout << " DemandeQuai("<<p<<","<<q<<","<<t<<")" << endl;
                      }
        }
 }
*/

/*
for  (int p=1;p<=NbProduit;p++) {
   for  (int q=1;q<=NbQuai;q++) {
        for (int t=1;t<=NbPeriode;t++) {
             for (int s=1;s<=NbPeriode;s++) {
                       _fout << " K("<<p<<","<<q<<","<<t<<","<<s<<")"<< endl;
                    }
               }
            }
        }*/



 // _fout << " TSDMoy " << endl;






  //Partie des variales binaires
 /*
 _fout << "Binaries" << endl;



 for (int j=0;j<=NbSommet+1;j++) {
        if( StockageMax[j]>0 && j>=1 && j<=NbSommet){

                        _fout << " F("<<j<<")" << endl;
                  }
 }




for (int j=1;j<=6;j++) {
      for (int p=1;p<=NbProduit;p++) {
              for (int t=1;t<=NbPeriode;t++) {


                        _fout << " X("<<j<<","<<p<<","<<t<<")" << endl;
                  }
              }
        }
*/


	_fout << "End" << endl;

	_fout.close();


	cout << endl << "Le fichier LP '" << f_name << "' a ete enregistre." << endl << endl;


}




//Lecture du fichier

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


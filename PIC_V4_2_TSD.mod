
/* Modèle de Base : modèle de PIC
On mazimise le CA - les couts
la contrainte 41  et 42 sont modifiées on remplace le stockMax par H  et on ajoute C33 sur la capacité de stockage
maximize obj: PoidsChiffreA*ChiffreA - (PoidsCoutChaLigne*CoutChaLigne+ PoidsCoutOuverLigne*CoutOuverLigne + PoidsCoutChaHangar*CoutChaHangar + PoidsCoutOuverHangar*CoutOuverHangar + PoidsCoutStokage*CoutStokage);
  */ 

/* définition des indices*/

param NbSommets, integer, >= 0;
param Horizon, integer, >= 0;
param NbFamille, integer, >0;
param H, integer, >= 0;
param ProdMin,integer, >=0;
param TSDcible, >= 0;
param DelaiCharg, integer, >= 0;
param NbHeureParJour, integer := 24 ;
param NbJourParMois, integer := 30 ;

/* définition des ensembles*/

set SetSommet := 0..NbSommets+1;
set SetSommet2 := 1..6;
set SetPeriode := 1..Horizon;
set SetPeriode2 := 0..Horizon;
set SetPeriode3 := 1..Horizon-2;
set SetFamille := 1..NbFamille;
set SetQuai := 1..6;
param NumSommetQuai{q in SetQuai}, integer, >=0;

/* déclaration des tableaux*/

param Demande{f in SetFamille, q in SetQuai, t in SetPeriode}, integer, >= 0;

param capabilite{f in SetFamille, j in SetSommet2}, integer, >=0;
param CapaMaxHeure{i in SetSommet, j in SetSommet}, integer, >= 0;

param StockageMax{i in SetSommet}, integer, >= 0;
param StockIntial{i in SetSommet,f in SetFamille}, integer, >=0;
param CoefConversion{f in SetFamille}, >=0;
param Succ{i in SetSommet, j in SetSommet}, integer, >= 0;
param DispersionFamHangar{f in SetFamille}, integer, > 0;
param CoutProd {f in SetFamille, i in SetSommet2}, >= 0;
param CoutStock {f in SetFamille, i in SetSommet}, integer, >= 0;
param CoutOuvLigne {i in SetSommet2}, integer, >= 0;
param CoutOuvHangare {i in SetSommet}, integer, >= 0;
param PrixVentes {f in SetFamille}, integer, >= 0;
param CoutChangLigneProd, integer, >= 0;
param CoutChangHangar, integer, >= 0;

/* parametres de la fonction objectif*/

param PoidsChiffreA, integer; 
param PoidsCoutChaLigne, integer;
param PoidsCoutChaHangar, integer;
param PoidsCoutOuverLigne, integer;
param PoidsCoutOuverHangar, integer;
param PoidsCoutStokage, integer;

/* déclaration des variables*/

var Flot{i in SetSommet, j in SetSommet, f in SetFamille, t in SetPeriode}, integer, >= 0;
var Stock{i in SetSommet,f in SetFamille, t in SetPeriode2}, integer, >= 0;
var PertCapa{j in SetSommet2 ,f in SetFamille, t in SetPeriode},integer, >= 0;
var TSD{f in SetFamille, t in SetPeriode}, >= 0, <= 1;
var TsdQuai{f in SetFamille,q in SetQuai, t in SetPeriode}, >= 0, <= 1;

var TSDMoy,>= 0;
var X{j in SetSommet2,f in SetFamille, t in SetPeriode}, >= 0, <= 1 , binary;  /* a ajouter dautres var binaires */
var Y{j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode}, >= 0, <= 1 , binary;
var Z{j in SetSommet,f in SetFamille, t in SetPeriode }, >= 0, <= 1 , binary;
var A{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode }, >= 0, <= 1 , binary;
var K{f in SetFamille, q in SetQuai, s in SetPeriode , u in SetPeriode }, integer, >= 0;
var F{j in SetSommet}, >= 0, <= 1 , binary;
var G{j in SetSommet,f in SetFamille, t in SetPeriode}, >= 0, <= 2 , integer; 
var U{j in SetSommet2}, >= 0, <= 1 , binary;
var ChiffreA, integer;
var CoutChaLigne, integer;
var CoutChaHangar, integer;
var CoutOuverLigne, integer;
var CoutOuverHangar, integer;
var CoutStokage, integer;


/* définition des contraintes*/

/* 0. contrainte de stock intial : hypothése stock initial nul*/

s.t. C0{i in SetSommet,f in SetFamille: StockageMax[i]>0}: Stock[i,f,0] = 0 ;

/* 1. contrainte de de capacité de l'arc ,: le flot qui passe dans l'arc est inférieur à la capacité mensuelle de l'arc  */

 s.t. C1{i in SetSommet, j in SetSommet, t in SetPeriode : Succ[i,j] = 1 }: sum{f in SetFamille} Flot[i,j,f,t] <= CapaMaxHeure[i,j]*NbHeureParJour*NbJourParMois;

/* 2. contrainte de conservation de flot : le flot d'entrée à un noeud de transit est égal au flot de sortie  */

s.t. C2{j in SetSommet,f in SetFamille, t in SetPeriode : StockageMax[j] = 0 and j >= 1 and j <= NbSommets}: sum{i in SetSommet : Succ[i,j] = 1} Flot[i,j,f,t] = sum{k in SetSommet : Succ[j,k] = 1} Flot[j,k,f,t] ;

/* 3. contrainte de conservation de stock  : le flot d'entrée  à t à un noeud de  stockage plus le stock à t-1 est égale au  flot de sortie à t plus le stock à t */

s.t. C31{j in SetSommet, f in SetFamille, t in SetPeriode : StockageMax[j] > 0 and j >= 1 and j <= NbSommets}:  Stock[j,f,t-1] + sum{i in SetSommet : Succ[i,j] = 1} Flot[i,j,f,t] = sum{k in SetSommet : Succ[j,k] = 1} Flot[j,k,f,t] + Stock[j,f,t];

s.t. C32{j in SetSommet, f in SetFamille : StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: sum{i in SetSommet, t in SetPeriode : Succ[i,j] = 1} Flot[i,j,f,t] = sum{k in SetSommet, t in SetPeriode : Succ[j,k] = 1} Flot[j,k,f,t] ;

/* Contrainet C33 sur la capacité de stockage */

s.t. C33 {i in SetSommet, t in SetPeriode : StockageMax[i]>0 and i >= 1 and i <= NbSommets}: sum{f in SetFamille} Stock[i,f,t] <= StockageMax[i] ;
 
/* 41. et 42. contrainte de  capacité de stockage : le stock à la période t ne doit pas dépasser la capacité de stockge du hangar*/

s.t. C41{j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: sum{f in SetFamille, i in SetSommet, t in SetPeriode : Succ[i,j] = 1} Flot[i,j,f,t] <= H*F[j] ;
s.t. C42{j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: sum{f in SetFamille, i in SetSommet, t in SetPeriode : Succ[i,j] = 1} Flot[i,j,f,t] >= F[j] ;


/* 43 et 44. contrainte de  capacité de stockage : le produit p est dispersé dans plusieurs hangars */

s.t. C43{j in SetSommet,f in SetFamille, t in SetPeriode: StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: sum{i in SetSommet : Succ[i,j] = 1} Flot[i,j,f,t] <= StockageMax[j]*G[j,f,t] ;

s.t. C44{f in SetFamille, t in SetPeriode}: sum{j in SetSommet : StockageMax[j] > 0 and j >= 1 and j <= NbSommets} G[j,f,t] <= DispersionFamHangar[f] ;

/* 5.  contrainte de  demande : la somme des flot arrivant au puits est égale à la demande * le Taux de satisfaction de la demande*/

s.t. C51 {f in SetFamille,q in SetQuai, t in SetPeriode}: sum{i in SetSommet : Succ[i,NumSommetQuai[q]] = 1} Flot[i,NumSommetQuai[q],f,t] = TsdQuai[f,q,t]*Demande[f,q,t]  ; 

s.t. C53{f in SetFamille,q in SetQuai, t in SetPeriode}: TsdQuai[f,q,t] >= 1 - Demande[f,q,t] ;

/* 6. contrainte de capabilité des lignes de production :  les produtions sur les lignes doivenet respecter la capabilité des lignes à produire ou non une famille de produit*/

 s.t. C6{j in SetSommet2,f in SetFamille, t in SetPeriode : Succ[0,j] = 1}: Flot[0,j,f,t]<= capabilite[f,j]*CapaMaxHeure[0,j]*NbHeureParJour*NbJourParMois; 

/* 7. contrainte de de calcul de TSD moyen : le TSD moyen est égal à la sommes des produtions sur la sommes des demandes */

s.t. C71{f in SetFamille}:  sum {j in SetSommet2, t in SetPeriode : Succ[0,j] = 1} Flot[0,j,f,t] <= sum {q in SetQuai, t in SetPeriode } Demande[f,q,t] ; 

s.t. C72: TSDMoy =  (sum {f in SetFamille, q in SetQuai, t in SetPeriode } TsdQuai[f,q,t])/(NbFamille*6*Horizon) ; 

/* 8 : contrainte perte de capacité :  de de calcul de perte de capacité de production : est la sommes du produit des coeffi de coversion et la capacité de production des lignes , de toutes les familles produits sur une ligne donnée  + contrainte d'actualisation de la capacité maximale de l'arc : la nouvelle capacité de l'arc est egalé à la capacité maximale de l'arc mois la perte de capacité de la ligne  */

s.t. C8{j in SetSommet2, t in SetPeriode : Succ[0,j] = 1}: sum {f in SetFamille} (1+CoefConversion[f])*Flot[0,j,f,t]<= CapaMaxHeure[0,j]*NbHeureParJour*NbJourParMois; 

/* 9 : utilisation ou non des hangars sur tout l'horizon*/

/*s.t. C91{j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: F[j] <= sum {t in SetPeriode, f in SetFamille} G[j,f,t];
s.t. C92{j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets}: H*F[j] >= sum {t in SetPeriode, f in SetFamille} G[j,f,t];*/

/* 9.   TSD cible à respecter */

/*C9: TSDMoy >= TSDcible ;*/

/* 10.   Variable Binaire X signifiant l'utilisant d'une ligne j par une famille f à une période t */

s.t. C101 {j in SetSommet2,f in SetFamille, t in SetPeriode : Succ[0,j] = 1}: X[j,f,t] <= Flot[0,j,f,t] ;

s.t. C102 {j in SetSommet2,f in SetFamille, t in SetPeriode : Succ[0,j] = 1}: H*X[j,f,t] >= Flot[0,j,f,t] ;

s.t. C103{j in SetSommet2,f in SetFamille, t in SetPeriode : Succ[0,j] = 1}: Flot[0,j,f,t]<= X[j,f,t]*CapaMaxHeure[0,j]*NbHeureParJour*NbJourParMois; 

/* 11.   Contrainte de non changement de ligne de production j sur l'horizon pour une famille f  */

s.t. C111{j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 }: sum{s in SetPeriode : s >= t+1} X[j2,f,s] - H*(1-X[j1,f,t]) <= Y[j1,j2,f,t]*H ; 
s.t. C112{j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 }: Y[j1,j2,f,t] <= X[j1,f,t] ; 
s.t. C113{j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 }: Y[j1,j2,f,t] <= sum{s in SetPeriode : s >= t+1} X[j2,f,s]; 


/* 12.   Contrainte de non changement d'hangar j sur l'horizon pour une famille f */

s.t. C121{j in SetSommet, f in SetFamille, t in SetPeriode : StockageMax[j] > 0 and j >= 1 and j <= NbSommets}:  H*Z[j,f,t] >= sum{i in SetSommet:Succ[i,j] = 1} Flot[i,j,f,t] ;

s.t. C122{j in SetSommet, f in SetFamille, t in SetPeriode : StockageMax[j] > 0 and j >= 1 and j <= NbSommets}:  Z[j,f,t] <= sum{i in SetSommet:Succ[i,j] = 1} Flot[i,j,f,t] ;


s.t. C123{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 and StockageMax[j1] > 0 and j1 >= 1 and j1 <= NbSommets and StockageMax[j2] > 0 and j2 >= 1 and j2 <= NbSommets}: sum{s in SetPeriode : s >= t+1} Z[j2,f,s] - H*(1-Z[j1,f,t]) <= A[j1,j2,f,t]*H ; 

s.t. C124{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 and StockageMax[j1] > 0 and j1 >= 1 and j1 <= NbSommets and StockageMax[j2] > 0 and j2 >= 1 and j2 <= NbSommets}: A[j1,j2,f,t] <= Z[j1,f,t] ; 

s.t. C125{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 and StockageMax[j1] > 0 and j1 >= 1 and j1 <= NbSommets and StockageMax[j2] > 0 and j2 >= 1 and j2 <= NbSommets}: A[j1,j2,f,t] <= sum{s in SetPeriode : s >= t+1} Z[j2,f,s];

/*variable ouverture de ligne de production sur tout l'horizon de planification*/

s.t. C131 {j in SetSommet2 : Succ[0,j] = 1}: U[j] <= sum{f in SetFamille, t in SetPeriode} X[j,f,t] ;

s.t. C132 {j in SetSommet2 : Succ[0,j] = 1}: sum{f in SetFamille, t in SetPeriode} X[j,f,t] <= H*U[j] ;

/*Calcule des couts*/
s.t. C141 : CoutChaLigne = sum {j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode : j1 <> j2} CoutChangLigneProd*Y[j1,j2,f,t];
s.t. C142 : CoutChaHangar = sum{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 and StockageMax[j1] > 0 and j1 >= 1 and j1 <= NbSommets and StockageMax[j2] > 0 and j2 >= 1 and j2 <= NbSommets} CoutChangHangar*A[j1,j2,f,t];
s.t. C143 : CoutOuverLigne = sum {j in SetSommet2} CoutOuvLigne[j]*U[j];
s.t. C144 : CoutOuverHangar = sum {j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets} CoutOuvHangare[j]*F[j];
s.t. C145 : CoutStokage = sum{j in SetSommet, f in SetFamille, t in SetPeriode } CoutStock[f,j]*Stock[j,f,t];
/*s.t. C146 : ChiffreA = sum{f in SetFamille, q in SetQuai, t in SetPeriode} PrixVentes[f]*Demande[f,q,t]*TsdQuai[f,q,t] ;*/


/* la fonction objectif  : maximiser le profit en minimisdant les couts*/
/*maximize obj: PoidsChiffreA*ChiffreA - (PoidsCoutChaLigne*CoutChaLigne+ PoidsCoutOuverLigne*CoutOuverLigne + PoidsCoutChaHangar*CoutChaHangar + PoidsCoutOuverHangar*CoutOuverHangar + PoidsCoutStokage*CoutStokage);-*/


/* maximize obj: sum{f in SetFamille, q in SetQuai, t in SetPeriode} PrixVentes[f]*Demande[f,q,t]*TsdQuai[f,q,t] - (sum {j in SetSommet2,} CoutOuvLigne[j]*U[j] + sum{j in SetSommet2, f in SetFamille, t in SetPeriode : Succ[0,j] = 1 } CoutProd[f,j]*Flot[0,j,f,t] + sum{j in SetSommet, f in SetFamille, t in SetPeriode } CoutStock[f,j]*Stock[j,f,t] + sum {j in SetSommet: StockageMax[j] > 0 and j >= 1 and j <= NbSommets} CoutOuvHangare[j]*F[j] + sum{j1 in SetSommet2,j2 in SetSommet2, f in SetFamille, t in SetPeriode : j1 <> j2} CoutChangLigneProd*Y[j1,j2,f,t] + sum{j1 in SetSommet,j2 in SetSommet, f in SetFamille, t in SetPeriode : j1 <> j2 and t <= Horizon-1 and StockageMax[j1] > 0 and j1 >= 1 and j1 <= NbSommets and StockageMax[j2] > 0 and j2 >= 1 and j2 <= NbSommets} CoutChangHangar*A[j1,j2,f,t]);*/


maximize obj: (sum {f in SetFamille, q in SetQuai, t in SetPeriode } TsdQuai[f,q,t]); 



/* affichage des resultats*/

solve;
printf "----------------------------------------------------------------- \n";
printf " TSDMoyen = %3d\n",TSDMoy;
printf "----------------------------------------------------------------- \n";
printf{ t in SetPeriode , f in SetFamille, i in SetSommet, j in SetSommet: Flot[i,j,f,t] > 0} " Période = %3d  Famille = %3d  orig =%3d  dest =%3d Quantite = %3d  \n",t, f,i, j, Flot[i,j,f,t];
printf "----------------------------------------------------------------- \n";
printf{t in SetPeriode2 , i in SetSommet, f in SetFamille : Stock[i,f,t] > 0 } "periode = %3d  SommetStockage = %3d  Famille = %3d   Stock = %3d \n", t,i,f,Stock[i,f,t];
printf "----------------------------------------------------------------- \n";

/* données */

data;

param NbSommets := 46;
param NbFamille := 6 ;
param Horizon := 12;
param H := 9999999;
param ProdMin:= 720 ;
param TSDcible := 0; 
param DelaiCharg := 0;

/* valeur des parametres de la fonction objectif*/ 
param PoidsChiffreA:=1; 
param PoidsCoutChaLigne:=0;
param PoidsCoutChaHangar:=0;
param PoidsCoutOuverLigne:=0;
param PoidsCoutOuverHangar:=0;
param PoidsCoutStokage:=0;

/*param DispersionFamHangar :=1   2,2   2,3   2,4   2,5   2,6   2;*/
param DispersionFamHangar :=1   6,2   6,3   6,4   6,5   6,6   6;
param CoutChangLigneProd := 1000000;
param CoutChangHangar := 1000000;
param NumSommetQuai :=1   41,2   42,3   43,4   44,5   45,6   46;

param StockageMax := 
		0	0	,
		1	0	,
		2	0	,
		3	0	,
		4	0	,
		5	0	,
		6	0	,
		7	0	,
		8	0	,
		9	0	,
		10	50000	,
		11	50000	,
		12	50000	,
		13	0	,
		14	0	,
		15	0	,
		16	0	, 
		17	0	,
		18	0	,
		19	0	,
		20	0	,
		21	0	,
		22	0	,
		23	0	,
		24	0	,
		25	0	,
		26	0	,
		27	50000	,
		28	50000	,
		29	50000	,
		30	50000	,
		31	100000	,
		32	100000	,
		33	0	,
		34	0	,
		35	0	,
		36	0	,
		37	0	,
		38	0	,
		39	0	,
		40	0	,
		41	0	,
		42	0	,
		43	0	,
		44	0	,
		45	0	,
		46	0	,
		47	0	;

param CapaMaxHeure :	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	:=
0	0	90	90	180	120	120	120	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
1	0	0	0	0	0	0	0	90	90	90	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
2	0	0	0	0	0	0	0	90	90	90	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
3	0	0	0	0	0	0	0	180	180	180	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
4	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	120	120	120	120	120	120	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
5	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	120	120	120	120	120	120	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
6	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	120	120	120	120	120	120	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
7	0	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
8	0	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
9	0	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
10	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
11	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
12	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
13	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
14	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
15	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
16	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
17	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
18	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
19	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
20	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
21	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
22	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
23	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
24	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
25	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
26	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
27	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
28	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
29	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
30	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	0	0	0	0	0	0	
31	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	
32	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	450	0	0	0	0	0	0	0	0	0	0	0	0	
33	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	
34	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	0	
35	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	0	0	
36	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	0	
37	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	0	0	0	0	0	0	0	
38	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	0	0	0	0	0	0	0	
39	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	450	0	450	450	0	
40	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	450	450	450	450	0	
41	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
42	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
43	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
44	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
45	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
46	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	450	
47	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	;





param Demande :=													
[*,1,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	19440	10589	16128	7803	22239	20948	12902	21587	4221	2745	7524	4271	
2	15444	9257	8604	20885	4613	21699	12177	7245	6480	5936	15971	12506	
3	18126	14234	5198	3641	18981	15930	18693	10769	15309	11309	9158	10724	
4	8618	10949	12569	15071	11966	17955	22325	20430	15224	878	2669	10539	
5	6719	18896	15741	4910	24062	19211	10827	13856	7862	12654	19040	12600	
6	7709	19841	1247	12285	3267	13217	3299	15111	19697	9743	8613	2394	
													
[*,2,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	22842	11768	22478	1494	23000	16115	4253	7736	22784	15228	24075	6863	
2	22955	20930	17883	158	9725	22662	1418	2048	8856	16713	9990	22950	
3	15575	3807	3060	6147	12564	10013	19850	1076	13010	4424	1566	17402	
4	22635	15984	9212	15723	2633	14958	7484	6287	22712	2853	20205	21299	
5	13118	9266	635	21798	13401	4226	11129	1058	6174	13730	23031	18734	
6	16425	531	21065	10085	8177	9522	16331	7943	12920	9783	4320	9923	
													
[*,3,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	13572	22442	4482	10629	19809	15390	21569	12254	17100	24296	4253	2340	
2	9167	24111	20313	108	10292	2664	3672	16650	21987	14319	10098	10314	
3	360	12695	14814	23175	104	6674	21101	18860	20057	11003	11318	14189	
4	4266	9315	12002	5801	15552	17235	14841	13370	9878	14778	2570	11633	
5	1872	19998	19310	12411	13401	7002	16434	6404	23342	2475	15291	7893	
6	16848	22550	23814	2318	6242	15764	1269	16128	11025	12695	8330	12587	
													
[*,4,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	2111	17330	23378	13347	10764	9212	19251	8361	2754	8411	23747	11916	
2	527	8685	180	6998	17393	21398	8334	18504	23891	16493	7241	8676	
3	9531	13334	24111	3794	17780	243	4082	17451	21843	17091	20192	302	
4	10292	1868	10085	19575	4176	5067	22059	12789	22406	4356	22064	12263	
5	14324	14931	18882	15012	16371	19292	13199	21515	13109	22509	914	23198	
6	10413	3456	22986	15921	12411	15215	2790	10472	21960	21024	11817	11606	
													
[*,5,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	10260	8033	17537	18455	8618	19404	21461	17361	3861	23886	21816	7650	
2	20516	14468	19836	11228	21326	12380	4122	13028	15984	9936	4640	17474	
3	18554	16677	16200	9621	6332	3213	8469	22937	5823	12677	17807	12506	
4	13964	392	21317	13982	5364	21339	6503	16029	2664	9401	19877	13820	
5	1265	9446	10391	14553	945	17037	17280	14508	1593	20772	11295	11912	
6	14981	554	21474	13383	3722	11669	17708	9576	13523	13986	15458	17856	
													
[*,6,*] :													
	1	2	3	4	5	6	7	8	9	10	11	12	:=
1	19629	12816	4640	11700	22145	10850	21929	18801	5081	585	23391	10148	
2	5571	6017	8897	7313	13149	15858	17811	13347	5666	16623	23441	16916	
3	4248	5643	4361	2948	4559	16907	20988	9923	5837	22014	20345	14468	
4	14819	3888	8339	4541	18194	833	2336	14112	10683	12348	9500	6399	
5	4320	6404	3011	5747	1827	14877	13496	1737	6975	15998	20417	3825	
6	9801	13910	3870	20664	12978	17222	15696	6507	18014	15354	17537	17586	;






param capabilite :	1	2	3	4	5	6	:=
				1	1	1	1	1	1	1	
				2	1	1	1	1	1	1	
				3	1	1	1	1	1	1	
				4	1	1	1	1	1	1	
				5	0	0	0	1	1	1	
				6	1	1	1	0	0	0	;

param CoefConversion:= 	1	0	,
						2	0	,
						3	0.25,
						4	0.1	,
						5	0.2	,
						6	0.55;


param PrixVentes := 			
	1	500	,
	2	550	,
	3	500	,
	4	600	,
	5	500	,
	6	450	;


param CoutOuvLigne := 			
	1	10000	,
	2	10000	,
	3	20000	,
	4	15000	,
	5	15000	,
	6	15000	;


param CoutOuvHangare  := 			
	0	0	,
	1	0	,
	2	0	,
	3	0	,
	4	0	,
	5	0	,
	6	0	,
	7	0	,
	8	0	,
	9	0	,
	10	5000	,
	11	5000	,
	12	5000	,
	13	0	,
	14	0	,
	15	0	,
	16	0	, 
	17	0	,
	18	0	,
	19	0	,
	20	0	,
	21	0	,
	22	0	,
	23	0	,
	24	0	,
	25	0	,
	26	0	,
	27	7500	,
	28	7500	,
	29	7500	,
	30	7500	,
	31	10000	,
	32	10000	,
	33	0	,
	34	0	,
	35	0	,
	36	0	,
	37	0	,
	38	0	,
	39	0	,
	40	0	,
	41	0	,
	42	0	,
	43	0	,
	44	0	,
	45	0	,
	46	0	,
	47	0	;

param CoutStock :		0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	:=
	1	0	0	0	0	0	0	0	0	0	0	0	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	8	8	8	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
	2	0	0	0	0	0	0	0	0	0	0	0	9	9	9	0	0	0	0	0	0	0	0	0	0	0	0	0	0	9	9	9	9	9	9	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
	3	0	0	0	0	0	0	0	0	0	0	0	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	8	8	8	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
	4	0	0	0	0	0	0	0	0	0	0	0	10	10	10	0	0	0	0	0	0	0	0	0	0	0	0	0	0	10	10	10	10	10	10	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
	5	0	0	0	0	0	0	0	0	0	0	0	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	8	8	8	8	8	8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
	6	0	0	0	0	0	0	0	0	0	0	0	7	7	7	0	0	0	0	0	0	0	0	0	0	0	0	0	0	7	7	7	7	7	7	0	0	0	0	0	0	0	0	0	0	0	0	0	0	;


param CoutProd :		1	2	3	4	5	6	:=
	1	50	55	60	55	55	55	
	2	50	55	60	55	55	55	
	3	50	55	60	55	55	55	
	4	50	55	60	55	55	55	
	5	50	55	60	55	55	55	
	6	50	55	60	55	55	55	;

param Succ :	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	:=
0	0	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
1	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
2	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
3	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
4	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
5	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
6	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
7	0	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
8	0	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
9	0	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
10	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
11	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
12	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
13	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
14	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
15	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
16	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
17	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
18	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
19	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
20	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
21	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
22	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
23	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
24	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
25	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
26	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
27	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
28	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
29	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
30	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	
31	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	
32	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	
33	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	
34	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	
35	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	
36	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	
37	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	
38	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	0	0	0	0	0	0	0	
39	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	0	1	1	0	
40	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	1	1	0	
41	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
42	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
43	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
44	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
45	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
46	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	
47	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	;





/*param NbArche := 
		0	0	,
		1	0	,
		2	0	,
		3	0	,
		4	0	,
		5	0	,
		6	0	,
		7	0	,
		8	0	,
		9	0	,
		10	12	,
		11	12	,
		12	12	,
		13	0	,
		14	0	,
		15	0	,
		16	0	,
		17	0	,
		18	0	,
		19	0	,
		20	0	,
		21	0	,
		22	0	,
		23	0	,
		24	0	,
		25	0	,
		26	0	,
		27	12	,
		28	12	,
		29	12	,
		30	12	,
		31	24	,
		32	24	,
		33	0	,
		34	0	,
		35	0	,
		36	0	,
		37	0	,
		38	0	;
		
param CapaArcheMax:
	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24:=
0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
4	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
5	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
6	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
7	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
8	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
9	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
10	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
11	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
12	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
13	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
14	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
15	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
16	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
17	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
18	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
19	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
20	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
21	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
22	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
23	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
24	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
25	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
26	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
27	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
28	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
29	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
30	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	0	0	0	0	0	0	0	0	0	0	0	0
31	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166
32	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166	4166
33	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
34	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
35	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
36	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
37	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
38	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
 ;
*/











/*param NumFamille :=	1	1	,
					2	1	,
					3	1	,
					4	1	,
					5	2	,
					6	2	,
					7	2	,
					8	2	,
					9	2	,
					10	2	,
					11	2	,
					12	2	,
					13	3	,
					14	3	,
					15	3	,
					16	3	,
					17	3	,
					18	4	,
					19	4	,
					20	4	,
					21	4	,
					22	5	,
					23	5	,
					24	5	,
					25	5	,
					26	5	,
					27	5	,
					28	5	,
					29	5	,
					30	5	,
					31	5	,
					32	5	,
					33	6	;   */
                                                                




/*param StockInitial :	1	2	3	4	5	6	:=
					0	0	0	0	0	0	0	
					1	0	0	0	0	0	0	
					2	0	0	0	0	0	0	
					3	0	0	0	0	0	0	
					4	0	0	0	0	0	0	
					5	0	0	0	0	0	0	
					6	0	0	0	0	0	0	
					7	0	0	0	0	0	0	
					8	0	0	0	0	0	0	
					9	0	0	0	0	0	0	
					10	0	0	0	0	0	0	
					11	0	0	0	0	0	0	
					12	0	0	0	0	0	0	
					13	0	0	0	0	0	0	
					14	0	0	0	0	0	0	
					15	0	0	0	0	0	0	
					16	0	0	0	0	0	0	
					17	0	0	0	0	0	0	
					18	0	0	0	0	0	0	
					19	0	0	0	0	0	0	
					20	0	0	0	0	0	0	
					21	0	0	0	0	0	0	
					22	0	0	0	0	0	0	
					23	0	0	0	0	0	0	
					24	0	0	0	0	0	0	
					25	0	0	0	0	0	0	
					26	0	0	0	0	0	0	
					27	0	0	0	0	0	0	
					28	0	0	0	0	0	0	
					29	0	0	0	0	0	0	
					30	0	0	0	0	0	0	
					31	0	0	0	0	0	0	
					32	0	0	0	0	0	0	
					33	0	0	0	0	0	0	
					34	0	0	0	0	0	0	
					35	0	0	0	0	0	0	
					36	0	0	0	0	0	0	
					37	0	0	0	0	0	0	
					38	0	0	0	0	0	0	;*/


end;
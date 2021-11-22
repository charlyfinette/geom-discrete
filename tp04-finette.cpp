/*
        FINETTE Charly - version du 18/10/2021  
*/


#include "astico2d.hpp"
#include <set>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

// ----------------------Définition de classes----------------------------------

class ContourF8 
{
    private:

    int m_x;
    int m_y;
    vector<int> m_chaineFreeman;
    int m_dirInitiale;

    public:

    ContourF8(int x, int y, int dir) 
    {
        m_x = x;
        m_y = y;
        m_dirInitiale = dir;
    }

    vector<int> getChaineFreeman()
    {
        return m_chaineFreeman;
    }

    int getDirInitiale()
    {
        return m_dirInitiale;
    }

    int getStart_x()
    {
        return m_x;
    }

    int getStart_y()
    {
        return m_y;
    }

    void add(int dir)
    {
        m_chaineFreeman.push_back(dir);
    }

    int size()
    {
        return m_chaineFreeman.size();
    }   

    void print()
    {
        if(m_chaineFreeman.size()>0)
        {
            cout << "Point de départ : (" << m_x << "," << m_y <<")" << endl;
            cout << "Chaîne de Freeman : ";
            cout << "[" << m_chaineFreeman[0] << ",";
            for(unsigned int i = 1; i < m_chaineFreeman.size()-1; i++)
                cout << m_chaineFreeman[i] << ",";
            cout << m_chaineFreeman[m_chaineFreeman.size()-1] << "]" << endl;
            cout << endl;
        }
        
    }
  
};


class ContourPol
{
    private:
    vector<int> m_cont_x;
    vector<int> m_cont_y;
    vector<bool> m_cont_f;

    public:
    ContourPol(vector<int> cont_x, vector<int> cont_y, vector<bool> cont_f)
    {
        m_cont_x = cont_x;
        m_cont_y = cont_y;
        m_cont_f = cont_f;
    }

    vector<int> getcont_x()
    {
        return m_cont_x;
    }

    vector<int> getcont_y()
    {
        return m_cont_y;
    }

    vector<bool> getcont_f()
    {
        return m_cont_f;
    }

    unsigned int size()
    {
        return m_cont_f.size();
    }

    void print()
    {
        cout << " X : [" << m_cont_x[0];
        for(unsigned int i = 1; i < m_cont_f.size()-1; i++)
        {
            cout << "," << m_cont_x[i];
        }
        cout << "," <<  m_cont_x[m_cont_f.size()-1] << "]" << endl;

        cout << " Y : [" << m_cont_y[0];
        for(unsigned int i = 1; i < m_cont_f.size()-1; i++)
        {
            cout << "," << m_cont_y[i];
        }
        cout << "," << m_cont_y[m_cont_f.size()-1] << "]" << endl;

        cout << " F : [" << m_cont_f[0];
        for(unsigned int i = 1; i < m_cont_f.size()-1; i++)
        {
            cout << "," << m_cont_f[i];
        }
        cout << "," << m_cont_f[m_cont_f.size()-1] << "]" << endl;
    }

};

// -------------------- Fonctions auxiliaires--------------------------------

// Vérifie si un pixel fait partie du contour de la forme pour
// la 8-connexité avec le fond
bool est_un_contours_c8_fond (cv::Mat &img_niv, int y, int x)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);
    bool res = false;
    int g = img_niv.at<int>(y,x);

        if (g > 0) // si g est un pixel de l'objet
        { 
            if (y == 0  // Et si g est au bord de l'image 
             || x == 0 
             || y == img_niv.rows - 1 
             || x == img_niv.cols - 1)
            res = true; // alors g fait parti du contour de l'image

        //  On teste si un de ses 4-voisins appartient au fond 
            else if (img_niv.at<int>(y-1,x) == 0
                || img_niv.at<int>(y+1,x) == 0
                || img_niv.at<int>(y,x-1) == 0
                || img_niv.at<int>(y,x+1) == 0)
                res = true;    
        }

    return res;
}

// affiche le contenu d'un std::set<int>
void print(const set<int>& ens) 
{   
    cout << "{";

    for (auto it = ens.begin(); it != ens.end(); ++it)
        cout << "," << *it;
    
    cout << "}" << endl;
}

// Une classe d'equivalence sort de la partie 1.3 de l'algo
// Si elle a un élement en commun avec une classe déjà existante
// La fonction renvoie l'indice de cette classe
// Sinon elle renvoie -1
int indiceClasseEq(const vector<set<int>>& listeClasses, const set<int>& ens_cur)
{
    for(unsigned int i = 0; i < listeClasses.size(); i++)
    {
        for( auto it = listeClasses[i].begin(); it != listeClasses[i].end(); it++)
        {
            if(ens_cur.count(*it)==1)
                return i;
        }     
    }

    return -1;
}

// ajoute une nouvelle classe d'équivalence
// ou complète une classe déja existante
void MajClasseEq(vector<set<int>>& listeClasses, const set<int>& ens_cur)
{
    if(indiceClasseEq(listeClasses, ens_cur) == -1)
        listeClasses.push_back(ens_cur);
    else
    {
        for( auto it = ens_cur.begin(); it != ens_cur.end(); it++)
        listeClasses[indiceClasseEq(listeClasses,ens_cur)].insert(*it);
    }        
}

ContourF8 suivre_un_contour_c8(cv::Mat &img_niv, int yA, int xA, int  dirA, int num_contour)
{
    ContourF8 contour_curr(xA, yA, dirA);
    vector<int> nx8 = { 1, 1, 0, -1, -1, -1, 0, 1};
    vector<int> ny8 = { 0, 1, 1, 1, 0, -1, -1, -1};
    int dir_finale = -1;

// on parcout les directions jusqu'à ce qu'on trouve un pixel dans la forme
    for(int i = 0; i < 8 ; i++) 
    {
        int d = (dirA + i) % 8;
        int xV = xA + nx8[d];
        int yV = yA + ny8[d];
        
        if( xV >= 0 && yV >= 0 && xV < img_niv.cols && yV < img_niv.rows // Si le voisin appartient à l'image
            && img_niv.at<int>(yV,xV) > 0) // est qu'il fait partie de la forme
            {
                dir_finale =  (d+4)%8;
                break;
            }
    }

    int x = xA;
    int y = yA;
    int dir = dir_finale;

    do 
    {
        img_niv.at<int>(y,x) = num_contour;
        dir = (dir + 4 - 1) % 8;

        int i = 0;
        while(i < 8)
        {
            int d = (dir + 8 - i) % 8;
            int xV = x + nx8[d];
            int yV = y + ny8[d];

            if( xV >= 0 && yV >= 0 && xV < img_niv.cols && yV < img_niv.rows // Si le voisin appartient à l'image
            && img_niv.at<int>(yV,xV) > 0) // est qu'il fait partie de la forme
            {
                x = xV;
                y = yV;
                dir = d;
                contour_curr.add(d); // emplacement pour mémoriser la chaîne de Freeman
                break;
            }

            i++;
        }

        if (i == 8)
            {
               cout << " Point isolé !" << endl; // faut faire un truc ici?
               break;
            } 
    }
    while(!(x == xA && y == yA && dir == dir_finale));

    // contour_curr.print();
    return contour_curr;

}

int next_x(const int & x_curr,const int & dir)
{
    vector<int> nx8 = { 1, 1, 0, -1, -1, -1, 0, 1};
    int next = x_curr + nx8[dir];
    return next;
}

int next_y(const int & y_curr,const int & dir)
{
    vector<int> ny8 = { 0, 1, 1, 1, 0, -1, -1, -1};
    int next = y_curr + ny8[dir];
    return next;
}
    
int normeCarre(int x, int y)
{
    return x*x + y*y;
}

void doNothing(){}

void approximer_contour_recur(const vector<int> & cont_x,
                              const vector<int> & cont_y, 
                              vector<bool> & cont_f, 
                              const float & seuil,
                              const unsigned int & debut, 
                              const unsigned int & fin)
{
    int tailleTroncon = fin - debut + 1;

    if (tailleTroncon < 3)
        return doNothing();
    
    int indicePointDistanceMax = 0;
    float DistanceMax = 0;

    for(unsigned int i = debut + 1; i < fin; i++)
    {   
        int P_x = cont_x[debut];
        int P_y = cont_y[debut];
        int Q_x = cont_x[fin];
        int Q_y = cont_y[fin];
        int R_x = cont_x[i];
        int R_y = cont_y[i];

        float distanceCarre = (Q_x - P_x)*(R_y - P_y) - (Q_y - P_x)*(R_x - P_x);
        distanceCarre = pow(distanceCarre,2);
        distanceCarre /= normeCarre(Q_x - P_x, Q_y - P_y);

        if(distanceCarre > DistanceMax)
        {
            DistanceMax = distanceCarre;
            indicePointDistanceMax = i;
        }     
    }

    cout << "distanceMax :"<< DistanceMax << endl;
    if(DistanceMax < pow(seuil,2))
    {   
        for(unsigned int i = debut + 1; i < fin - 1; i++)
            cont_f[i] = 0; 
    }
    else 
    {
        approximer_contour_recur(cont_x, cont_y, cont_f, seuil, debut, indicePointDistanceMax);
        approximer_contour_recur(cont_x, cont_y, cont_f, seuil, indicePointDistanceMax, fin);
    }

}

//Algorithme de Ramer-Douglas-Peucker

ContourPol approximer_contour_c8(ContourF8 contour, float seuil)
{
    //  Initialisation 

    vector<int> cfc = contour.getChaineFreeman();
    int x_curr = contour.getStart_x();
    int y_curr = contour.getStart_y();
    vector<int> cont_x = {x_curr};
    vector<int> cont_y = {y_curr};
    vector<bool> cont_f = {1};

    for(unsigned int i = 0; i < cfc.size(); i++)
    {   x_curr = next_x(x_curr,cfc[i]);
        cont_x.push_back(x_curr);
        y_curr = next_y(y_curr,cfc[i]);
        cont_y.push_back(y_curr);
        cont_f.push_back(1);
    }

    

    // ETAPE 1 : On cherche le point B le plus eloignés du de A
    int indicePointDistanceMax = 0;
    int DistanceMax = 0;
    

    for(unsigned int i = 1; i < cont_f.size()-1; i++)
    {
        int distanceCarre = normeCarre(cont_x[0] - cont_x[i], cont_y[0] - cont_y[i]);
        if(distanceCarre > DistanceMax)
        {
            DistanceMax = distanceCarre;
            indicePointDistanceMax = i;
        }       
    } 
 
    // ETAPE 2 : APPEL RECURSIF
    
    approximer_contour_recur(cont_x, cont_y, cont_f, seuil, 0, indicePointDistanceMax);
    approximer_contour_recur(cont_x, cont_y, cont_f, seuil, indicePointDistanceMax, cont_f.size()-1);

    //ETAPE 3 : RELAXATION

    cout << " Avant relaxation : " << endl;
    ContourPol pol0 = ContourPol(cont_x, cont_y, cont_f);
    pol0.print();

    for(unsigned int i = 1; i < cont_f.size() - 1; i++)
    {
        int B_x = cont_x[i-1];
        int B_y = cont_y[i-1];
        int A_x = cont_x[i];
        int A_y = cont_y[i];
        int C_x = cont_x[i+1];
        int C_y = cont_y[i+1];
    
        float dA_BC = (C_x - B_x)*(A_y - B_y) - (C_y - B_y)*(A_x - B_x);
    
        dA_BC = pow(dA_BC,2);
        dA_BC /= normeCarre(C_x - B_x, C_y - B_y);

        cout << "dA_BC = " << dA_BC << endl; 

        if(dA_BC < pow(seuil,2))
            cont_f[i] = 0;
        
    }    

    cout << " Après relaxation : " << endl;
    pol0 = ContourPol(cont_x, cont_y, cont_f);
    pol0.print();

    return ContourPol(cont_x, cont_y, cont_f) ;

}

void colorier_morceaux(ContourPol contourPol, cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    int couleur = 1;
    unsigned int i = 0;

    while(i < contourPol.size()-1)
    {
        do
        {
            img_niv.at<int>(contourPol.getcont_y()[i], contourPol.getcont_x()[i]) = couleur;
            i++;
        }
        while(contourPol.getcont_f()[i] != 1 && i < contourPol.size()-2);

        couleur++;

        if(couleur == 255)
            couleur++;

    }
}

void remplir_polyg(ContourPol pol, cv::Mat &img_niv, int coul)
{
  vector<int> cont_x = pol.getcont_x();
  vector<int> cont_y = pol.getcont_y();
  
  vector<cv::Point> fillContSingle;

  //add all points of the contour to the vector
  for(unsigned int i = 0; i < pol.size(); i++)
  {
    if(pol.getcont_f()[i] == 1)
        fillContSingle.push_back(cv::Point(cont_x[i],cont_y[i]));
  }
  
  std::vector<std::vector<cv::Point> > fillContAll;
  //fill the single contour 
  //(one could add multiple other similar contours to the vector)
  fillContAll.push_back(fillContSingle);
  cv::fillPoly( img_niv, fillContAll, cv::Scalar(coul));
}

//----------------------- T R A N S F O R M A T I O N S -----------------------

// Placez ici vos fonctions de transformations à la place de ces exemples

// colore le contour de la forme pour la 8-connexité pour le fond
void marquer_contours_c8 (cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
        if(est_un_contours_c8_fond(img_niv, y, x))
                img_niv.at<int>(y,x) = 1;   
    }
}

// colore le contour de la forme pour la 4-connexité pour le fond
void marquer_contours_c4 (cv::Mat &img_niv)
{
   CHECK_MAT_TYPE(img_niv, CV_32SC1);

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
        int g = img_niv.at<int>(y,x);
        if (g > 0) // si g est un pixel de l'objet
        { 
            if (y == 0  // Et si g est au bord de l'image
             || x == 0 
             || y == img_niv.rows - 1 
             || x == img_niv.cols - 1)
             img_niv.at<int>(y,x) = 1; // alors g fait parti du contour de l'image

        //  On teste si un de ses 8-voisins appartient au fond 
            else if (img_niv.at<int>(y-1,x) == 0
                 || img_niv.at<int>(y+1,x) == 0
                 || img_niv.at<int>(y,x-1) == 0
                 || img_niv.at<int>(y,x+1) == 0
                 || img_niv.at<int>(y-1,x-1) == 0
                 || img_niv.at<int>(y+1,x-1) == 0
                 || img_niv.at<int>(y-1,x+1) == 0
                 || img_niv.at<int>(y+1,x+1) == 0)
                     img_niv.at<int>(y,x) = 1;       
       }  
    }
}

void numeroter_contours_c8 (cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    vector<set<int>> classesEquivalence;
    int label_cur = 1;

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
        set<int> cur_classe;

        // si (y,x) est un point de contour
        if(est_un_contours_c8_fond(img_niv,y,x))
        {
            // si le voisin fait partie de la forme && la forme a déja été marqué
            if (img_niv.at<int>(y-1,x) > 0 && img_niv.at<int>(y-1,x) != 255) 
                cur_classe.insert(img_niv.at<int>(y-1,x));
               
            if (img_niv.at<int>(y+1,x) > 0 && img_niv.at<int>(y+1,x) != 255) 
                cur_classe.insert(img_niv.at<int>(y+1,x));
               
            if (img_niv.at<int>(y,x-1) > 0 && img_niv.at<int>(y,x-1) != 255) 
                cur_classe.insert(img_niv.at<int>(y,x-1));
            
            if (img_niv.at<int>(y,x+1) > 0 && img_niv.at<int>(y,x+1) != 255) 
                cur_classe.insert(img_niv.at<int>(y,x+1));
       
            if (img_niv.at<int>(y-1,x-1) > 0 && img_niv.at<int>(y-1,x-1) != 255) 
                cur_classe.insert(img_niv.at<int>(y-1,x-1));
       
            if (img_niv.at<int>(y+1,x-1) > 0 && img_niv.at<int>(y+1,x-1) != 255) 
                cur_classe.insert(img_niv.at<int>(y+1,x-1));
               
            if (img_niv.at<int>(y-1,x+1) > 0 && img_niv.at<int>(y-1,x+1) != 255) 
                cur_classe.insert(img_niv.at<int>(y-1,x+1));
             
            if (img_niv.at<int>(y+1,x+1) > 0 && img_niv.at<int>(y+1,x+1) != 255) 
                cur_classe.insert(img_niv.at<int>(y+1,x+1));

            // si aucun 8-voisin de dans la forme n'est déjà marqué
            //on le marque avec un nouveau label
            if (cur_classe.size() == 0)
            {
                if (label_cur == 255)
                    label_cur++;

                img_niv.at<int>(y,x) = label_cur;
                label_cur++;
            }

            //sinon, si un seul -voisin de dans la forme est déjà marqué,
            //on recopie son label 
            else if (cur_classe.size()==1)
            {
                auto it = cur_classe.begin();
                img_niv.at<int>(y,x) = *it;
            }
            
            //sinon on choisit le plus petit label et
            //on met à jour les classe d'équivalences
            else
            {
                auto it = cur_classe.begin();
                img_niv.at<int>(y,x) = *it;
                MajClasseEq(classesEquivalence, cur_classe);
            }   
        }
    }

// Pour chaque classe d'équivalence, on rebalaye l'image.
   for(unsigned int i = 0; i != classesEquivalence.size() ; i++)
        {
            auto it = classesEquivalence[i].begin();
            int min = *it;
            it++;
            
            while( it != classesEquivalence[i].end())
            {
                for (int y = 0; y < img_niv.rows; y++)
                for (int x = 0; x < img_niv.cols; x++)
                {
                    if(img_niv.at<int>(y,x) == *it)
                        img_niv.at<int>(y,x) = min;
                }
                it++;
            }   
        } 
}

list<ContourF8> effectuer_suivi_contours_c8(cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    list<ContourF8> listeContours;
    int num_contour = 1;
    int dir = -1;

    for(int y = 0; y < img_niv.rows; y++)
    for(int x = 0; x < img_niv.cols; x++)
    {
        if(img_niv.at<int>(y,x) == 255 && est_un_contours_c8_fond (img_niv, y, x))
        {
            if(x == img_niv.cols - 1 || img_niv.at<int>(y,x+1) == 0)
                dir = 0;
            else if(y == img_niv.rows - 1 || img_niv.at<int>(y+1,x) == 0)
                dir = 2;
            else if(x == 0 || img_niv.at<int>(y,x-1) == 0)
                dir = 4;
            else if(y == 0|| img_niv.at<int>(y-1,x) == 0)
                dir = 6;

            if (dir>=0)
            {
                listeContours.push_back(suivre_un_contour_c8(img_niv, y, x, dir, num_contour));
                
                num_contour++;
                if(num_contour == 255) num_contour++;
            }
        }
    }
   
    
    
    return listeContours;
}

void approximer_et_colorier_contours_c8(cv::Mat &img_niv, float seuil)
{
    list<ContourF8> listeContours = effectuer_suivi_contours_c8(img_niv);

    for( auto it = listeContours.begin(); it != listeContours.end(); it++)
        {
            ContourPol pol = approximer_contour_c8(*it, seuil);
            
            colorier_morceaux(pol, img_niv);
            
        }
}

void approximer_et_remplir_contours_c8(float seuil, cv::Mat &img_niv)
{
    list<ContourF8> listeContours = effectuer_suivi_contours_c8(img_niv);
    img_niv.setTo(0);

    for(auto it = listeContours.begin(); it != listeContours.end(); it++)
    {
        ContourPol contour_curr = approximer_contour_c8(*it, seuil);

        int coul;

        if(it->getDirInitiale() == 6 || it->getDirInitiale() == 4)
            coul = 255;
        else
            coul =0;


        remplir_polyg(contour_curr, img_niv, coul);
    }
}


//----------------------------- I N T E R F A C E -----------------------------


class MonApp : public Astico2D {

  public:
    // Déclarez ici d'autres membres éventuels
   int polyg_niv;

    MonApp (int argc, char **argv) : 
        Astico2D (argc, argv) 
        // initialisez ici vos classes membre éventuelles
    {
        if (!init_ok) return;  // erreur dans l'initialisation

        // Autres actions du constructeur
	creer_slider ( "Polyg", &polyg_niv, 1000);
    }


    void afficher_touches_clavier () override
    {
        // Ceci affiche l'aide de base
        Astico2D::afficher_touches_clavier ();

        // Indiquez ici les touches du clavier et vos transformations
        std::cout <<
            "  1     affiche les contours pour la 8-connexite avec le fond\n"
            "  2     affiche les contours pour la 4-connexité avec le fond\n"
            "  3     affiche chaque contour avec une couleur differente\n"
        << std::endl;
    }


    bool traiter_touche_clavier (char key) override
    {
        switch (key) {

            // Gérez ici les touches de flags.

            // Rajoutez ici les touches pour effectuer_transformations.
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
                // On mémorise la touche pressée
                touche_transfo = key;
                // On précise le mode : M_NIVEAUX ou M_COULEURS
                mode_transfo = M_NIVEAUX;
                break;

            default : return false;  // touche non traitée
        }
        return true;  // touche traitée
    }


    void effectuer_transformations () override
    {
        // Appelez ici vos transformations selon touche_transfo et mode_transfo :
        // - si mode_transfo est M_NIVEAUX, l'image d'entrée est l'image seuillée
        //   img_niv, de type CV_32SC1 ; à la fin, pour l'affichage, il faut la
        //   convertir en couleur dans img_coul, de type CV_8UC3, par exemple avec 
        //   representer_en_couleurs_vga.
        // - si mode_transfo est M_COULEURS, travaillez directement sur img_coul,
        //   de type CV_8UC3, elle sera affichée telle quelle.

        switch (touche_transfo) {
            case '1' :
                marquer_contours_c8 (img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '2' :
                marquer_contours_c4 (img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '3' :
                numeroter_contours_c8 (img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '4' :
                effectuer_suivi_contours_c8 (img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '5' :
                approximer_et_colorier_contours_c8(img_niv, (float)polyg_niv/100);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '6' :
                approximer_et_remplir_contours_c8((float)polyg_niv/100, img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;

        }
    }

};


//---------------------------------- M A I N ----------------------------------


int main (int argc, char **argv)
{
    MonApp app (argc, argv);
    return app.run ();
}

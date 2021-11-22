/*
    demo1.cpp - exemple d'utilisation de Astico2D

    CC BY-SA Edouard.Thiel@univ-amu.fr - 22/08/2021

    Usage :
      $ make demo1
      $ ./demo1 [-mag width height] [-thr seuil] image_in [image_out]
*/

/*
        FINETTE Charly - version du 27/09/2021  
*/


#include "astico2d.hpp"
#include <set>
#include <vector>

using namespace std;

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




void suivre_un_contour_c8(cv::Mat &img_niv, int yA, int xA, int  dirA, int num_contour)
{
    vector<int> nx8 = { 1, 1, 0, -1, -1, -1, 0, 1};
    vector<int> ny8 = { 0, 1, 1, 1, 0, -1, -1, -1};
    int dir_finale = -1;

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
        while( i < 8)
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
                // emplacement pour mémoriser la chaîne de Freeman
                cout << d << " ";
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



void effectuer_suivi_contours_c8(cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

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
                cout << "contour n°" << num_contour << " : ";
                suivre_un_contour_c8(img_niv, y, x, dir, num_contour);
                cout << endl;
                num_contour++;
                if(num_contour == 255) num_contour++;
            }
        }
    }
}

//----------------------------- I N T E R F A C E -----------------------------


class MonApp : public Astico2D {

  public:
    // Déclarez ici d'autres membres éventuels


    MonApp (int argc, char **argv) : 
        Astico2D (argc, argv) 
        // initialisez ici vos classes membre éventuelles
    {
        if (!init_ok) return;  // erreur dans l'initialisation

        // Autres actions du constructeur
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
        }
    }

};


//---------------------------------- M A I N ----------------------------------


int main (int argc, char **argv)
{
    MonApp app (argc, argv);
    return app.run ();
}

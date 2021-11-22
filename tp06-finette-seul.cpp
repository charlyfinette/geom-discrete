/*
    demo1.cpp - exemple d'utilisation de Astico2D

    CC BY-SA Edouard.Thiel@univ-amu.fr - 22/08/2021

    Usage :
      $ make demo1
      $ ./demo1 [-mag width height] [-thr seuil] image_in [image_out]
*/

/*
    FINETTE Charly - version du 08nov
*/


#include "astico2d.hpp"


//----------------------- TYPES DE BASE -----------------------

enum NumeroMasque 
{
    M_D4, M_D8, M_2_3, M_3_4, M_5_7_11, M_LAST
};

class Ponderation
{
    public:

    int     x;
    int     y;
    int     w;

    
    Ponderation(int a,int b, int c)
    {
        x = a;
        y = b;
        w = c;
    }
};

class DemiMasque
{
    public:

    std::list<Ponderation>  liste_ponderations;
    std::size_t             taille;
    NumeroMasque            numero_masque;
    std::string             nom;


    DemiMasque()
    {

    }

    DemiMasque(NumeroMasque numero)
    {
        numero_masque = numero;

        switch (numero)
        {
        case M_D4:
        {
            nom = "M_D4";
            Ponderation p1 = Ponderation(1,0,1);
            Ponderation p2 = Ponderation(0,1,1);
            liste_ponderations.push_back(p1);
            liste_ponderations.push_back(p2);
            taille = liste_ponderations.size();
            break;
        }
        
        case M_D8:
        {
            nom = "M_D8";
            Ponderation p1 = Ponderation(1,0,1);
            Ponderation p2 = Ponderation(0,1,1);
            Ponderation p3 = Ponderation(1,1,1);
            Ponderation p4 = Ponderation(-1,1,1);
          
            liste_ponderations.push_back(p1);
            liste_ponderations.push_back(p2);
            liste_ponderations.push_back(p3);
            liste_ponderations.push_back(p4);
            taille = liste_ponderations.size();
            break;
        }
        
        case M_2_3:
        {
            nom = "M_2_3";
            Ponderation p1 = Ponderation(1,0,2);
            Ponderation p2 = Ponderation(0,1,2);
            Ponderation p3 = Ponderation(1,1,3);
            Ponderation p4 = Ponderation(-1,1,3);
          
            liste_ponderations.push_back(p1);
            liste_ponderations.push_back(p2);
            liste_ponderations.push_back(p3);
            liste_ponderations.push_back(p4);
            taille = liste_ponderations.size();
            break;
        }
        
        
        case M_3_4:
        {
            nom = "M_3_4";
            Ponderation p1 = Ponderation(1,0,3);
            Ponderation p2 = Ponderation(0,1,3);
            Ponderation p3 = Ponderation(1,1,4);
            Ponderation p4 = Ponderation(-1,1,4);
          
            liste_ponderations.push_back(p1);
            liste_ponderations.push_back(p2);
            liste_ponderations.push_back(p3);
            liste_ponderations.push_back(p4);
            taille = liste_ponderations.size();
            break;
        }
        
        case M_5_7_11:
        {
            nom = "M_5_7_11";
            Ponderation p1 = Ponderation(1,0,5);
            Ponderation p2 = Ponderation(0,1,5);
            Ponderation p3 = Ponderation(1,1,7);
            Ponderation p4 = Ponderation(-1,1,7);
            Ponderation p5 = Ponderation(-2,1,11);
            Ponderation p6 = Ponderation(2,1,11);
            Ponderation p7 = Ponderation(-1,2,11);
            Ponderation p8 = Ponderation(1,2,11);
          
            liste_ponderations.push_back(p1);
            liste_ponderations.push_back(p2);
            liste_ponderations.push_back(p3);
            liste_ponderations.push_back(p4);
            liste_ponderations.push_back(p5);
            liste_ponderations.push_back(p6);
            liste_ponderations.push_back(p7);
            liste_ponderations.push_back(p8);
            taille = liste_ponderations.size();
            break;
        }
        
        case M_LAST:
        {
            nom = "M_LAST";
            break;
        }
        
        
        }

    }
};

//----------------------- T R A N S F O R M A T I O N S -----------------------

void calculer_Rosenfeld_DT (cv::Mat &img_niv, DemiMasque dm)
{
   
    

    // Balayage avant
    for(int y = 0; y < img_niv.rows; y++)
    for(int x = 0; x < img_niv.cols; x++)
    {
        if(img_niv.at<int>(y,x) != 0)
        {
            int min = img_niv.at<int>(y,x);
            int val = min;
            auto it = dm.liste_ponderations.begin();

            while(it != dm.liste_ponderations.end())
            {
                Ponderation curr = (*it);
                if(img_niv.at<int>(y - curr.y ,x - curr.x) != 0)
                {
                    val = img_niv.at<int>(y - curr.y ,x - curr.x) + curr.w;
                    if(val < min)
                         min = val;
                }
                it++;
            }

            img_niv.at<int>(y,x) = val;            
        }
        

    }

   // Balayage arrière
    for(int y = img_niv.rows - 1; y >= 0; y--)
    for(int x = img_niv.cols - 1; x >= 0; x--)
    {
        if(img_niv.at<int>(y,x) != 0)
        {
            int min = img_niv.at<int>(y,x);
            int val = min;
            auto it = dm.liste_ponderations.begin();

            while(it != dm.liste_ponderations.end())
            {
                Ponderation curr = (*it);
                if(img_niv.at<int>(y + curr.y,x + curr.x) != 0)
                {
                    val = img_niv.at<int>(y + curr.y,x + curr.x ) + curr.w;
                    if(val < min && val != curr.w)
                        min = val;

                
                }
                it++;
            }

            img_niv.at<int>(y,x) = val;            
        }
    }
}
void calculer_sedt_saito_toriwaki(cv::Mat &img_niv)
{
    // Premier passage: dimension x
    // Aller:

    int dist_fond_gauche;
    int dist_fond_droite;

    for(int y = 0; y < img_niv.rows; y++)
    {
        dist_fond_gauche = INT_MAX;
        
        for(int x = 0; x < img_niv.cols; x++)
        {
            if(img_niv.at<int>(y,x) == 0)
                dist_fond_gauche = 0;
            else if(img_niv.at<int>(y,x) != 0 && !(dist_fond_gauche == INT_MAX)) 
            {
                dist_fond_gauche = sqrt(img_niv.at<int>(y,x-1)) + 1;
                img_niv.at<int>(y,x) = dist_fond_gauche*dist_fond_gauche;
            }  
                
        }
    }
    
    //retour:

    for(int y = 0; y < img_niv.rows; y++)
    {
        dist_fond_droite = INT_MAX;
        
        for(int x = img_niv.cols - 1; x > 0 ; x--)
        {
            if(img_niv.at<int>(y,x) == 0)
                dist_fond_droite = 0;
            else if(img_niv.at<int>(y,x) != 0 && !(dist_fond_droite == INT_MAX)) 
            {
                dist_fond_droite = sqrt(img_niv.at<int>(y,x+1)) + 1;
                int temp = dist_fond_droite*dist_fond_droite;
                if(temp < img_niv.at<int>(y,x))
                    img_niv.at<int>(y,x) = temp;
            }     
        }
    }

    // Deuxième passage : dimension y

    // Pour chaque colonne x:

    for(int x = 0; x < img_niv.cols; x++)
    {
        // on mémorise la colonne
        std::vector<int> mem_col {};

        for(int y = 0; y < img_niv.rows; y++)
            mem_col.push_back(img_niv.at<int>(y,x));

        // pour chaque élément de la colonne :
        for(int y = 0; y < img_niv.rows; y++)
            {
                std::vector<int> curr_col = mem_col;
                // On rajoute l'incrément de distance verticale au carré sur toute la colonne
                for(int i = 0; i < img_niv.rows; i++)
                {
                    curr_col[i] += (y - i)*(y - i);
                }

                // On cherche le minimum de la colonne courrante
                auto min = min_element(curr_col.begin(), curr_col.end());
                img_niv.at<int>(y,x) = *min;
            } 
    }

    

    
}

void calculer_sedt_courbes_niveau (cv::Mat &img_niv)
{
    for(int y = 0; y < img_niv.rows; y++)
    for(int x = 0; x < img_niv.cols; x++)
    {
        if (img_niv.at<int>(y,x) != 0)
            img_niv.at<int>(y,x) = floor(sqrt(img_niv.at<int>(y,x)));
    }

}
//----------------------------- I N T E R F A C E -----------------------------


class MonApp : public Astico2D {

  public:
  NumeroMasque numero_masque;
  DemiMasque demi_masque;
  
    // Déclarez ici d'autres membres éventuels


    MonApp (int argc, char **argv) : 
        Astico2D (argc, argv) 
        // initialisez ici vos classes membre éventuelles
    {
        numero_masque = M_D4;
        demi_masque = DemiMasque(numero_masque);

        if (!init_ok) return;  // erreur dans l'initialisation

        // Autres actions du constructeur
    }


    void afficher_touches_clavier () override
    {
        // Ceci affiche l'aide de base
        Astico2D::afficher_touches_clavier ();

        // Indiquez ici les touches du clavier et vos transformations
        std::cout <<
            "  1     dessine des bandes horizontales\n"
            "  2     dessine des bandes verticales\n"
            "  3     dessine des bandes diagonales\n"
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
            case 'n' :
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
                calculer_Rosenfeld_DT (img_niv, numero_masque);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '2' :
                
                break;
            case '3' :
            
                break;
            case '4' :

                break;
            case '5' :
            calculer_sedt_saito_toriwaki(img_niv);
            representer_en_couleurs_vga (img_niv, img_coul);
            break;
            case '6' :
            calculer_sedt_saito_toriwaki(img_niv);
            calculer_sedt_courbes_niveau(img_niv);
            representer_en_couleurs_vga (img_niv, img_coul);
            break;
            case 'n' :
            {
                int suiv = static_cast<int>(numero_masque)+1;
                numero_masque = static_cast<NumeroMasque>(suiv);
                if (numero_masque == M_LAST) numero_masque = M_D4;
                demi_masque = DemiMasque(numero_masque);
                std::cout << "Numero masque : " 
                    << demi_masque.nom << std::endl;
                break;
            }
        }
    }

};


//---------------------------------- M A I N ----------------------------------


int main (int argc, char **argv)
{
    MonApp app (argc, argv);
    return app.run ();
}


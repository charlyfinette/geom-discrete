/*

    CC BY-SA Edouard.Thiel@univ-amu.fr - 22/08/2021

    Usage :
      $ make demo1
      $ ./demo1 [-mag width height] [-thr seuil] image_in [image_out]
*/

/*
    <FINETTE Charly> - version du lundi 15nov 
*/


#include "astico2d.hpp"
#include <list>
//----------------------- A U X ----------------------------------------------

enum ES_NOM{ CARRE = 0, DIAMANT, LAST };

class Point
{
	public:
	int m_x;
	int m_y;


	Point(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
};

class ES
{
    public:
	std::list<Point> m_liste;
	std::string m_nom;
    ES();
    ES(ES_NOM nom)
    {
        if(nom == CARRE)
        {
            m_liste = { Point(0,0),    // centre
                        Point(1,0),    // bas
                        Point(0, 1),   // droite
                        Point(1, 1),   // bas et droite
                        Point(-1, 0),  // haut
                        Point(0, -1),  // gauche
                        Point(-1, -1), // haut et droite
                        Point(0, -1),  // haut et gauche
                        Point(1, -1)   // bas et gauche
                        };

	    m_nom = "CARRE";
        }
        else if(nom == DIAMANT)
        {
             m_liste = { Point(0,0),    // centre
                         Point(1,0),    // bas
                         Point(0, 1),   // droite
                         Point(-1, 0),  // haut
                         Point(0, -1),  // gauche
                        };

	     m_nom = "DIAMANT";
        }
    }

};

//----------------------- T R A N S F O R M A T I O N S -----------------------

// Placez ici vos fonctions de transformations à la place de ces exemples


void  erosion(cv::Mat &img_niv, ES &es)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
	    if(img_niv.at<int>(y,x) != 0)// attention
	    {
		for( auto const& point  : es.m_liste)
		{
			std::cout << point.m_x << std::endl;
			std::cout << point.m_y << std::endl;	
		}
	    }
    }
}


void transformer_bandes_verticales (cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
        int g = img_niv.at<int>(y,x);
        if (g > 0) {
            img_niv.at<int>(y,x) = x;
        }
    }
}


void transformer_bandes_diagonales (cv::Mat &img_niv)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
        int g = img_niv.at<int>(y,x);
        if (g > 0) {
            img_niv.at<int>(y,x) = x+y;
        }
    }
}


//----------------------------- I N T E R F A C E -----------------------------


class MonApp : public Astico2D {

  public:
    // Déclarez ici d'autres membres éventuels
    ES es;
    ES_NOM es_nom;

    MonApp (int argc, char **argv) : 
        Astico2D (argc, argv) 
        // initialisez ici vos classes membre éventuelles
    {
//	    es_nom = CARRE;
//	    es = ES(es_nom);
        if (!init_ok) return;  // erreur dans l'initialisation

        // Autres actions du constructeur
    }


    void afficher_touches_clavier () override
    {
        // Ceci affiche l'aide de base
        Astico2D::afficher_touches_clavier ();

        // Indiquez ici les touches du clavier et vos transformations
        std::cout <<
            "  1     erosion\n"
            "  2     dessine des bandes verticales\n"
            "  3     dessine des bandes diagonales\n"
	    "  n     change l'élément structurant\n"
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
	    case 'n' :
                // On mémorise la touche pressée
                touche_transfo = key;
                // On précise le mode : M_NIVEAUX ou M_COULEURS
                mode_transfo = M_NIVEAUX;
                break;
	    case 'n':
	{
                int suiv = static_cast<int>(es_nom)+1;
                es_nom   = static_cast<ES_NOM>(suiv);
                if (es_nom == LAST) es_nom = CARRE;
                es = ES(es_nom);
                std::cout << "Element structurant : "
                    << es.m_nom << std::endl;
		break;
	}


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
                erosion(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '2' :
                transformer_bandes_verticales (img_niv);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '3' :
                transformer_bandes_diagonales (img_niv);
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


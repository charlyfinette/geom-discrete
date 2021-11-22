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

enum ES_NOM{ CARRE = 0, DIAMANT, TOR_0, TOR_1, AMINC_a_0, AMINC_a_1, AMINC_b_0, AMINC_b_1, LAST };

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
    ES() {};
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
	else if(nom == TOR_0)
        {
             m_liste = { 
                         Point(0, 1),   // droite
                         Point(-1, 0),  // haut
                         Point(-1, -1),  // haut droite 
                        };

	     m_nom = "TOR_0";
        }
	else if(nom == TOR_1)
        {
             m_liste = { Point(0,0),    // centre
                         Point(1,0),    // bas
                         Point(0, -1),  // gauche
                        };

	     m_nom = "TOR_1";
        }
	else if(nom == AMINC_a_0)
        {
            m_liste = { 
                        Point(-1, 0),  // haut
                        Point(-1, -1), // haut et droite
                        Point(0, -1),  // haut et gauche
                        };

	     m_nom = "AMINC_a_0";
        }
	else if(nom == AMINC_a_1 )
        {
            m_liste = { Point(0,0),    // centre
                        Point(1,0),    // bas
                        Point(1, 1),   // bas et droite
                        Point(1, -1)   // bas et gauche
                        };

	     m_nom = "AMINC_a_1";
        }else if(nom == AMINC_b_0)
        {
            m_liste = {
                        Point(0, 1),   // droite
                        Point(-1, 0),  // haut
                        Point(-1, -1), // haut et droite
                        };

	     m_nom = "AMINC_b_0";
        }
	else if(nom == AMINC_b_1 )
        {
            m_liste = { Point(0,0),    // centre
                        Point(1,0),    // bas
                        Point(0, -1),  // gauche
                        Point(1, -1)   // bas et gauche
                        };

	     m_nom = "AMINC_b_1";
        }
    }

};


struct TOR_Mask 
{
	ES es0;
	ES es1;
};

//----------------------- T R A N S F O R M A T I O N S -----------------------

// Placez ici vos fonctions de transformations à la place de ces exemples

void  une_erosion(cv::Mat &img_niv, ES &es)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    cv::Mat img_cp = img_niv.clone();

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
	img_niv.at<int>(y,x) = 0;
	bool is_in_new_shape = true;

	for( const auto & neigh : es.m_liste )
	{
		if(y + neigh.m_y < 0
		|| y + neigh.m_y > img_niv.rows - 1
		|| x + neigh.m_x < 0
		|| x + neigh.m_x > img_niv.cols - 1)
			is_in_new_shape = false;
		else
        		is_in_new_shape *= (img_cp.at<int>(y + neigh.m_y, x + neigh.m_x) != 0 );
	}

	if(is_in_new_shape)
	 	img_niv.at<int>(y,x) = 255;
    }
}

void  erosion(cv::Mat &img_niv, ES &es, int nbr_repetitions)
{
	for( int i = 0; i < nbr_repetitions; i++)
	{
		une_erosion(img_niv, es);	
	}	
}

void une_dilatation(cv::Mat &img_niv, ES es)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    cv::Mat img_cp = img_niv.clone();

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
	img_niv.at<int>(y,x) = 0;
	bool is_in_new_shape = false;

	for( const auto & neigh : es.m_liste )
	{
		if(!(y + neigh.m_y < 0
		|| y + neigh.m_y > img_niv.rows - 1
		|| x + neigh.m_x < 0
		|| x + neigh.m_x > img_niv.cols - 1))
		{
			if( img_cp.at<int>(y + neigh.m_y, x + neigh.m_x) != 0)
			{
				is_in_new_shape = true;
				break;
			}
		}
	}

	if(is_in_new_shape)
	 	img_niv.at<int>(y,x) = 255;
    }
}

void  dilatation(cv::Mat &img_niv, ES &es, int nbr_repetitions)
{
	for( int i = 0; i < nbr_repetitions; i++)
	{
		une_dilatation(img_niv, es);	
	}	
}

void ouverture(cv::Mat &img_niv, ES es)
{
	une_erosion(img_niv, es);
	une_dilatation(img_niv, es);
}

void fermeture(cv::Mat &img_niv, ES es)
{
	une_dilatation(img_niv, es);
	une_erosion(img_niv, es);
}

void debruiter(cv::Mat &img_niv, ES &es)
{
	ouverture(img_niv, es);
	fermeture(img_niv, es);
}

void ouverture_puis_fermeture(cv::Mat &img_niv, ES &es)
{
	ouverture(img_niv, es);
	fermeture(img_niv, es);
}

void fermeture_puis_ouverture(cv::Mat &img_niv, ES &es)
{
	fermeture(img_niv, es);
	ouverture(img_niv, es);
}


void reconstruction_morphologique(cv::Mat &img_niv, ES &es)
{ 

}

void hit(cv::Mat &img_niv, TOR_Mask &mask)
{
    CHECK_MAT_TYPE(img_niv, CV_32SC1);

    cv::Mat img_cp = img_niv.clone();

    for (int y = 0; y < img_niv.rows; y++)
    for (int x = 0; x < img_niv.cols; x++)
    {
	img_niv.at<int>(y,x) = 0;

	bool es0_b = true;
	// si tous les éléments de l'es mask.es0 font parti du fond
	// es0_b = true;
	
	for( const auto & neigh : mask.es0.m_liste )
	{
		
		if(!(y + neigh.m_y < 0
		|| y + neigh.m_y > img_niv.rows - 1
		|| x + neigh.m_x < 0
		|| x + neigh.m_x > img_niv.cols - 1))
		{
		
			if(img_niv.at<int>(y + neigh.m_y, x + neigh.m_x) != 0 )
			{
				es0_b = false;
				break;
			}
		}
	}


	bool es1_b = true;
	// si tous les éléments de l'es mask.es1 font parti de la forme
	// es1_b = true;
	

	for( const auto & neigh : mask.es1.m_liste )
	{
		
		if(y + neigh.m_y < 0
		|| y + neigh.m_y > img_niv.rows - 1
		|| x + neigh.m_x < 0
		|| x + neigh.m_x > img_niv.cols - 1)
		{
			es1_b = false;
			break;
		}
		else
		{
			if(img_niv.at<int>(y + neigh.m_y, x + neigh.m_x) == 0)
			{
				es1_b = false;
				break;
			}	
		}
	}

	bool is_in_new_shape = es0_b && es1_b;

	if(is_in_new_shape)
	 	img_niv.at<int>(y,x) = 255;

    }
}

//----------------------------- I N T E R F A C E -----------------------------


class MonApp : public Astico2D {

  public:
    // Déclarez ici d'autres membres éventuels
    ES es;
    ES_NOM es_nom;
    int repetitions;
    TOR_Mask tm;
    TOR_Mask AMINC_a_tm;
    TOR_Mask AMINC_b_tm;

    MonApp (int argc, char **argv) : 
        Astico2D (argc, argv) 
        // initialisez ici vos classes membre éventuelles
    {
	    repetitions = 1;
	    es_nom = CARRE;
	    es = ES(es_nom);
	    tm.es0 = ES(TOR_0);
	    tm.es1 = ES(TOR_1);
	    AMINC_a_tm.es0 = ES(AMINC_a_0);
	    AMINC_a_tm.es1 = ES(AMINC_a_1); 
	    AMINC_b_tm.es0 = ES(AMINC_b_0);
	    AMINC_b_tm.es1 = ES(AMINC_b_1);
        if (!init_ok) return;  // erreur dans l'initialisation
	creer_slider ("Répétitions", &repetitions, 50);
        // Autres actions du constructeur
    }


    void afficher_touches_clavier () override
    {
        // Ceci affiche l'aide de base
        Astico2D::afficher_touches_clavier ();

        // Indiquez ici les touches du clavier et vos transformations
        std::cout <<
            "  1     erosion\n"
            "  2     dilatation\n"
            "  3     ouverture\n"
	    "  4     fermeture\n"
	    "  5     debruitage\n"
	    "  6     ouverture puis fermeture\n"
	    "  5     fermeture puis ouverture\n"
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
	    case '4' :
	    case '5' :
	    case '6' :
	    case '7' :
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
		debruiter(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '2' :
		hit(img_niv, tm);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '3' :
//                ouverture(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
            case '4' :
//                fermeture(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
	    case '5' :
//                debruiter(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
	    case '6' :
//               ouverture_puis_fermeture(img_niv, es);
                representer_en_couleurs_vga (img_niv, img_coul);
                break;
	    case '7' :
//                fermeture_puis_ouverture(img_niv, es);
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


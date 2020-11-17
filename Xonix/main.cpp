// on a pas le droit de se croiser (comme dans serpent)
// si un ennemi touche la ligne verte avant le drop (verrouillage en mur bleu) on perd
// on gagne si on r�duit la proportion dans laquelle les ennemis �voluent au fur et � mesure des niveaux
// si j'ai bien compris. Il n'y a ici qu'un seul niveau.


// Le jeux pr�c�dent m'avait fatigu� car plus dur. Celui-ci est plus sympathique, me redonne g�ut d'aller au bout des 16 jeux.
// je vois que plus je fais de jeux, plus la lecture du code est ais� et en g�n�ral, je bloque que sur les aspects de logique de jeu
// qui necessite de "voir ce qui se passe" en clair.
// ca vaut le coup d'aller au bout des 16 car alors j'aurais la version "sans game engine et d�coupage de code" et je pourrais comparer avec les
// version avec "game engine diy" c'est � dire avec classe et s�paration plus claire entre logique de jeu, affichage, bref, plus poo.

// l'avantage de ces petits jeux, c'est qu'ils tiennent tous en une seule fonction principale sans classe, � la rigueur une structure.
// mais on voit que si il fallait mettre des niveaux ou tout simplement avoir une logique un poil plus complexe et le code deviendrait super long avec des aller-retours
// pesants.


#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int M = 25;
const int N = 40;

int grid[M][N] = {0};
int ts = 18; //tile size

struct Enemy
{int x,y,dx,dy;

  Enemy()
   {
    x=y=300;

    // technique permettant de g�nerer un nombre al�atoire entre +4 et -4
    dx=4-rand()%8;
    dy=4-rand()%8;
   }

  void move()
   { 
      // rebond si touche les bords ou tout element bleu = � 1.
      // il est � noter que le rebond est bien d'allure triangulaire si seulement x ou y est invers� car
      // l'autre x ou y reste identique.

    x+=dx; if (grid[y/ts][x/ts]==1) {dx=-dx; x+=dx;}
    y+=dy; if (grid[y/ts][x/ts]==1) {dy=-dy; y+=dy;}
   }
};


// fonction qui permet de g�nerer un mur � l'interieur de ce qui a �t� cercl�
// par encore bien claire, appel de r�cursivit�. On ferra du debug dessus.

// Ca y est j'ai pig�: il faut regarder si un ennemi est emprisonn� dans la boite
// si oui, toutes les cases passent � -1 (pour les diff�rencier de celles vont passer � zero qui deviendront egal � 1
// le point c'est que au fur et � mesure du jeu, les ennemis peuvent �tre separ� dans des espaces diff�rents. il faut donc
// verifier pour chaque ennemi si dans une boite donn�, il s'y trouve de tel mani�re � ne pas passer cette zone en mur.
// l'appel recursif, sans que je rentre dans les d�tail du code permettra j'imagine pour au moins une des quatres lignes, � remplir de 
// de -1 l'ensemble de la boite contenant l'ennemi.

void drop(int y,int x)
{
  if (grid[y][x]==0) grid[y][x]=-1;
  if (grid[y-1][x]==0) drop(y-1,x);
  if (grid[y+1][x]==0) drop(y+1,x);
  if (grid[y][x-1]==0) drop(y,x-1);
  if (grid[y][x+1]==0) drop(y,x+1);
}

int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(N*ts, M*ts), "Xonix Game!");
    window.setFramerateLimit(60);

    sf::Vector2i Position(1, 1);
    window.setPosition(Position);

    Texture t1,t2,t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100,100);

    // les ennemis ayant une forme d'�toile de ninja tournante, il faut redefinir le vrai origine au centre (20,20) car image 40x40
    sEnemy.setOrigin(20,20);

    int enemyCount = 4;

    // l'auteur a l'habitude de d�finir des tableaux plus grand que necessaire (au cas ou il decide de changer le nombre d'ennemis 
    // ou par horreur des buffer overflow?

    Enemy a[10];

    bool Game=true;
    int x=0, y=0, dx=0, dy=0;
    float timer=0, delay=0.07, timer2=0; 
    Clock clock;

    for (int i=0;i<M;i++)
     for (int j=0;j<N;j++)
         // tous les bords sont d�finis comme mur bleu de valeur 1
      if (i==0 || j==0 || i==M-1 || j==N-1)  grid[i][j]=1;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();// regarde le temps pass�
        clock.restart();// redemarre le time a chaque it�ration
        timer+=time;// incr�mente le timer.
        timer2 += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
               
            if (e.type == Event::KeyPressed)
             if (e.key.code==Keyboard::Escape)
               {
                for (int i=1;i<M-1;i++)
                 for (int j=1;j<N-1;j++)
                     // si l'utilisateur veut rejouer en appuyant sur escape, repasse tout l'int�rieur � zero mais pas les murs.
                   grid[i][j]=0;

                x=10;y=0;// remet en position initiale
                Game=true; 
               }
        }
        // deplacement classique selon la grille
        if (Keyboard::isKeyPressed(Keyboard::Left)) {dx=-1;dy=0;};
        if (Keyboard::isKeyPressed(Keyboard::Right))  {dx=1;dy=0;};
        if (Keyboard::isKeyPressed(Keyboard::Up)) {dx=0;dy=-1;};
        if (Keyboard::isKeyPressed(Keyboard::Down)) {dx=0;dy=1;};
        
        if (!Game) continue;


        // lorsque le timer devient sup�rieur � 0.07s
        if (timer>delay)
        {
         x+=dx; // change la position de la "t�te" en x, puis en y
         y+=dy;

         if (x<0) x=0; if (x>N-1) x=N-1;// on peut rentrer se proteger dans les murs mais on ne depasse pas les murs.
         if (y<0) y=0; if (y>M-1) y=M-1;

         if (grid[y][x]==2) Game=false;// si on se croise (le rouge arrive sur le vert); on perd.
         if (grid[y][x]==0) grid[y][x]=2;// si la t�te est � cet endroit (est pass� par l�) ca passe � deux et ca deviendra vert.
         timer=0;
        }

        // on a nos 4 ennemis et on les fait bouger en appelant la fonction move (mais s'ils d�passent, ils rebondissent)
        for (int i=0;i<enemyCount;i++) a[i].move();

        // si on arrive dans un mur, bleu, on remet dx et dy � zero (sinon tant qu'on r�appuie pas, ils restent identiques).

        if (grid[y][x]==1)
          {
           dx=dy=0;


           // on regarde l'ensemble des espaces clots contenant des ennemis et on les remplis de -1 si ils contiennent
           // au moins un ennemi

           for (int i=0;i<enemyCount;i++)
                drop(a[i].y/ts, a[i].x/ts);

           // on passe alors en revu toute les cases: celles qui sont � -1 repasse � zero (vide)
           // celles qui sont � zero passe � 1: elles ne contenaient pas d'ennemi et donc deviennent des murs.

           for (int i=0;i<M;i++)
             for (int j=0;j<N;j++)
              if (grid[i][j]==-1) grid[i][j]=0;
              else grid[i][j]=1;
          }

        for (int i=0;i<enemyCount;i++)
           if  (grid[a[i].y/ts][a[i].x/ts]==2) Game=false; //  si un ennemi touche la ligne verte

      /////////draw//////////
        // on rep�re mieux la s�paration de la logique du jeu de la partie dessin.



      window.clear();

      for (int i=0;i<M;i++)
        for (int j=0;j<N;j++)
         {
            if (grid[i][j]==0) continue;
            if (grid[i][j]==1) sTile.setTextureRect(IntRect( 0,0,ts,ts));// les murs bleus
            if (grid[i][j]==2) sTile.setTextureRect(IntRect(54,0,ts,ts));// vert
            sTile.setPosition(j*ts,i*ts);
            window.draw(sTile);
         }

      sTile.setTextureRect(IntRect(36,0,ts,ts));// la t�te en rouge au coordonn�es x et y en cours
      sTile.setPosition(x*ts,y*ts);
      window.draw(sTile);

      sEnemy.rotate(10);// fonction rotate d�fini dans la SFML d'o� le fait qu'on a repositionn� le centre
      for (int i=0;i<enemyCount;i++)
       {
        sEnemy.setPosition(a[i].x,a[i].y);// r�actualisation de l'image des ennemis � l'ecran.
        window.draw(sEnemy);
       }

      if (!Game) window.draw(sGameover);

       window.display();
    }

    return 0;
}

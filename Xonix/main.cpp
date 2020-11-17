// on a pas le droit de se croiser (comme dans serpent)
// si un ennemi touche la ligne verte avant le drop (verrouillage en mur bleu) on perd
// on gagne si on réduit la proportion dans laquelle les ennemis évoluent au fur et à mesure des niveaux
// si j'ai bien compris. Il n'y a ici qu'un seul niveau.

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

    // technique permettant de génerer un nombre aléatoire entre +4 et -4
    dx=4-rand()%8;
    dy=4-rand()%8;
   }

  void move()
   { 
      // rebond si touche les bords ou tout element bleu = à 1.
      // il est à noter que le rebond est bien d'allure triangulaire si seulement x ou y est inversé car
      // l'autre x ou y reste identique.

    x+=dx; if (grid[y/ts][x/ts]==1) {dx=-dx; x+=dx;}
    y+=dy; if (grid[y/ts][x/ts]==1) {dy=-dy; y+=dy;}
   }
};


// fonction qui permet de génerer un mur à l'interieur de ce qui a été cerclé
// par encore bien claire, appel de récursivité. On ferra du debug dessus.
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

    Texture t1,t2,t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100,100);

    // les ennemis ayant une forme d'étoile de ninja tournante, il faut redefinir le vrai origine au centre (20,20) car image 40x40
    sEnemy.setOrigin(20,20);

    int enemyCount = 4;

    // l'auteur a l'habitude de définir des tableaux plus grand que necessaire (au cas ou il decide de changer le nombre d'ennemis 
    // ou par horreur des buffer overflow?

    Enemy a[10];

    bool Game=true;
    int x=0, y=0, dx=0, dy=0;
    float timer=0, delay=0.07; 
    Clock clock;

    for (int i=0;i<M;i++)
     for (int j=0;j<N;j++)
         // tous les bords sont définis comme mur bleu de valeur 1
      if (i==0 || j==0 || i==M-1 || j==N-1)  grid[i][j]=1;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();// regarde le temps passé
        clock.restart();// redemarre le time a chaque itération
        timer+=time;// incrémente le timer.

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
                     // si l'utilisateur veut rejouer en appuyant sur escape, repasse tout l'intérieur à zero mais pas les murs.
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


        // lorsque le timer devient supérieur à 0.07s
        if (timer>delay)
        {
         x+=dx; // change la position de la "tête" en x, puis en y
         y+=dy;

         if (x<0) x=0; if (x>N-1) x=N-1;// on peut rentrer se proteger dans les murs mais on ne depasse pas les murs.
         if (y<0) y=0; if (y>M-1) y=M-1;

         if (grid[y][x]==2) Game=false;// si on se croise (le rouge arrive sur le vert); on perd.
         if (grid[y][x]==0) grid[y][x]=2;// si la tête est à cet endroit (est passé par là) ca passe à deux et ca deviendra vert.
         timer=0;
        }

        // on a nos 4 ennemis et on les fait bouger en appelant la fonction move (mais s'ils dépassent, ils rebondissent)
        for (int i=0;i<enemyCount;i++) a[i].move();

        // si on arrive dans un mur, bleu, on remet dx et dy à zero (sinon tant qu'on réappuie pas, ils restent identiques).

        if (grid[y][x]==1)
          {
           dx=dy=0;

           for (int i=0;i<enemyCount;i++)
                drop(a[i].y/ts, a[i].x/ts);// comprend pas encore

           // comprends pas encore
           for (int i=0;i<M;i++)
             for (int j=0;j<N;j++)
              if (grid[i][j]==-1) grid[i][j]=0;
              else grid[i][j]=1;
          }

        for (int i=0;i<enemyCount;i++)
           if  (grid[a[i].y/ts][a[i].x/ts]==2) Game=false; //  si un ennemi touche la ligne verte

      /////////draw//////////
        // on repère mieux la séparation de la logique du jeu de la partie dessin.



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

      sTile.setTextureRect(IntRect(36,0,ts,ts));// la tête en rouge au coordonnées x et y en cours
      sTile.setPosition(x*ts,y*ts);
      window.draw(sTile);

      sEnemy.rotate(10);// fonction rotate défini dans la SFML d'où le fait qu'on a repositionné le centre
      for (int i=0;i<enemyCount;i++)
       {
        sEnemy.setPosition(a[i].x,a[i].y);// réactualisation de l'image des ennemis à l'ecran.
        window.draw(sEnemy);
       }

      if (!Game) window.draw(sGameover);

       window.display();
    }

    return 0;
}

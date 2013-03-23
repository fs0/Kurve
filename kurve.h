/*******************************************************************************
 *
 *    Code written by Ramesh (rameshg87@gmail.com)
 *    Release under Open Source
 *
 *****************************************************************************/  

#include <QWidget>
#include <QPainter>
#include <QTextStream>
#include <QMessageBox>
#include <math.h>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>

typedef struct state
{
    float x;
    float y;
    int dir;
    bool flag; 
    int finali;   
} state;

class zsnake 
{

  public:
    bool isalive;
    float curx,cury;
    int cdir;
    int pressdir;
    QColor color;
    int clkey,anclkey;
    int points;
    bool iscpu;
  
    void ssnake (int a,int b,int c)
      {
        curx = a; cury = b; cdir = c;
        isalive = true;
      }
      
    void setdead ()
    {
      isalive = false;
    }  
      
    void presseddir (int dir)
    {
      pressdir = dir;
    }  
    void setdir ();
    void move ();
};

class zwidget : public QWidget
{
  Q_OBJECT
  int maxx,maxy;
  int dx,dy;
  QPen *pen;
  zsnake z1[6];
  int zcount;
  QTimer *timer,*ktimer,*htimer1,*htimer2;
  bool started;
  int state;
  bool clearbackground;
  bool score_changed;
  bool draw;
  
  bool **marker;
  int p[6];
  bool mouse;
  int mplayer;
  
  float initx,inity,initdir;

  public:
  
    zwidget ();
    
    void make_decision_for (int);
    void paintEvent (QPaintEvent*);
    void keyPressEvent (QKeyEvent*);
    void keyReleaseEvent (QKeyEvent*);
    void mousePressEvent (QMouseEvent*);
    void mouseReleaseEvent (QMouseEvent*);
    struct state is_obstacle_present (struct state,int,int);
    int triple_recursive_function (struct state,int );
    int quick_decision (struct state,int);
    
  public slots:
    void makemove ();  
    void keymove ();
    void setsnakes ();
    void setupsnakes ();
    void startgame ();
    void stopgame ();
    void setdraw ();
    void resetdraw ();


};


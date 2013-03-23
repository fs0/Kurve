/*******************************************************************************
 *
 *    Code written by Ramesh (rameshg87@gmail.com)
 *    Release under Open Source
 *
 *****************************************************************************/      

#include "kurve.h"

/* States of the game
   
1 -> Starting and displaying the title
2 -> Selecting the controls
3 -> Create all the snakes and start the timer
4 -> Game in Progress
5 -> All Snakes die but game not over yet
6 -> Game over show final score

*/

#define MOVE_TIMER_PERIOD	8      // speed (higher number = slower)
#define CONTROLLER_TIMER_PERIOD	80 // circumference
#define SNAKE_THICKNESS		3
#define	ANGULAR_CHANGE		22
#define HOLE_TIMER_PERIOD	3000
#define	HOLE_EXISTENCE_PERIOD	140
#define CLOCKWISE 1
#define ANTI_CLOCKWISE 0


float mul_const = 3.1415 / 180;

//Changes the angle according to the direction stored in the buffer pressdir
void zsnake :: setdir ()
{
  
  //If no key was pressed in the time span
  if (pressdir == 2)return;
  
  bool direction = pressdir;
  
  if (direction == false)
    cdir += ANGULAR_CHANGE; //Anticlockwise
  else
    cdir -= ANGULAR_CHANGE; //Clockwise
  
  //If completion of a rotation occurs in either direction
  if (cdir < 0) 
    cdir = cdir + 360;
  
  if (cdir >= 360)
    cdir = cdir - 360;
} 

//Funtion to recalculate the current x and y co-ordinates according to the changed angle
void zsnake :: move ()
{
  int tempdir = 360 - cdir;
  
  if (tempdir < 0 ) tempdir = tempdir + 360;
  
  ///////////////////////-------ACTUAL MOVEMENT EQUATION----------//////////////////

  curx = curx + cos (tempdir * mul_const);
  cury = cury + sin (tempdir * mul_const);

  //////////////////////////////////////////////////////////////////////////////////
}

///////////////// FUNCTIONS THAT CREATE THE HOLE //////////////////////////////////

//the actual co-ordinates are plotted only if draw = true
//draw is set to zero every HOLE_TIMER_PERIOD and set to 1 after HOLE_EXISTENCE_PERIOD

void zwidget :: setdraw ()
{
  draw = true;
  htimer1 -> stop ();
}

void zwidget :: resetdraw ()
{
  draw = false;
  htimer1 -> start (HOLE_EXISTENCE_PERIOD);
}
////////////////////////////////////////////////////////////////////////////////////

//Constructor for the window widget
zwidget :: zwidget ()
{
  //game in start state displaying the title
  state = 1;
  draw = true;
  setWindowTitle ("Zatacka");
  
  
  
  //timer for moving the snake
  timer = new QTimer (this);
  connect (timer,SIGNAL(timeout()),
             this,SLOT (makemove()));
  
  //timer for making the response to keyboard/mouse action    
  ktimer = new QTimer (this);
  connect (ktimer,SIGNAL(timeout()),
	   this,SLOT (keymove()));

  htimer1 = new QTimer (this);
  connect (htimer1,SIGNAL(timeout()),
	   this,SLOT (setdraw()));

  htimer2 = new QTimer (this);
  connect (htimer2,SIGNAL(timeout()),
	   this,SLOT (resetdraw()));
  
  
  //Set our widget to opaque
  setAttribute (Qt::WA_OpaquePaintEvent,true);
    
  //Show it maximized
  showFullScreen ();
  
  //Get the height and width of the screen
  maxx = 1024;
  maxy = 768;
  
  FILE *file = fopen ("kurve.ini","r");
  
  if (file)
  {
    char *temp,delim[]=" \t=",line[100];
    while (!feof (file))
    {
      fgets (line,100,file);
      temp = strtok (line,delim);
      if (temp)
      {
        if (strcmp (temp,"resx") == 0)
        {
           temp = strtok (NULL,delim);
           maxx = strtol (temp,NULL,10);          
        }
        if (strcmp (temp,"resy") == 0)
        {
           temp = strtok (NULL,delim);
           maxy = strtol (temp,NULL,10);          
        }
      } 
    }
  
  }
  
  //Calculate the game region
  dx = int(maxx*0.88);
  dy = int(maxy);
  
  marker = new bool* [maxx];
  for (int i=0;i<maxx;i++)
    marker [i] = new bool [maxy];
  
  //variable to tell display function to update the score
  score_changed = true;
  
}

//Function to set the snakes and start the timers
void zwidget :: startgame ()
{
  setsnakes ();
  
  //start the timer
  timer -> start (MOVE_TIMER_PERIOD);
  ktimer -> start (CONTROLLER_TIMER_PERIOD);
  htimer2 -> start (HOLE_TIMER_PERIOD);
}

//all snakes dead,time to start a new one
void zwidget :: stopgame ()
{

  //stop the timer
  timer -> stop ();
  ktimer -> stop ();

  //memset is not giving good results for me
  for (int i=0;i<maxx;i++)
    for (int j=0;j<maxy;j++)
	     marker[i][j]=0;

  //check whether someone has won the game

  bool gameover=false;
  int limit = (zcount-1) * 10;
  
  for (int i=0;i<zcount;i++)
    if (z1[i].points >= limit) 
      gameover = true;
  
  if (!gameover)
    state=3;
  else
  { 
    state=6;
    sleep (2); 
  }
}

//function to set the snakes
void zwidget :: setsnakes ()
{
    //Set the random number generator seed
  srand(time(0));
  
  clearbackground = true;
  score_changed = true;
  
  for (int i=0;i<zcount;i++)
    z1[i].isalive = false;
  
  for (int i=0;i<zcount;i++)
    {
      //Create a snake object
      z1 [i] . curx = (rand () % (dx-150)) + 75;
      z1 [i] . cury = (rand () % (dy-100)) + 100;
      z1 [i] . cdir = rand () % 360;
      z1 [i] . isalive = true;
      //Make the snake object move straight
      z1[i] . presseddir (2);
      repaint ();
      sleep (1);  
    }
  score_changed = true;
}

//function to set the color and key of the snakes
void zwidget :: setupsnakes ()
{
  int i=-1;

  if (p[0])
    {
      i++;
      z1[i] . color = Qt::red;
      z1 [i] . anclkey = Qt::Key_Q;
      z1 [i] . clkey = Qt::Key_A;
      z1 [i] . points = 0;
      if (p[0] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
  if (p[1])
    { 
      i++;
      z1[i] . color = Qt::yellow;
      z1 [i] . anclkey = Qt::Key_X;
      z1 [i] . clkey = Qt::Key_C;
      z1 [i] . points = 0;
      if (p[1] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
  if (p[2])
    {
      i++;
      z1[i] . color = Qt::gray;
      z1 [i] . anclkey = Qt::Key_M;
      z1 [i] . clkey = Qt::Key_Comma;
      z1 [i] . points = 0;
      if (p[2] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
  if (p[3])
    {
      i++;
      z1[i] . color = Qt::blue;
      z1 [i] . anclkey = Qt::Key_V;
      z1 [i] . clkey = Qt::Key_B;
      z1 [i] . points = 0;
      if (p[3] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
  if (p[4])
    {  
      i++;
      z1 [i] . color = Qt::magenta;
      z1 [i] . clkey = Qt::Key_Up;
      z1 [i] . anclkey = Qt::Key_Right;
      z1 [i] . points = 0;
      if (p[4] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
  if (p[5])
    {
      i++;
      z1[i] . color = Qt::white;
      z1 [i] . anclkey = Qt::Key_Left;
      z1 [i] . clkey = Qt::Key_Down;
      z1 [i] . points = 0;
      if (p[5] == 2) 
	z1[i].iscpu = true;
      else
	z1[i].iscpu = false;
    }
}

///////////////////////// FUNCTIONS THAT MAKE DECISION FOR CPU PLAYERS /////////

struct state zwidget::is_obstacle_present (struct state s,int angular_change,int count)
{
    
    float backx,backy,tempx = s.x,tempy = s.y;
    int i,dir = s.dir;
    int t1,t2,t3,t4;
    struct state b;
    b.flag = true;
    
    if (dir >= 360)
      dir = dir - 360;
    if (dir < 0)
      dir = dir + 360;
    
    
    for (i=0;i<count;i++)
    {
      if (angular_change)
        if (i%6 == 0)
        {
          dir += angular_change;
          if (dir >= 360)
            dir = dir - 360;
          if (dir < 0)
            dir = dir + 360;
        }
      
      backx = tempx;
      backy = tempy;
      
      tempx = tempx + cos ((360-dir) * mul_const);
      tempy = tempy + sin ((360-dir) * mul_const);
    
      t1 = int (nearbyint(tempx));
      t2 = int (nearbyint(tempy));
      t3 = int (nearbyint (backx));
      t4 = int (nearbyint (backy));
    
      if (t1 == t3 && t2 == t4)continue;
   
      if (t1 <= 0 || t1 >= dx || t2 <= 0 || t2 >= dy)
      {
        b.flag = false;
        break;
      }
    
      t3 = int (nearbyint (backx));
      t4 = int (nearbyint (backy));
    
      if (marker [t1][t2] || (marker [t1][t4] && marker[t3][t2]))
      {
        b.flag = false;
        break;
      }
    }
    
    if (b.flag)
    {
      b.x = tempx;
      b.y = tempy;
      b.dir = dir;
    }
    b.finali = i;
    return b;
}                          

int zwidget::triple_recursive_function (struct state s,int level)
{
  if (level == 3)
    return 1;
  else
  {
    struct state s1;
    int t1=0,t2=0,t3=0;
    s1 = is_obstacle_present (s,9,6);
    if (s1.flag)
      t1 = triple_recursive_function (s1,level+1);
    s1 = is_obstacle_present (s,0,6);
    if (s1.flag)
      t2 = triple_recursive_function (s1,level+1);
    s1 = is_obstacle_present (s,-9,6);
    if (s1.flag)
      t3 = triple_recursive_function (s1,level+1);
    return t1+t2+t3;      
  }
}

int zwidget :: quick_decision (struct state s,int i)
{
  struct state s1;
  int cc,mc,ac,go;

  s1 = is_obstacle_present (s,-9,6);
  cc = triple_recursive_function (s1,0);
  
  s1 = is_obstacle_present (s,0,6);
  mc = triple_recursive_function (s1,0);

  s1 = is_obstacle_present (s,9,6);
  ac = triple_recursive_function (s1,0);

  if (mc >= cc)
  {
    if (mc >= ac)
    {
      go = 2;
    }
    else
    {
      go = ANTI_CLOCKWISE;
    }
  }
  else
  {
    if (cc > ac)
    {
      go = CLOCKWISE;
    }
    else
    {
      go = ANTI_CLOCKWISE;
    }
  }

  return go;
}

void zwidget :: make_decision_for (int zat)
{
  struct state s,s1;
  int go=2;
  
  s.x = z1[zat].curx;
  s.y = z1[zat].cury;
  s.dir = z1[zat].cdir;

  s1 = is_obstacle_present (s,0,90);
  
  if (!s1.flag)
  {
      if (s1.finali < 20)
      {
        z1[zat].pressdir = quick_decision (s,zat);
        return; 
      }
      s1 = is_obstacle_present (s,-9,60);
      if (s1.flag)
        go = CLOCKWISE;
      else
        go = ANTI_CLOCKWISE;
      
  }
  
  
  z1[zat].pressdir = go;  
}

////////////////////////////////////////////////////////////////////////////////

//make a single move for every snake in the game
void zwidget :: makemove ()
{
 
  float backx,backy;
  int t1,t2,t3,t4;
  
  for (int i=0;i<zcount;i++)
    if (z1[i] . isalive)
      {
      backx=z1[i].curx;backy=z1[i].cury;  
        
	

	z1[i] . move ();
   
   t1 = int(nearbyint(z1[i] . curx));
   t2 = int(nearbyint(z1[i] . cury));

     	
	if (t1 >=dx || t1 <= 0)
	  {
        int alive=0;
            z1[i] . setdead ();
	    score_changed = true;
	    
            for (int i=0;i<zcount;i++)
              if (z1[i] . isalive) 
		{ ++alive ; ++z1[i].points ; }
            if (alive <= 1)
		{  stopgame ();}
        update ();
        continue;
	  }

        if (t2 >=dy || t2 <= 0) 
	  {
            int alive=0;
            z1[i] . setdead ();
	    score_changed = true;
            for (int i=0;i<zcount;i++)
	      if (z1[i] . isalive) {++alive ; ++z1[i].points; }
            if (alive <= 1)
		{ stopgame ();}
        update ();
        continue;
	  }

   t3 = int(nearbyint(backx));
   t4 = int(nearbyint(backy));

	if (t1 == t3 && t2 == t4)continue;
    

	bool dead=false;

	if ( (marker [ t1 ] [ t2 ] == true))
		dead = true;
	
	if (abs (t3 - t1) == 1 &&
	    abs (t4 - t2) == 1)
	{
		if (marker [ t3 ] [ t2 ] == true &&
		    marker [ t1 ] [ t4 ] == true)
			dead = true;
	}

	if (dead)
	  {
            int alive=0;
            z1[i] . setdead ();
	    score_changed = true;
            for (int i=0;i<zcount;i++) 
              if (z1[i] . isalive) { ++ alive; ++z1[i].points; }
            if (alive <= 1)
		{ stopgame ();}
	  }

        if (draw == false)continue;

    marker [t1]    [t2] = true;
	update ();

      }
  
}

//set the direction according to the pressed key
void zwidget :: keymove ()
{ 
  for (int i=0;i<zcount;i++)
  {
    if (z1[i].iscpu && z1[i].isalive)
	  make_decision_for (i);
    z1[i] . setdir ();
    }
}


void zwidget::paintEvent (QPaintEvent *event)
{
  QPainter painter (this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  QFont serifFont("Times");
  int j=0;
  switch (state)
    {

      
      //Display the title
    case 1:
      painter.fillRect (0,0,maxx,maxy,QBrush (Qt::black));
      painter.setPen(QPen(Qt::darkYellow, 5, Qt::SolidLine, Qt::RoundCap));
      serifFont . setPointSize (75);
      serifFont . setBold (true);
      painter.setFont (serifFont);
      painter.drawText (250,250,"d-Kurv");
      serifFont . setPointSize (25);
      serifFont . setItalic (true);
      painter.setFont (serifFont);
      break;
      

      //Select the controls
    case 2:
      painter.fillRect (0,0,maxx,maxy,QBrush (Qt::black));
      painter.setPen(QPen(Qt::darkYellow, 5, Qt::SolidLine, Qt::RoundCap));
      serifFont . setPointSize (25);
      serifFont . setBold (true);
      painter.setFont (serifFont);
      painter.drawText (100,100,"Select the controls");
      serifFont . setBold (false);          
      painter.setFont (serifFont);
      painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[0] == 1)
	painter.drawText (500,200,"HUMAN");
      else if (p[0] == 2)
	painter.drawText (500,200,"CPU");

      painter.setFont (serifFont);
      painter.drawText (100,200,"Player 1\t\tQ\tA\t\t\t");
      painter.setPen(QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[1] == 1)
	painter.drawText (700,250,"HUMAN");
      else if (p[1] == 2)
	painter.drawText (700,250,"CPU");


      painter.setFont (serifFont);
      painter.drawText (100,250,"Player 2\t\tX\tC\t\t\t");
      painter.setPen(QPen(Qt::gray, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[2] == 1)
	painter.drawText (500,300,"HUMAN");
      else if (p[2] == 2)
	painter.drawText (500,300,"CPU");

      painter.setFont (serifFont);
      painter.drawText (100,300,"Player 3\t\tM\t,\t\t\t");
      painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[3] == 1)
	painter.drawText (700,350,"HUMAN");
      else if (p[3] == 2)
	painter.drawText (700,350,"CPU");


      painter.setFont (serifFont);
      painter.drawText (100,350,"Player 4\t\tV\tB\t\t\t");
      painter.setPen(QPen(Qt::magenta, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[4] == 1)
	painter.drawText (500,400,"HUMAN");
      else if (p[4] == 2)
	painter.drawText (500,400,"CPU");


      painter.setFont (serifFont);
      painter.drawText (100,400,"Player 5\t\tUp\tRt\t\t\t");
      painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));
      
      if (p[5] == 1)
	painter.drawText (700,450,"HUMAN");
      else if (p[5] == 2)
	painter.drawText (700,450,"CPU");


      painter.setFont (serifFont);
      painter.drawText (100,450,"Player 6\t\tLt\tDn\t\t\t");
      
      painter.setFont (serifFont);
      painter.drawText (100,600,"Press keys 1 - 6 to select human/cpu ");
      
      
          
      break;
      
    case 3:

	  serifFont . setPointSize (75);
	  painter.setFont (serifFont); 
	  painter.fillRect (dx+10,0,maxx-dx,dy,QBrush (Qt::black));
	  j=0;
	  for (int i=0;i<6;i++)
	    {
	      if (p[i])
		{ 
		  int x=dx+10,y=i*(dy/6) + 75;
		  QString str;
		  QTextStream (&str) <<z1[j].points;
		  painter.setPen (z1[j++].color);
		  painter.drawText (x,y,str);
		}
	    }
	   break;
          
      //Game running
    case 4:
      
      if (clearbackground )
	{
	  painter.fillRect (0,0,maxx,maxy,QBrush (Qt::black));  
	  painter.setPen(QPen(Qt::darkYellow, 5, Qt::SolidLine, Qt::RoundCap));
	  painter.drawRect (0,0,dx,dy);		
    clearbackground = false;
	}
      
      if (score_changed)
	{ 
	  serifFont . setPointSize (75);
	  painter.setFont (serifFont); 
	  painter.fillRect (dx+10,0,maxx-dx,dy,QBrush (Qt::black));
	  int j=0;
	  for (int i=0;i<6;i++)
	    {
	      if (p[i])
		{ 
		  int x=dx+10,y=i*(dy/6) + 75;
		  QString str;
		  QTextStream (&str) <<z1[j].points;
		  painter.setPen (z1[j++].color);
		  painter.drawText (x,y,str);
		}
	    }
	  score_changed = false;
	}
      
      painter.setPen(QPen(Qt::darkYellow, 5, Qt::SolidLine, Qt::RoundCap));		
      // painter.drawRect (dx+10,0,maxx-dx,dy/6);
      
      for (int i=0;i<zcount ; i++)
	{
	  if (!z1[i].isalive)continue;
	  pen = new QPen (z1[i].color);
	  pen -> setWidth (SNAKE_THICKNESS);    
	  painter.setPen (*pen);
	  painter.drawPoint (int (nearbyint (z1[i] . curx)),int (nearbyint (z1[i] . cury)));
      delete (pen);          
	}
      
      break;
      
    case 6:
      
      QString str;
      painter.fillRect (0,0,maxx,maxy,QBrush (Qt::black));
      serifFont . setPointSize (30);
      painter.setFont (serifFont); 
      
      int j=0;
      for (int i=0;i<6;i++)
	{
	  if (p[i])
	    { 
	      int x=dx/2,y=100 + i*75;
	      QString str;
	      QTextStream (&str) <<z1[j].points;
	      painter.setPen (z1[j++].color);
	      painter.drawText (x,y,str);
	    }
	}
      
      
    }
  

}

//Record the keys pressed
void zwidget :: keyPressEvent (QKeyEvent *event)
{
  int key = event -> key (),i;
  
  switch (state)
  {
    case 1:
      if (key == Qt::Key_Escape)
        close ();
      else if (key == Qt::Key_Space)
       {
         state = 2;
          for (i=0;i<6;i++)
	           p[i] = false;
          update ();
       }
      break;
    case 2:
      if (key == Qt::Key_Escape)
      {
        state = 1;update ();return;
      }
    {
      if (key == Qt::Key_1)
	p[0] = (p[0]+1)%3;
      
      if (key == Qt::Key_2)
	p[1] = (p[1]+1)%3;
      
      if (key == Qt::Key_3)
	p[2] = (p[2]+1)%3;
      
      if (key == Qt::Key_4)
	p[3] = (p[3]+1)%3;
      
      if (key == Qt::Key_5)
	p[4] = (p[4]+1)%3;
      
      if (key == Qt::Key_6)
	p[5] = (p[5]+1)%3;
      
      zcount = 0;
      
      for (i=0;i<6;i++)
	    if (p[i]) 
	    {
	       if (i == 3)mplayer=zcount;
	       zcount ++;
	    }
      
      if (key == Qt::Key_Space)
	if (zcount > 1) 
        {
          setupsnakes ();
          state = 4;
          startgame ();
          clearbackground = true;
          update ();
        }
	else
		QMessageBox :: information (NULL,"Cannot start the game",
					    "You need a minimum of 2 players to start the game");
      else  
	update ();
	break;
    }
  
  case 3:
    {
      if (key == Qt::Key_Escape)
        {
          state = 2;
           for (i=0;i<6;i++)
      	     p[i] = false;
          update ();
        }
        
      if (key == Qt::Key_Space)
        {
          state = 4;
          startgame ();
          clearbackground = true;
          update ();
        }
    }
    break;
  case 4:
    {
      if (key == Qt::Key_Escape)
        {
          state = 2;
           for (i=0;i<6;i++)
      	     p[i] = false;
          update ();
        }


      for (int i=0;i<zcount ; i++)
	{
	        if (z1[i].iscpu)continue;
		if (key == z1[i] . clkey)
		  {z1[i] . presseddir (1);break;}
		else if (key == z1[i] . anclkey)
		  {z1[i] . presseddir (0);break;}
	}
    }
    break;
  case 6:
      if(key == Qt::Key_Space)
    {
      state = 2;
      for (i=0;i<6;i++)
      	p[i] = false;
      update ();
    }

  }
}

//key release event considered for continous pressing of the key
void zwidget :: keyReleaseEvent (QKeyEvent *event)
{
  int i,key = event -> key ();
  
  for (i=0;i<zcount;i++)
    {
      if (z1[i].iscpu)continue;
      if (key == z1[i].clkey || key == z1[i].anclkey)
	z1[i].presseddir (2);
    }
  
}

//record the mouse movements
void zwidget :: mousePressEvent (QMouseEvent *event)
{
  int button = event -> button ();
  if (button == z1[mplayer] . clkey)
    z1[mplayer]. pressdir = (1);
  else if (button == z1[mplayer] . anclkey)
    z1[mplayer]. pressdir = (0);
}

void zwidget :: mouseReleaseEvent (QMouseEvent *event)
{
  z1[mplayer]. pressdir = (2);
}

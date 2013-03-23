/*******************************************************************************
 *
 *    Code written by Ramesh (rameshg87@gmail.com)
 *    Release under Open Source
 *
 *****************************************************************************/  

#include <QApplication>
#include "kurve.h"

int main (int argc,char **argv)
{
  QApplication app (argc,argv);
  zwidget *zw = new zwidget;
  return app.exec ();
  
}

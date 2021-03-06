/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori <francesco.nori@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#ifndef YARP_YARPMOTORGUI_GTK_GTKMESSAGES_H
#define YARP_YARPMOTORGUI_GTK_GTKMESSAGES_H

///////////GTK///////////
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <yarp/os/Time.h>

extern GtkWidget *window;

GtkWidget * dialog_message_generator(GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);
bool dialog_message (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);
bool dialog_severe_error (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);

//*********************************************************************************
// This callback exits from the gtk_main() main loop when the main window is closed
//static void destroy_main (GtkWindow *window,	gpointer   user_data)
//{
//  gtk_widget_destroy (GTK_WIDGET(window));
//  gtk_main_quit ();
//}

#endif // YARP_YARPMOTORGUI_GTK_GTKMESSAGES_H

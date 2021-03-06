// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GOOCANVASMM_CANVAS_H
#define _GOOCANVASMM_CANVAS_H


#include <glibmm.h>

/* Copyright (C) 1998-2006 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <gtkmm/adjustment.h>
#include <gtkmm/container.h>
#include <gdkmm/cursor.h>
#include <cairomm/context.h>
#include <gtkmm/papersize.h> //For Gtk::Unit
#include <goocanvasmm/item.h>
#include <goocanvasmm/itemmodel.h>
#include <goocanvasmm/itemsimple.h>
#include <goocanvasmm/style.h>
#include <goocanvasmm/widget.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GooCanvas GooCanvas;
typedef struct _GooCanvasClass GooCanvasClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Goocanvas
{ class Canvas_Class; } // namespace Goocanvas
namespace Goocanvas
{

/** The main canvas widget, containing a number of canvas items.
 * Add child items (see Item) to the root item (see get_root_item()), for instance with Item::add_child().
 */

class Canvas :
  public Gtk::Container
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Canvas CppObjectType;
  typedef Canvas_Class CppClassType;
  typedef GooCanvas BaseObjectType;
  typedef GooCanvasClass BaseClassType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  virtual ~Canvas();

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
  friend class Canvas_Class;
  static CppClassType canvas_class_;

  // noncopyable
  Canvas(const Canvas&);
  Canvas& operator=(const Canvas&);

protected:
  explicit Canvas(const Glib::ConstructParams& construct_params);
  explicit Canvas(GooCanvas* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GtkObject.
  GooCanvas*       gobj()       { return reinterpret_cast<GooCanvas*>(gobject_); }

  ///Provides access to the underlying C GtkObject.
  const GooCanvas* gobj() const { return reinterpret_cast<GooCanvas*>(gobject_); }


public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::
  virtual void on_set_scroll_adjustments(Gtk::Adjustment* hadjustment, Gtk::Adjustment* vadjustment);
  virtual void on_item_created(const Glib::RefPtr<Item>& item, const Glib::RefPtr<ItemModel>& model);


private:


public:
  Canvas();

  
  /** Gets the root item of the canvas, usually a Goo::CanvasGroup.
   * @return The root item, or <tt>0</tt> if there is no root item.
   */
  Glib::RefPtr<Item> get_root_item();
  
  /** Gets the root item of the canvas, usually a Goo::CanvasGroup.
   * @return The root item, or <tt>0</tt> if there is no root item.
   */
  Glib::RefPtr<const Item> get_root_item() const;
  
  /** Sets the root item of the canvas. Any existing canvas items are removed.
   * @param item The root canvas item.
   */
  void set_root_item(const Glib::RefPtr<Item>& item);

  
  /** Gets the root item model of the canvas.
   * @return The root item model, or <tt>0</tt> if there is no root item model.
   */
  Glib::RefPtr<ItemModel> get_root_item_model();
  
  /** Gets the root item model of the canvas.
   * @return The root item model, or <tt>0</tt> if there is no root item model.
   */
  Glib::RefPtr<const ItemModel> get_root_item_model() const;
  
  /** Sets the root item model of the canvas.
   * 
   * A hierarchy of canvas items will be created, corresponding to the hierarchy
   * of items in the model. Any current canvas items will be removed.
   * @param model A Goo::CanvasItemModel.
   */
  void set_root_item_model(const Glib::RefPtr<ItemModel>& model);

  
  Glib::RefPtr<Item> get_static_root_item();
  
  Glib::RefPtr<const Item> get_static_root_item() const;
  
  void set_static_root_item(const Glib::RefPtr<Item>& item);

  
  Glib::RefPtr<ItemModel> get_static_root_item_model();
  
  Glib::RefPtr<const ItemModel> get_static_root_item_model() const;
  
  void set_static_root_item_model(const Glib::RefPtr<ItemModel>& model);

  
  /** Gets the canvas item associated with the given Goo::CanvasItemModel.
   * This is only useful when set_root_item_model() has been used to
   * set a model for the canvas.
   * 
   * For simple applications you can use get_item() to set up
   * signal handlers for your items, e.g.
   * 
   * @code
   * item = goo_canvas_get_item (GOO_CANVAS (canvas), my_item);
   * g_signal_connect (item, "button_press_event",
   * (GtkSignalFunc) on_my_item_button_press, <tt>0</tt>);
   * @endcode
   * 
   * More complex applications may want to use the Goo::Canvas::item-created
   * signal to hook up their signal handlers.
   * @param model A Goo::CanvasItemModel.
   * @return The canvas item corresponding to the given Goo::CanvasItemModel,
   * or <tt>0</tt> if no canvas item has been created for it yet.
   */
  Glib::RefPtr<Item> get_item(const Glib::RefPtr<ItemModel>& model);
  
  /** Gets the canvas item associated with the given Goo::CanvasItemModel.
   * This is only useful when set_root_item_model() has been used to
   * set a model for the canvas.
   * 
   * For simple applications you can use get_item() to set up
   * signal handlers for your items, e.g.
   * 
   * @code
   * item = goo_canvas_get_item (GOO_CANVAS (canvas), my_item);
   * g_signal_connect (item, "button_press_event",
   * (GtkSignalFunc) on_my_item_button_press, <tt>0</tt>);
   * @endcode
   * 
   * More complex applications may want to use the Goo::Canvas::item-created
   * signal to hook up their signal handlers.
   * @param model A Goo::CanvasItemModel.
   * @return The canvas item corresponding to the given Goo::CanvasItemModel,
   * or <tt>0</tt> if no canvas item has been created for it yet.
   */
  Glib::RefPtr<const Item> get_item(const Glib::RefPtr<ItemModel>& model) const;

  
  /** Gets the item at the given point.
   * @param x The x coordinate of the point.
   * @param y The y coordinate of the point.
   * @param is_pointer_event <tt>true</tt> if the "pointer-events" property of
   * items should be used to determine which parts of the item are tested.
   * @return The item found at the given point, or <tt>0</tt> if no item was found.
   */
  Glib::RefPtr<Item> get_item_at(double x, double y, bool is_pointer_event);
  
  /** Gets the item at the given point.
   * @param x The x coordinate of the point.
   * @param y The y coordinate of the point.
   * @param is_pointer_event <tt>true</tt> if the "pointer-events" property of
   * items should be used to determine which parts of the item are tested.
   * @return The item found at the given point, or <tt>0</tt> if no item was found.
   */
  Glib::RefPtr<const Item> get_item_at(double x, double y, bool is_pointer_event) const;

#
#


#


  /** Gets all items at the given point.
   * @param x The x coordinate of the point.
   * @param y The y coordinate of the point.
   * @param is_pointer_event <tt>true</tt> if the "pointer-events" property of
   * items should be used to determine which parts of the item are tested.
   * @return A list of items found at the given point, with the top item at
   * the start of the list, or <tt>0</tt> if no items were found. The list must be
   * freed with Glib::list_free().
   */
  Glib::ListHandle< Glib::RefPtr<Item> > get_items_at(double x, double y, bool is_pointer_event);
  
  /** Gets all items at the given point.
   * @param x The x coordinate of the point.
   * @param y The y coordinate of the point.
   * @param is_pointer_event <tt>true</tt> if the "pointer-events" property of
   * items should be used to determine which parts of the item are tested.
   * @return A list of items found at the given point, with the top item at
   * the start of the list, or <tt>0</tt> if no items were found. The list must be
   * freed with Glib::list_free().
   */
  Glib::ListHandle< Glib::RefPtr<const Item> > get_items_at(double x, double y, bool is_pointer_event) const;
  
  /** Gets a list of items inside or outside a given area.
   * @param area The area to compare with each item's bounds.
   * @param inside_area <tt>true</tt> if items inside @a area should be returned, or <tt>false</tt> if
   * items outside @a area should be returned.
   * @param allow_overlaps <tt>true</tt> if items which are partly inside and partly outside
   * should be returned.
   * @param include_containers <tt>true</tt> if containers should be checked as well as
   * normal items.
   * @return A list of items in the given area, or <tt>0</tt> if no items are found.
   * The list should be freed with Glib::list_free().
   */
  Glib::ListHandle< Glib::RefPtr<Item> > get_items_in_area(const Bounds& area, bool inside_area, bool allow_overlaps, bool include_containers);
  
  /** Gets a list of items inside or outside a given area.
   * @param area The area to compare with each item's bounds.
   * @param inside_area <tt>true</tt> if items inside @a area should be returned, or <tt>false</tt> if
   * items outside @a area should be returned.
   * @param allow_overlaps <tt>true</tt> if items which are partly inside and partly outside
   * should be returned.
   * @param include_containers <tt>true</tt> if containers should be checked as well as
   * normal items.
   * @return A list of items in the given area, or <tt>0</tt> if no items are found.
   * The list should be freed with Glib::list_free().
   */
  Glib::ListHandle< Glib::RefPtr<const Item> > get_items_in_area(const Bounds& area, bool inside_area, bool allow_overlaps, bool include_containers) const;

  
  /** Gets the current scale of the canvas.
   * 
   * The scale specifies the magnification factor of the canvas, e.g. if an item
   * has a width of 2 pixels and the scale is set to 3, it will be displayed with
   * a width of 2 x 3 = 6 pixels.
   * @return The current scale setting.
   */
  double get_scale() const;
  
  /** Sets the scale of the canvas.
   * 
   * The scale specifies the magnification factor of the canvas, e.g. if an item
   * has a width of 2 pixels and the scale is set to 3, it will be displayed with
   * a width of 2 x 3 = 6 pixels.
   * @param scale The new scale setting.
   */
  void set_scale(double scale);


  /** Gets the bounds of the canvas, in canvas units.
   * 
   * By default, canvas units are pixels, though the Goo::Canvas:units property
   * can be used to change the units to points, inches or millimeters.
   * @param left A pointer to a #gdouble to return the left edge, or <tt>0</tt>.
   * @param top A pointer to a #gdouble to return the top edge, or <tt>0</tt>.
   * @param right A pointer to a #gdouble to return the right edge, or <tt>0</tt>.
   * @param bottom A pointer to a #gdouble to return the bottom edge, or <tt>0</tt>.
   */
  void get_bounds(double& left, double& top, double& right, double& bottom) const;
  void get_bounds(Bounds& bounds) const;

  
  /** Sets the bounds of the Goo::Canvas, in canvas units.
   * 
   * By default, canvas units are pixels, though the Goo::Canvas:units property
   * can be used to change the units to points, inches or millimeters.
   * @param left The left edge.
   * @param top The top edge.
   * @param right The right edge.
   * @param bottom The bottom edge.
   */
  void set_bounds(double left, double top, double right, double bottom);
  void set_bounds(const Bounds& bounds);

  
  /** Scrolls the canvas, placing the given point as close to the top-left of
   * the view as possible.
   * @param left The x coordinate to scroll to.
   * @param top The y coordinate to scroll to.
   */
  void scroll_to(double left, double top);

  
  /** Grabs the keyboard focus for the given item.
   * @param item The item to grab the focus.
   */
  void grab_focus(const Glib::RefPtr<Item>& item);

  
  /** Renders all or part of a canvas to the given cairo context.
   * @param cr A cairo context.
   * @param bounds The area to render, or <tt>0</tt> to render the entire canvas.
   * @param scale The scale to compare with each item's visibility
   * threshold to see if they should be rendered. This only affects items that
   * have their visibility set to Goo::CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD.
   */
  void render(const Cairo::RefPtr<Cairo::Context>& context, const Bounds& bounds, double scale = 0.0);
  void render(const Cairo::RefPtr<Cairo::Context>& context, double scale = 0.0);

  
  /** Converts a coordinate from the canvas coordinate space to pixels.
   * 
   * The canvas coordinate space is specified in the call to
   * set_bounds().
   * 
   * The pixel coordinate space specifies pixels from the top-left of the entire
   * canvas window, according to the current scale setting.
   * See set_scale().
   * @param x A pointer to the x coordinate to convert.
   * @param y A pointer to the y coordinate to convert.
   */
  void convert_to_pixels(double& x, double& y) const;
  
  /** Converts a coordinate from pixels to the canvas coordinate space.
   * 
   * The pixel coordinate space specifies pixels from the top-left of the entire
   * canvas window, according to the current scale setting.
   * See set_scale().
   * 
   * The canvas coordinate space is specified in the call to
   * set_bounds().
   * @param x A pointer to the x coordinate to convert.
   * @param y A pointer to the y coordinate to convert.
   */
  void convert_from_pixels(double& x, double& y) const;
  
  /** Converts a coordinate from the canvas coordinate space to the given
   * item's coordinate space, applying all transformation matrices including the
   * item's own transformation matrix, if it has one.
   * @param item A Goo::CanvasItem.
   * @param x A pointer to the x coordinate to convert.
   * @param y A pointer to the y coordinate to convert.
   */
  void convert_to_item_space(const Glib::RefPtr<Item>& item, double& x, double& y) const;
  
  /** Converts a coordinate from the given item's coordinate space to the canvas
   * coordinate space, applying all transformation matrices including the
   * item's own transformation matrix, if it has one.
   * @param item A Goo::CanvasItem.
   * @param x A pointer to the x coordinate to convert.
   * @param y A pointer to the y coordinate to convert.
   */
  void convert_from_item_space(const Glib::RefPtr<Item>& item, double& x, double& y) const;
  
  void convert_bounds_to_item_space(const Glib::RefPtr<Item>& item, Bounds& bounds) const;

  //TODO: Use C++ types?
 

  /** Attempts to grab the pointer for the given item.
   * @param item The item to grab the pointer for.
   * @param event_mask The events to receive during the grab.
   * @param cursor The cursor to display during the grab, or <tt>0</tt>.
   * @param time The time of the event that lead to the pointer grab. This should
   * come from the relevant Gdk::Event.
   * @return Gdk::GRAB_SUCCESS if the grab succeeded.
   */
  Gdk::GrabStatus pointer_grab(const Glib::RefPtr<Item>& item, Gdk::EventMask mask, const Gdk::Cursor& cursor, guint32 time);
  Gdk::GrabStatus pointer_grab(const Glib::RefPtr<Item>& item, Gdk::EventMask mask, guint32 time);

  
  /** Ungrabs the pointer, if the given item has the pointer grab.
   * @param item The item that has the grab.
   * @param time The time of the event that lead to the pointer ungrab. This should
   * come from the relevant Gdk::Event.
   */
  void pointer_ungrab(const Glib::RefPtr<Item>& item, guint32 time);
  
  /** Attempts to grab the keyboard for the given item.
   * @param item The item to grab the keyboard for.
   * @param owner_events <tt>true</tt> if keyboard events for this application will be
   * reported normally, or <tt>false</tt> if all keyboard events will be reported with
   * respect to the grab item.
   * @param time The time of the event that lead to the keyboard grab. This should
   * come from the relevant Gdk::Event.
   * @return Gdk::GRAB_SUCCESS if the grab succeeded.
   */
  GdkGrabStatus keyboard_grab(const Glib::RefPtr<Item>& item, bool owner_events, guint32 time);
  
  /** Ungrabs the keyboard, if the given item has the keyboard grab.
   * @param item The item that has the keyboard grab.
   * @param time The time of the event that lead to the keyboard ungrab. This should
   * come from the relevant Gdk::Event.
   */
  void keyboard_ungrab(const Glib::RefPtr<Item>& item, guint32 time);

  
  /** Creates a cairo context, initialized with the default canvas settings.
   * @return A new cairo context. It should be freed with cairo_destroy().
   */
  Cairo::RefPtr<Cairo::Context> create_cairo_context();

  
  /** This function is only intended to be used when implementing new canvas
   * items, typically container items such as Goo::CanvasGroup.
   * 
   * It creates a new canvas item for the given item model, and recursively
   * creates items for any children.
   * 
   * It uses the create_item() virtual method if it has been set.
   * Subclasses of Goo::Canvas can define this method if they want to use
   * custom views for items.
   * 
   * It emits the Goo::Canvas::item-created signal after creating the view, so
   * application code can connect signal handlers to the new view if desired.
   * @param model The item model to create a canvas item for.
   * @return A new canvas item.
   */
  Glib::RefPtr<Item> create_item(const Glib::RefPtr<ItemModel>& model);
  
  /** This function is only intended to be used when implementing new canvas
   * items.
   * 
   * It should be called in the finalize method of Goo::CanvasItem
   * objects, to remove the canvas item from the Goo::Canvas's hash table.
   * @param model The item model whose canvas item is being finalized.
   */
  void unregister_item(const Glib::RefPtr<ItemModel>& model);

  
  /** This function is only intended to be used by subclasses of Goo::Canvas or
   * Goo::CanvasItem implementations.
   * 
   * It updates any items that need updating.
   * 
   * If the bounds of items change, they will request a redraw of the old and
   * new bounds so the display is updated correctly.
   */
  void update();
  
  /** This function is only intended to be used by subclasses of Goo::Canvas or
   * Goo::CanvasItem implementations.
   * 
   * It schedules an update of the Goo::Canvas. This will be performed in
   * the idle loop, after all pending events have been handled, but before
   * the canvas has been repainted.
   */
  void request_update();
  
  /** This function is only intended to be used by subclasses of Goo::Canvas or
   * Goo::CanvasItem implementations.
   * 
   * Requests that the given bounds be redrawn.
   * @param bounds The bounds to redraw.
   */
  void request_redraw(const Bounds& bounds);
  
  void request_item_redraw(const Bounds& bounds, bool is_static = false);
  
  /** Gets the default line width, which depends on the current units setting.
   * @return The default line width of the canvas.
   */
  double get_default_line_width() const;

  
  /** This function should only be used by Goo::CanvasWidget and subclass
   * implementations.
   * 
   * It registers a widget item with the canvas, so that the canvas can do the
   * necessary actions to move and resize the widget as needed.
   * @param witem A Goo::CanvasWidget item.
   */
  void register_widget_item(const Glib::RefPtr<Goocanvas::Widget>& widget);
  
  /** This function should only be used by Goo::CanvasWidget and subclass
   * implementations.
   * 
   * It unregisters a widget item from the canvas, when the item is no longer in
   * the canvas.
   * @param witem A Goo::CanvasWidget item.
   */
  void unregister_widget_item(const Glib::RefPtr<Goocanvas::Widget>& widget);

 
  /**
   * @par Prototype:
   * <tt>void on_my_%set_scroll_adjustments(Gtk::Adjustment* hadjustment, Gtk::Adjustment* vadjustment)</tt>
   */

  Glib::SignalProxy2< void,Gtk::Adjustment*,Gtk::Adjustment* > signal_set_scroll_adjustments();


  /**
   * @par Prototype:
   * <tt>void on_my_%item_created(const Glib::RefPtr<Item>& item, const Glib::RefPtr<ItemModel>& model)</tt>
   */

  Glib::SignalProxy2< void,const Glib::RefPtr<Item>&,const Glib::RefPtr<ItemModel>& > signal_item_created();


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_scale() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_scale() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_scale_x() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_scale_x() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_scale_y() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical magnification factor of the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_scale_y() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Where to place the canvas when it is smaller than the widget's allocated area.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Gtk::AnchorType> property_anchor() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Where to place the canvas when it is smaller than the widget's allocated area.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Gtk::AnchorType> property_anchor() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The x coordinate of the left edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_x1() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The x coordinate of the left edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_x1() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The y coordinate of the top edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_y1() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The y coordinate of the top edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_y1() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The x coordinate of the right edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_x2() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The x coordinate of the right edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_x2() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The y coordinate of the bottom edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_y2() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The y coordinate of the bottom edge of the canvas bounds, in canvas units.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_y2() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** If the bounds are automatically calculated based on the bounds of all the items in the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_automatic_bounds() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** If the bounds are automatically calculated based on the bounds of all the items in the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_automatic_bounds() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** If the automatic bounds are calculated from the origin.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_bounds_from_origin() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** If the automatic bounds are calculated from the origin.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_bounds_from_origin() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The padding added to the automatic bounds.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_bounds_padding() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The padding added to the automatic bounds.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_bounds_padding() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The units to use for the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Gtk::Unit> property_units() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The units to use for the canvas.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Gtk::Unit> property_units() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal resolution of the display, in dots per inch.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_resolution_x() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal resolution of the display, in dots per inch.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_resolution_x() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical resolution of the display, in dots per inch.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_resolution_y() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical resolution of the display, in dots per inch.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_resolution_y() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The color to use for the canvas background.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_WriteOnly<Glib::ustring> property_background_color() ;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The color to use for the canvas background, specified as a 24-bit integer value, 0xRRGGBB.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_WriteOnly<guint> property_background_color_rgb() ;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** If all item layout is done to the nearest integer.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_integer_layout() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** If all item layout is done to the nearest integer.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_integer_layout() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** If the background is cleared before the canvas is painted.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_clear_background() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** If the background is cleared before the canvas is painted.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_clear_background() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


protected:
    virtual Glib::RefPtr<Item> create_item_vfunc(const Glib::RefPtr<ItemModel>& model);


};

} //namespace Goocanvas


namespace Glib
{
  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Goocanvas::Canvas
   */
  Goocanvas::Canvas* wrap(GooCanvas* object, bool take_copy = false);
} //namespace Glib


#endif /* _GOOCANVASMM_CANVAS_H */


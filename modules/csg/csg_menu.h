/*************************************************************************/
/*  csg_menu.h                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef CSG_MENU_H
#define CSG_MENU_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "modules/csg/csg_gizmos.h"
#include "scene/gui/check_box.h"

class CSGShapeMenuPanelPlugin : public HBoxContainer {

	GDCLASS(CSGShapeMenuPanelPlugin, HBoxContainer);

	CheckBox *check_box;
	
	EditorNode *editor;
	CSGShapeSpatialGizmoPlugin *gizmos;
	//bool extra_handles;
	InputEventKey *input_event;
	ShortCut *shortcut;

protected:
	//void _node_removed(Node *p_node);
	static void _bind_methods();
	void toggle_extra_gizmos(bool);

public:
	//bool forward_gui_input(const Ref<InputEvent> &p_event);
	void set_gizmos(CSGShapeSpatialGizmoPlugin *p_gizmos);
	CheckBox *get_check_box();
	CSGShapeMenuPanelPlugin(EditorNode *p_editor);
	CSGShapeMenuPanelPlugin();
	~CSGShapeMenuPanelPlugin();
};

//class CSGEditorPlugin : public EditorPlugin {
//
//	GDCLASS(CSGEditorPlugin, EditorPlugin);
//
//	//CSGShapeMenuPanelPlugin *CSG_MENU_H;
//	EditorNode *editor;
//	String klass;
//
//public:
//	//virtual bool forward_canvas_gui_input(const Ref<InputEvent> &p_event) { return CSG_MENU_H->forward_gui_input(p_event); }
//
//	bool has_main_screen() const { return false; }
//	virtual String get_name() const { return klass; }
//
//	CSGEditorPlugin(EditorNode *p_node, CSGShapeMenuPanelPlugin *p_CSG_MENU_H, String p_class);
//	~CSGEditorPlugin();
//};
#endif // CSG_MENU_H

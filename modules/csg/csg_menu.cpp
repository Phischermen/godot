/*************************************************************************/
/*  csg_editor.cpp                                                       */
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

#include "csg_menu.h"

#include "editor/plugins/spatial_editor_plugin.h"
#include "core/os/keyboard.h"

void CSGShapeMenuPanelPlugin::_bind_methods() {
	//ClassDB::bind_method(D_METHOD("_node_removed"), &CSGEditor::_node_removed);
	ClassDB::bind_method(D_METHOD("toggle_extra_gizmos"), &CSGShapeMenuPanelPlugin::toggle_extra_gizmos);
}

void CSGShapeMenuPanelPlugin::toggle_extra_gizmos(bool p_extra_handles) {
	gizmos->set_extra_gizmos(p_extra_handles);
	SpatialEditor::get_singleton()->update_all_gizmos();
}

void CSGShapeMenuPanelPlugin::set_gizmos(CSGShapeSpatialGizmoPlugin *p_gizmos) {
	gizmos = p_gizmos;
}

CheckBox *CSGShapeMenuPanelPlugin::get_check_box() {
	return check_box;
}


//bool CSGShapeMenuPanelPlugin::forward_gui_input(const Ref<InputEvent> &p_event) {
//	print_line("Crossing my fingers.");
//	return false;
//}

CSGShapeMenuPanelPlugin::CSGShapeMenuPanelPlugin(EditorNode *p_editor) {
	print_line("CSGShapeMenuPanelPlugin constructor called");
	editor = p_editor;

	input_event = memnew(InputEventKey);
	shortcut = memnew(ShortCut);

	input_event->set_scancode(KEY_ALT);
	shortcut->set_shortcut(input_event);

	add_child(memnew(VSeparator));
	check_box = memnew(CheckBox);
	add_child(check_box);
	check_box->connect("toggled", this, "toggle_extra_gizmos");
	check_box->set_shortcut(shortcut);
	check_box->set_action_mode(BaseButton::ActionMode::ACTION_MODE_BUTTON_PRESS);
	check_box->set_text("Extra Gizmos");
	check_box->set_tooltip("Show extra gizmos to adjust shape and translation at the same time.");
}

CSGShapeMenuPanelPlugin::CSGShapeMenuPanelPlugin() {
	print_line("CSGShapeMenuPanelPlugin default constructor called");
	input_event = NULL;
	shortcut = NULL;
}

CSGShapeMenuPanelPlugin::~CSGShapeMenuPanelPlugin() {
	print_line("CSGShapeMenuPanelPlugin destructor called");
	if (input_event) {
		print_line("input_event_deleted");
		memdelete(input_event);
		input_event = NULL;
	}
	if (shortcut) {
		print_line("shortcut_deleted");
		memdelete(shortcut);
		shortcut = NULL;
	}
}
